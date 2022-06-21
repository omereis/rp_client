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
#include "pulse_info.h"

#include "jsoncpp/json/json.h"
//-----------------------------------------------------------------------------
TFloatVecQueue g_qPulses;
TFloatVecQueue g_qDebug;

TPulseInfoVec g_vPulsesInfo;
bool g_fRunning = false;
TMcaParams g_mca_params;
TCalcMca g_mca_calculator;
static const char *g_szReadData = "read_data";
static const char *g_szBufferLength = "buffer_length";
static const char *g_szReadMca   = "rea_dmca";
static const char *g_szSampling  = "sampling";
static const char *g_szStatus    = "status";
static const char *g_szAll       = "all";
static const char *g_szError     = "error";
static const char *g_szReset     = "reset";
static const char *g_szMCA       = "MCA";
static const char *g_szSaveMCA   = "SaveMca";
bool g_fMca = false;
//-----------------------------------------------------------------------------
TRedPitayaSetup g_rp_setup;
//-----------------------------------------------------------------------------

Json::Value HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup);
Json::Value HandleReadData(Json::Value &jRead, TRedPitayaSetup &rp_setup); 
Json::Value HandleSampling(Json::Value &jSampling, TRedPitayaSetup &rp_setup, bool &fRun);
Json::Value HandleMCA(Json::Value &jMCA, TRedPitayaSetup &rp_setup);
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
void SafeGetMcaSpectrum (TFloatVec &vSpectrum);
bool CountBraces (std::string &strJson);
int CountInString (const std::string &strJson, int c);
Json::Value ReadSignal (double dLen);
bool GetNextPulse (TFloatVec &vPulse);
bool GetPulseParams (TFloatVec vRawPulse, TPulseInfo &pi);
TFloatVec SmoothPulse (const TFloatVec &vRawPulse);
float VectorAverage (const TFloatVec &vec);

//-----------------------------------------------------------------------------
string SaveMCA ();
//-----------------------------------------------------------------------------
int main (void)
{
    bool fRun=true;
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
	Json::Value root, jReply;
	Json::Reader reader;
    //TRedPitayaSetup rp_setup;
    string strReply;
    Timer t;

    Json::Value jSetup = g_rp_setup.AsJson();
    g_rp_setup.LoadFromJson("rp_setup.json");
    g_mca_calculator.SetParams (g_rp_setup.GetMcaParams());
    printf ("Setup: %s\n", StringifyJson(jSetup).c_str());
    t.setInterval (AddPulse, 1000);
    while (fRun) {
        char buffer [1024];
        zmq_recv (responder, buffer, 1024, 0);
        std::string strJson = ToLower(buffer);
        printf ("Received Message:\n%s\n", strJson.c_str());
        strJson = ReplaceAll(strJson, "\'", "\"");

        if (reader.parse (strJson, root)) {
        	printf ("Message parsed\n");
			strReply  = "";
            if (!root["setup"].isNull())
                jReply["setup"] = HandleSetup(root["setup"], g_rp_setup);
            if (!root[g_szReadData].isNull())
                jReply["pulses"] = HandleReadData(root[g_szReadData], g_rp_setup);
            if (!root[g_szSampling].isNull())
                jReply[g_szSampling] = HandleSampling(root[g_szSampling], g_rp_setup, fRun);
            if (!root[g_szMCA].isNull())
                jReply[g_szMCA] = HandleMCA(root[g_szMCA], g_rp_setup);
        }
        else {
            fprintf (stderr, "Parsing error\n");
			strReply += strJson + "\nParsing Error";
		}
        strReply = StringifyJson (jReply);
        if (strReply.length() == 0)
            strReply += std::string("{}");
        zmq_send (responder, strReply.c_str(), strReply.length(), 0);
        if (strReply.length() < 50)
		    fprintf (stderr, "Reply:\n%s\n", strReply.c_str());
        else
            fprintf (stderr, "Reply length %d\n", (int) strReply.length());
    }
    return 0;
}

//-----------------------------------------------------------------------------
int CountInString (const std::string &strJson, int c)
{
    int iStart=0, nCount=0;

    iStart = strJson.find (c, iStart);
    while (iStart != std::string::npos) {
        nCount++;
        iStart = strJson.find (c, iStart + 1);
    }
    return (nCount);
}

//-----------------------------------------------------------------------------
bool CountBraces (std::string &strJson)
{
    int nOpen = CountInString (strJson, '{');
    int nClose = CountInString (strJson, '}');
    return ((nOpen > 0) && (nOpen == nClose));
}

//-----------------------------------------------------------------------------

Json::Value HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup)
{
    std::string strReply, strCommand;
    Json::Value jRead, jNew;

    try {
        if (jSetup.isString())
            strCommand = ToLower(jSetup.asString());
        if (strCommand == "read") {
            jNew = rp_setup.AsJson();
        }
        else {
            jNew = rp_setup.UpdateFromJson(jSetup);
            rp_setup.SaveToJson("rp_setup.json");
		}
		if (strReply.length() == 0)
	        strReply = StringifyJson (rp_setup.AsJson());
    }
    catch (std::exception &err) {
        jNew["error"] = err.what();
    }
    return (jNew);
}
//-----------------------------------------------------------------------------

