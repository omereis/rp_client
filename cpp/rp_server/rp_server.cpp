/******************************************************************************\
|                               rp_server.cpp                                  |
\******************************************************************************/

#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <mutex>
#include <thread>

using namespace std;
#include "rp_setup.h"
#include "misc.h"
#include "timer.h"
#include "bd_types.h"
#include "calc_mca.h"

#include "jsoncpp/json/json.h"
//-----------------------------------------------------------------------------
TFloatVecQueue g_qPulses;

TFloatVecQueue g_qDebug;

bool g_fRunning = false;
TMcaParams g_mca_params;
TCalcMca g_mca_calculator;
static const char *g_szReadPulse = "ReadPulse";
static const char *g_szReadMca   = "ReadMca";
static const char *g_szSampling  = "Sampling";
static const char *g_szStatus    = "Status";
static const char *g_szAll       = "All";
static const char *g_szError     = "Error";
static const char *g_szReset     = "Reset";
static const char *g_szMCA       = "MCA";
bool g_fMca = false;
//-----------------------------------------------------------------------------
std::string HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup);
std::string HandleRead(Json::Value &jRead, TRedPitayaSetup &rp_setup);
std::string HandleSampling(Json::Value &jSampling, TRedPitayaSetup &rp_setup);
std::string HandleMCA(Json::Value &jMCA, TRedPitayaSetup &rp_setup);
void AddPulse ();
void ExportDebugPulse(TFloatVecQueue &qDebug);
void SafeStartStop (bool fCommand);
bool SafeGetStatus ();
size_t SafeQueueSize ();
void SafeSetMca (bool fOnOff);
bool SafeGetMca ();
TMcaParams SafeGetMcaParams ();
void SafeSetMcaParams (const TMcaParams &params);
void SafeAddToMca (const TFloatVec &vPulse);
void SafeResetMca();
void SafeReadMca (Json::Value &jResult);
//-----------------------------------------------------------------------------
int main (void)
{
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
	Json::Value root;
	Json::Reader reader;
    TRedPitayaSetup rp_setup;
    string strReply;
    Timer t;

    Json::Value jSetup = rp_setup.AsJson();
    rp_setup.LoadFromJson("rp_setup.json");
    g_mca_calculator.SetParams (rp_setup.GetMcaParams());
    printf ("Setup: %s\n", StringifyJson(jSetup).c_str());
    t.setInterval (AddPulse, 1000);
    while (1) {
        char buffer [1024];
        zmq_recv (responder, buffer, 1024, 0);
        std::string strJson = buffer;
        printf ("Received Message:\n%s\n", strJson.c_str());
        if (reader.parse (strJson, root)) {
        	printf ("Message parsed\n");
            if (!root["setup"].isNull())
                strReply = HandleSetup(root["setup"], rp_setup);
            else if (!root[g_szReadPulse].isNull())
                strReply = HandleRead(root[g_szReadPulse], rp_setup);
            else if (!root[g_szSampling].isNull())
                strReply = HandleSampling(root[g_szSampling], rp_setup);
            else if (!root[g_szMCA].isNull())
                strReply = HandleMCA(root[g_szMCA], rp_setup);
        }
        sleep (1);          //  Do some 'work'
        if (strReply.length() == 0)
            strReply = std::string("World");
        //if (strReply.length() > 0)
            zmq_send (responder, strReply.c_str(), strReply.length(), 0);
        //else
            //zmq_send (responder, "World", 5, 0);
    }
    return 0;
}

//-----------------------------------------------------------------------------

