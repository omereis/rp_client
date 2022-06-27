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
TFloatVec g_vRawSignal;
//TFloatQueue g_qRawSignal;

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
void OnTimerTick ();
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
bool GetPulseParams (TFloatVec vBuffer, TPulseInfoVec &piVec);
TFloatVec SmoothPulse (const TFloatVec &vRawPulse);
float VectorAverage (const TFloatVec &vec);
Json::Value ReadMca ();

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
    t.setInterval (OnTimerTick, 1000);
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
        //if (strReply.length() < 50)
		    //fprintf (stderr, "Reply:\n%s\n", strReply.c_str());
        //else
            //fprintf (stderr, "Reply length %d\n", (int) strReply.length());
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
		std::string strSetup = StringifyJson (jSetup);
        strCommand = ToLower(jSetup["command"].asString());
		if (strCommand == "update") {
			//if (!jSetup["trigger"].isNull()) {
            	jNew = rp_setup.UpdateFromJson(jSetup);
            	rp_setup.SaveToJson("rp_setup.json");
			//}
		}
        jNew = rp_setup.AsJson();
		strReply = StringifyJson (jNew);
		//fprintf (stderr, "\nSetup reply JSON:\n%s\n\n", strReply.c_str());
    }
    catch (std::exception &err) {
        jNew = rp_setup.AsJson();
        jNew["error"] = err.what();
    }
	strReply = StringifyJson (jNew);
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
        //Json::Value jMca = jRead["mca"];
		if (jRead["signal"].isNull() == false) {
			string strSignalLength = jRead["signal"].asString();
			double dLen = stod (strSignalLength);
            if (dLen > 0) {
            	jAllPulses["signal"] = ReadSignal (dLen);
            	strReply = StringifyJson(jAllPulses);
            	ExportDebugPulse(g_qDebug);
            }
		}
        if (!jRead["mca"].isNull()) {
            jAllPulses["mca"] = ReadMca ();
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
    Json::Value jSignal;
	int nBuffer;
	TFloatVec vPulse;
	std::string strNumber;

    try {
		nBuffer = int ((dLen / 8e-9) + 0.5);
		fprintf (stderr, "length: i%g\nBuffer: %d\n", dLen, nBuffer);
        while ((jSignal.size() < nBuffer) && (g_vRawSignal.size() > 0)) {
			strNumber = to_string (*(g_vRawSignal.begin()));
            jSignal.append(strNumber.c_str());
			g_vRawSignal.erase (g_vRawSignal.begin());
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
    //s = g_qPulses.size();
	s = g_vPulsesInfo.size();
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
		jResult["buffer"] = to_string(g_vRawSignal.size());
    }
    catch (std::exception &exp) {
		fprintf (stderr, "Runtime error in 'HandleSampling':\n%s\n", exp.what());
        jResult[g_szSampling] = exp.what();
    }
    return (jResult);
}
//-----------------------------------------------------------------------------

Json::Value HandleMCA(Json::Value &jMCA, TRedPitayaSetup &rp_setup)
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
    TFloatVec::iterator i;
    TFloatVec vSpectrum;

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

void OnTimerTick ()
{
    TFloatVec vPulse;
    TFloatVec::iterator i;
	//mutex mtx;
    //float fDelta;
    TPulseInfoVec piVec;

	//g_vRawSignal.clear();
	//g_vRawSignal.push(back(17);
    if (g_rp_setup.GetSamplingOnOff ()) {
        if (GetNextPulse (vPulse)) {
            if (GetPulseParams (vPulse, piVec)) {
                g_vPulsesInfo.insert (g_vPulsesInfo.end(), piVec.begin(), piVec.end());
            	if (g_rp_setup.GetMcaOnOff())
                	g_mca_calculator.NewPulse (piVec);
                g_vRawSignal.insert (g_vRawSignal.end(), vPulse.begin(), vPulse.end());
                while (g_vRawSignal.size() > 10000)
                    g_vRawSignal.erase (g_vRawSignal.begin());
            }
        }
    }
}

//-----------------------------------------------------------------------------
bool GetPulseParams (TFloatVec vBuffer, TPulseInfoVec &piVec)
{
    bool fExtractParams=false;
    TPulseInfo piNew;
    TFloatVec vRawPulse, vPulse;
    bool fInPulse;
    int i, iStart;
    float fMax, fArea;

    try {
		FILE *file = fopen ("buf.csv", "w+");
		TFloatVec::iterator it;
		for (it=vBuffer.begin() ; it != vBuffer.end() ; it++)
			fprintf (file, "%g\n", *it);
		fclose (file);

        piVec.clear();
        TFloatVec::const_iterator i=vBuffer.begin();
        for (fInPulse=false ; i != vBuffer.end() ; i++) {
            if (!fInPulse) {
                if (*i > g_rp_setup.GetTriggerLevel ()) { // pulse start
                    fMax = fArea = *i;
                    fInPulse = true;
                    vPulse.push_back (*i);
                }
            }
            else { // in pulse
                fMax = max (fMax, *i);
                fArea += *i;
                vPulse.push_back (*i);
                if (*i < g_rp_setup.GetTriggerLevel ()) {
                    piNew.SetRawPulse (vRawPulse);
                    piNew.SetPulse (SmoothPulse (vRawPulse));
                    piNew.SetMaxVal (fMax);
                    piNew.SetLength (fArea);
                    piVec.push_back (piNew);
                    fInPulse = false;
                }
            }
        }
        fExtractParams = true;
    }
    catch (std::exception &err) {
        fprintf (stderr, "Runtime error in 'GetPulseParams':\n%s\n", err.what());
    }
    return (fExtractParams);
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
			for (n=0 ; (vWin.size() > 0) && (n < (vWin.size() - 1)) ; n++)
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
//-----------------------------------------------------------------------------

Json::Value ReadMca ()
{
    Json::Value jMCA(Json::arrayValue);
    TFloatVec vSpectrum;
    TFloatVec::iterator i;
    char *sz = new char[32];

    g_mca_calculator.GetSpectrum (vSpectrum);
    for (i=vSpectrum.begin() ; i <= vSpectrum.end() ; i++) {
        sprintf (sz, "%.3f", *i);
        jMCA.append (sz);
    }
    return (jMCA);
}