Json::Value HandleReadData(Json::Value &jRead, TRedPitayaSetup &rp_setup)
{
    TFloatVec vPulse;
    TFloatVec::iterator i;
	mutex mtx;
    std::string strReply, strNumber, strPulses, strPulse;
    char szNum[128];
    Json::Value jAllPulses, jPulse(Json::arrayValue), jPulseData(Json::arrayValue);
    int n, nPulses, j;
    
    try {
        Json::Value jMca = jRead["mca"];
		if (jRead["signal"].isNull() == false) {
			string strSignalLength = jRead["signal"].asString();
			double dLen = stod (strSignalLength);
            if (dLen > 0) {
            	jAllPulses["signal"] = ReadSignal (dLen);
            	strReply = StringifyJson(jAllPulses);
            	ExportDebugPulse(g_qDebug);
            }
		}
    }
    catch (std::exception &exp) {
        strReply = std::string("Runtime error in '': ") + std::string (exp.what());
    }
    return (jAllPulses);
}

//-----------------------------------------------------------------------------
Json::Value ReadSignal (double dLen)
{
    Json::Value jSignal(Json::arrayValue);
	mutex mtx;
	TFloatVec vPulse;
	TFloatVec::iterator i;
	char szNum[128];
	std::string strNumber;

    try {
		int nBuffer = int ((dLen / 8e-9) + 0.5);
		fprintf (stderr, "length: i%g\nBuffer: %d\n", dLen, nBuffer);
        	//for (n=0 ; (n < nPulses) && (SafeQueueSize () > 0) ; n++) {
		while ((jSignal.size() < nBuffer) && (SafeQueueSize() > 0)) {
        	//for (n=0 ; (n < nPulses) && (SafeQueueSize () > 0) ; n++) {
        	mtx.lock ();
        	vPulse = g_qPulses.back();
        	g_qPulses.pop();
        	mtx.unlock ();
            	//for (i=vPulse.begin(), j=0 ; (i != vPulse.end()) && (j < nBuffer) ; i++, j++) {
            for (i=vPulse.begin() ; (i != vPulse.end()) && (jSignal.size() < nBuffer) ; i++) {
                sprintf (szNum, "%.3f", *i);
                strNumber = std::string (szNum);
                jSignal.append(strNumber.c_str());
            }
        }
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error in 'ReadSignal':\n%s\n", exp.what());
    }
    return (jSignal);
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
    return (g_rp_setup.GetSamplingOnOff ());
}
//-----------------------------------------------------------------------------

void SafeSetMca (bool fOnOff)
{
    g_rp_setup.SetSamplingOnOff (fOnOff);
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

bool str_to_bool (const std::string &sSource)
{
	bool f  = false;
	std::string str = ToLower(sSource);

	if (str == "false")
		f = false;
	else if (str == "true")
		f = true;
	return (f);
}
//-----------------------------------------------------------------------------

Json::Value HandleSampling(Json::Value &jSampling, TRedPitayaSetup &rp_setup, bool &fRun)
{
    std::string strResult;
    Json::Value jResult, jStatus;
    bool fCommandOK;

    try {
        fCommandOK = true;
		strResult = StringifyJson(jSampling);
		if (!jSampling["signal"].isNull()) {
			rp_setup.SetSamplingOnOff (str_to_bool (ToLower (jSampling["signal"].asString())));
		}
		if (!jSampling["mca"].isNull()) {
			rp_setup.SetMcaOnOff (str_to_bool (ToLower (jSampling["mca"].asString())));
		}
		if (!jSampling["psd"].isNull()) {
			rp_setup.SetPsdOnOff (str_to_bool (ToLower (jSampling["psd"].asString())));
		}
		if (!jSampling["op"].isNull()) {
			std:string strOp = ToLower(jSampling["op"].asString());
			if (strOp == "quit")
				fRun = false;
		}
		jStatus["signal"] = rp_setup.GetSamplingOnOff ();
		jStatus["mca"] = rp_setup.GetMcaOnOff ();
		jStatus["psd"] = rp_setup.GetPsdOnOff ();
		jResult["status"] = jStatus;
/*
        string strCmd = ToLower (jSampling.asString());
        if (strCmd == "true")
            SafeStartStop (true);
        else if (strCmd == "false")
            SafeStartStop (false);
        else if (strCmd == "status")
            fCommandOK = true;
        else if (strCmd == "quit") {
            fCommandOK = true;
            fRun = false;
        }
        else
            fCommandOK = false;
        if (fCommandOK) {
            jResult[g_szSampling] = (SafeGetStatus () ? true : false);
			jResult["pulse_count"] = to_string(g_qDebug.size());
		}
            //jResult["sampling"] = (SafeGetStatus () ? "Running" : "stopped");
        else
            jResult[g_szSampling] = "Command Unknown";
*/
    }
    catch (std::exception &exp) {
		fprintf (stderr, "Runtime error in 'HandleSampling':\n%s\n", exp.what());
        jResult[g_szSampling] = exp.what();
    }
    return (jResult);
    //strResult = StringifyJson(jResult);
    //return (strResult);
}
//-----------------------------------------------------------------------------

Json::Value HandleMCA(Json::Value &jMCA, TRedPitayaSetup &rp_setup)
//std::string HandleMCA(Json::Value &jMCA, TRedPitayaSetup &rp_setup)
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
			jResult["pulse_count"] = to_string(g_qDebug.size());
        }
        else if (str == g_szReadMca)
            SafeReadMca (jResult);
        else if (str == g_szSaveMCA)
            jResult = SaveMCA ();
        else
            jResult[g_szMCA] = "Command Unknown";
    }
    catch (std::exception &exp) {
        jResult[g_szError] = exp.what();
    }
    return (jResult);
    //return (StringifyJson(jResult));
}
//-----------------------------------------------------------------------------