std::string HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup)
{
    std::string strReply;

    try {
        Json::Value jRead = jSetup["read"], jNew;
    	if (jRead.isNull()) {
            jNew = rp_setup.UpdateFromJson(jSetup);
            rp_setup.SaveToJson("rp_setup.json");
            strReply = StringifyJson(jNew);
		}
        else {
            string strCmd = jRead.asString();
            strCmd = ToLower(strCmd);
            if (strCmd == g_szSampling)
                strReply = StringifyJson (rp_setup.AsJson());
            else if (strCmd == "applications") {
                Json::Value j = rp_setup.McaAsJson();
                strReply = StringifyJson (j);//rp_setup.McaAsJson());
			}
        }
		if (strReply.length() == 0)
	        strReply = StringifyJson (rp_setup.AsJson());
    }
    catch (std::exception &err) {
        Json::Value root;

        root["error"] = err.what();
        strReply = StringifyJson (root);
    }
    return (strReply);
}
//-----------------------------------------------------------------------------

std::string HandleRead(Json::Value &jRead, TRedPitayaSetup &rp_setup)
{
    TFloatVec vPulse;
    TFloatVec::iterator i;
	mutex mtx;
    std::string strReply, strNumber, strPulses;
    char szNum[128];
    Json::Value jAllPulses(Json::arrayValue), jPulse(Json::arrayValue);
    int n, nPulses;
    
    try {
        strPulses = jRead.asString();
        nPulses = std::stoi(strPulses);
        if (nPulses <= 0)
            nPulses = (int) g_qPulses.size();
        printf ("Reading pulse");
        if (SafeQueueSize () > 0) {
            for (n=0 ; n < nPulses ; n++) {
        	    mtx.lock ();
        	    vPulse = g_qPulses.back();
        	    g_qPulses.pop();
        	    mtx.unlock ();
                for (i=vPulse.begin() ; i != vPulse.end() ; i++) {
                    sprintf (szNum, "%.3f", *i);
                    strNumber = std::string (szNum);
                    jPulse.append (*i);
                }
                jAllPulses.append(jPulse);
                jPulse.clear();
            }
        }
        strReply = StringifyJson(jAllPulses);
        ExportDebugPulse(g_qDebug);
    }
    catch (std::exception &exp) {
        strReply = std::string("Runtime error in '': ") + std::string (exp.what());
    }
    return (strReply);
}
//-----------------------------------------------------------------------------

size_t SafeQueueSize ()
{
    size_t s;
    mutex mtx;

    mtx.lock ();
    s = g_qPulses.size();
    mtx.unlock ();
    return (s);
}
//-----------------------------------------------------------------------------

void SafeStartStop (bool fCommand)
{
	mutex mtx;

    mtx.lock();
    g_fRunning = fCommand;
    mtx.unlock();
}
//-----------------------------------------------------------------------------

bool SafeGetStatus ()
{
	mutex mtx;
    bool fRunning;

    mtx.lock();
    fRunning = g_fRunning;
    mtx.unlock();
    return (fRunning);
}
//-----------------------------------------------------------------------------

void SafeSetMca (bool fOnOff)
{
	mutex mtx;

    mtx.lock();
    g_fMca = fOnOff;
    mtx.unlock();
}
//-----------------------------------------------------------------------------

bool SafeGetMca ()
{
	mutex mtx;
    bool fRunning;

    mtx.lock();
    fRunning = g_fMca;
    mtx.unlock();
    return (fRunning);
}
//-----------------------------------------------------------------------------

TMcaParams SafeGetMcaParams ()
{
	mutex mtx;
    TMcaParams params;

    mtx.lock();
    params = g_mca_params;
    mtx.unlock();
    return (params);
}
//-----------------------------------------------------------------------------

void SafeSetMcaParams (const TMcaParams &params)
{
	mutex mtx;

    mtx.lock();
    g_mca_params = params;
    mtx.unlock();
}
//-----------------------------------------------------------------------------

std::string HandleSampling(Json::Value &jSampling, TRedPitayaSetup &rp_setup)
{
    std::string strResult;
    Json::Value jResult;
    bool fCommandOK;

    try {
        fCommandOK = true;
        string strCmd = ToLower (jSampling.asString());
        if (strCmd == "true")
            SafeStartStop (true);
        else if (strCmd == "false")
            SafeStartStop (false);
        else if (strCmd == "status")
            fCommandOK = true;
        else
            fCommandOK = false;
        if (fCommandOK)
            jResult[g_szSampling] = (SafeGetStatus () ? true : false);
            //jResult["sampling"] = (SafeGetStatus () ? "Running" : "stopped");
        else
            jResult[g_szSampling] = "Command Unknown";
    }
    catch (std::exception &exp) {
        jResult[g_szSampling] = exp.what();
    }
    strResult = StringifyJson(jResult);
    return (strResult);
}
//-----------------------------------------------------------------------------

std::string HandleMCA(Json::Value &jMCA, TRedPitayaSetup &rp_setup)
{
    std::string strResult;
    Json::Value jResult;

    try {
        string str = jMCA.asString();
        if (str == g_szStatus)
            jResult[g_szStatus] = SafeGetMca();
        else if (str == "true") {
            SafeSetMca(true);
            jResult[g_szStatus] = SafeGetMca();
        }
        else if (str == "false") {
            SafeSetMca(false);
            jResult[g_szStatus] = SafeGetMca();
        }
        else if (str == g_szReset) {
            SafeResetMca();
            jResult[g_szStatus] = g_szReset;
        }
        else if (str == g_szReadMca)
            SafeReadMca (jResult);
    }
    catch (std::exception &exp) {
        jResult[g_szError] = exp.what();
    }
    return (StringifyJson(jResult));
}
//-----------------------------------------------------------------------------

void SafeReadMca (Json::Value &jResult)
{
    mutex mtx;
    TFloatVec vSpectrum;
    TFloatVec::iterator i;

    mtx.lock();
    g_mca_calculator.GetSpectrum (vSpectrum);
    mtx.unlock();
    for (i=vSpectrum.begin() ; i != vSpectrum.end() ; i++)
        jResult[g_szMCA].append(*i);
}
//-----------------------------------------------------------------------------

void SafeResetMca()
{
    mutex mtx;

    mtx.lock();
    g_mca_calculator.ResetSpectrum ();
    mtx.unlock();
}
//-----------------------------------------------------------------------------

float VectorMax (const TFloatVec &v)
{
    TFloatVec::const_iterator i=v.begin();
    float fMax = *i;

    for ( ; i != v.end() ; i++)
        if (*i > fMax)
            fMax = *i;
    return (fMax);
}
//-----------------------------------------------------------------------------

void AddPulse ()
{
    TFloatVec vPulse;
    TFloatVec::iterator i;
	mutex mtx;
    float fDelta;

    if ((SafeGetStatus ())) {
        if (ReadVectorFromFile ("pulse.csv", vPulse)) {
            float f, fMax = VectorMax (vPulse) * 0.1;

            for (i=vPulse.begin() ; i != vPulse.end() ; i++) {
                f = *i;
                f += (rand() / RAND_MAX) * fMax;
                *i = f;
            }
            mtx.lock ();
            if (g_qDebug.size() < 10)
                g_qDebug.push (vPulse);
            g_qPulses.push(vPulse);
            while (g_qPulses.size() > 1000)
                g_qPulses.pop();
            mtx.unlock ();
            if (SafeGetMca ())
                SafeAddToMca (vPulse);
        }
    }
}
//-----------------------------------------------------------------------------

void SafeAddToMca (const TFloatVec &vPulse)
{
	mutex mtx;

    mtx.lock ();
    g_mca_calculator.NewPulse (vPulse);
    mtx.unlock ();
}
//-----------------------------------------------------------------------------

void ExportDebugPulse(TFloatVecQueue &qDebug)
{
/*
    vector<TFloatVec> mtxPulses;
    TFloatVev v;

    for (i=qDebug.begin() ; i != qDebug.end() ; i++) {
        v = qPulses.back();
        qPulses.pop();
        mtxPulses.push_back(*i);
    }
*/
}