void SafeGetMcaSpectrum (TFloatVec &vSpectrum)
{
    mutex mtx;

    mtx.lock();
    g_mca_calculator.GetSpectrum (vSpectrum);
    mtx.unlock();
}
//-----------------------------------------------------------------------------

void SafeReadMca (Json::Value &jResult)
{
//    mutex mtx;
//    TFloatVec vSpectrum;
    TFloatVec::iterator i;
    TFloatVec vSpectrum;

//    mtx.lock();
//    g_mca_calculator.GetSpectrum (vSpectrum);
//    mtx.unlock();
    SafeGetMcaSpectrum (vSpectrum);
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
    TPulseInfo pi;

    if ((SafeGetStatus ())) {
        if (GetNextPulse (vPulse)) {
            if (GetPulseParams (vPulse, pi))
                g_vPulsesInfo.push_back (pi);
        }
/*
        if (ReadVectorFromFile ("pulse.csv", vPulse)) {
            float f, fMax = VectorMax (vPulse) * 0.1, r, rMax, noise;

            for (i=vPulse.begin() ; i != vPulse.end() ; i++) {
                f = *i;
                r = (float) rand();
                rMax = (float) RAND_MAX;
                noise = (r / rMax) * fMax;
                //f = *i;
                //f += (rand() / RAND_MAX) * fMax;
                f += noise;
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
*/
    }
}

//-----------------------------------------------------------------------------
bool GetPulseParams (TFloatVec vRawPulse, TPulseInfo &pi)
{
    bool fExtractParams=false;
    TFloatVec vPulse;

    try {
        pi.Clear ();
        pi.SetRawPulse (vRawPulse);
        pi.SetPulse (SmoothPulse (vRawPulse));
        fExtractParams = true;
    }
    catch (std::exception &err) {
        fprintf (stderr, "Runtime error in 'GetPulseParams':\n%s\n", err.what());
    }
    return (fExtractParams);
}

//-----------------------------------------------------------------------------
float VectorAverage (const TFloatVec &vec)
{
    TFloatVec::const_iterator i;
    double dAverage=0;

	if (vec.size() > 0) {
    	for (i=vec.begin() ; i != vec.end() ; i++)
       		dAverage += (double) *i;
    	dAverage /= (double) vec.size();
	}
    return (dAverage);
}

//-----------------------------------------------------------------------------
TFloatVec SmoothPulse (const TFloatVec &vRawPulse)
{
    TFloatVec vSmooth, vWin;

    if (vRawPulse.size() > 0) {
        TFloatVec::const_iterator iRaw;
        TFloatVec::iterator iSmooth, iWin;
        size_t n;
        vSmooth.resize(vRawPulse.size());
        int nSmoothWindow=7;
        for (iRaw=vRawPulse.begin(), iSmooth=vSmooth.begin() ; iRaw != vRawPulse.end() ; iRaw++, iSmooth++) {
			for (n=0 ; n < (vWin.size() - 1) ; n++)
				vWin[n] = vWin[n + 1];
            if (vWin.size() >= nSmoothWindow)
				vWin[nSmoothWindow - 1] = *iRaw;
			else
            	vWin.push_back (*iRaw);
            float f = VectorAverage (vWin);
            *iSmooth = f;//VectorAverage (vWin);
        }
    }
    return (vSmooth);
}

//-----------------------------------------------------------------------------

bool GetNextPulse (TFloatVec &vPulse)
{
    bool fPulse = false;

    fPulse = ReadVectorFromFile ("pulse.csv", vPulse);
    return (fPulse);
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
//-----------------------------------------------------------------------------

string SaveMCA ()
{
    string strResult;
    TFloatVec vSpectrum;
    TFloatVec::iterator i;

    try {

        strResult = "mca.csv";
        SafeGetMcaSpectrum (vSpectrum);
        FILE *file = fopen (strResult.c_str(), "w+");
        if (file != NULL) {
            for (i=vSpectrum.begin() ; i != vSpectrum.end() ; i++)
                fprintf (file, "%g\n", *i);
        }
        fclose (file);
    }
    catch (std::exception exp) {
        strResult = exp.what();
    }
    return (strResult);

}
