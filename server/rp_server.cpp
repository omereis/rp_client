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
#include "rp_server.h"

#include "jsoncpp/json/json.h"
//-----------------------------------------------------------------------------
TFloatVecQueue g_qPulses;
TFloatVecQueue g_qDebug;
TFloatVec g_vRawSignal;

TPulseInfoVec g_vPulsesInfo;
bool g_fRunning = false;
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
static const char *g_szDefaultSetup = "rp_setup.json";
bool g_fMca = false;
//-----------------------------------------------------------------------------
//#ifdef	__HARDWARE
TRedPitayaSetup g_rp_setup;
//TRedPitayaSampling rps;
//TRedPitayaTrigger rpt;
//#endif
//-----------------------------------------------------------------------------

Json::Value HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup, TCalcMca &mca_calculator);
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
//bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TFloatVec &vPulse);

//-----------------------------------------------------------------------------
std::string ret_str()
{
	std::string s = "this is a string";
	fprintf (stderr, "ret_str, s-'%s'\n", s.c_str());
	return (s);
}
//-----------------------------------------------------------------------------
string SaveMCA ();
//-----------------------------------------------------------------------------
int main1 ()
{
#ifdef  _RED_PITAYA_HW
	rp_Init();
//#endif
				PrintTriggerSource ("rp_server.cpp:139");
	fprintf (stderr, "running\n");
	std::string str = ret_str();
	fprintf (stderr, "main, str: '%s'\n", str.c_str());
	std::string strSrc = GetHardwareTriggerName (RP_TRIG_SRC_DISABLED);
	fprintf (stderr, "main, strSrc: '%s'\n", strSrc.c_str());
//#ifdef  _RED_PITAYA_HW
	rp_Release();
#endif
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main ()
//int main (int argc, char *argv[])
{
#ifdef  _RED_PITAYA_HW
	rp_Init();
#endif
    bool fRun=true, fUpdateHardware=false;
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
	Json::Value root, jReply;
	Json::Reader reader;
    string strReply;
    Timer t;

	fprintf (stderr, "Loading Setup\n");
#ifdef	_RED_PITAYA_HW
	fUpdateHardware = true;
	//fprintf (stderr, "Loading Hardware Setup\n");
	//g_rp_setup.LoadFromHardware (true);
#else
#endif
    g_rp_setup.LoadFromJson(g_szDefaultSetup);// "rp_setup.json");
    Json::Value jSetup = g_rp_setup.AsJson();
	g_rp_setup.UpdateFromJson(jSetup, fUpdateHardware);
    g_mca_calculator.SetParams (g_rp_setup.GetMcaParams());
    printf ("Setup: %s\n", StringifyJson(jSetup).c_str());
    t.setInterval (OnTimerTick, 100);
    while (fRun) {
        char buffer [1024];
        zmq_recv (responder, buffer, 1024, 0);
        std::string strJson = ToLower(buffer);
        printf ("Received Message:\n%s\n", strJson.c_str());
        strJson = ReplaceAll(strJson, "\'", "\"");

        if (reader.parse (strJson, root)) {
        	printf ("Message parsed\n");
			strReply  = "";
            if (!root["setup"].isNull()) {
				//PrintTriggerSource ("rp_server.cpp:139");
                jReply["setup"] = HandleSetup(root["setup"], g_rp_setup, g_mca_calculator);
			}
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
Json::Value HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup, TCalcMca &mca_calculator)
{
    std::string strReply, strCommand;
    Json::Value jRead, jNew;

    try {
		//fprintf (stderr, "\n----------------------------------------------------------\n");
		bool fUpdateHardware = false;
#ifdef	_RED_PITAYA_HW
		fUpdateHardware = true;
#endif
		std::string strSetup = StringifyJson (jSetup);
		fprintf (stderr, "Setup Message: %s\n", strSetup.c_str());
        strCommand = ToLower(jSetup["command"].asString());
		fprintf (stderr, "Command: %s\n", strCommand.c_str());
		Json::Value jBkgnd = jSetup["background"];
		if (strCommand == "update") {
#ifdef	_RED_PITAYA_HW
    		rp_AcqReset();
#endif
            jNew = rp_setup.UpdateFromJson(jSetup, fUpdateHardware);
		}
		else if (strCommand == "trigger_now") {
			rp_setup.TriggerNow ();
			jNew = rp_setup.TriggerAsJson();
		}
		else if (!jBkgnd .isNull()) {
			fprintf (stderr, "\n\nBackground Command\n");
			jNew = rp_setup.HandleBackground (jBkgnd);
			if (jBkgnd.isString()) {
				string sBkgnd = jSetup["background"].asString();
				fprintf (stderr, "    Command content:%s\n", sBkgnd.c_str());
			} 
			else {
				string s = StringifyJson (jBkgnd);
				fprintf (stderr, "    Command content:%s\n", s.c_str());
			}
		}
/*
#ifdef	_RED_PITAYA_HW
		fprintf (stderr, "\nLoading from hardware\n");
		rp_setup.LoadFromHardware (true);
#endif
*/
		if (strCommand == "read_trigger") {
			fprintf (stderr, "Writing trigger\n");
			jNew = rp_setup.TriggerAsJson();
		}
		else if (strCommand == "update_trigger")
            jNew = rp_setup.UpdateFromJson(jSetup, fUpdateHardware);
		else
        	jNew = rp_setup.AsJson();
		strReply = StringifyJson (jNew);
#ifdef  _RED_PITAYA_HW
#endif
		rp_setup.SaveToJson (g_szDefaultSetup);
		fprintf (stderr, "Setup reply JSON:\n%s\n\n", strReply.c_str());
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
		if (jRead["signal"].isNull() == false) {
			string strSignalLength = jRead["signal"].asString();
			fprintf (stderr, "Required length: %s\n", strSignalLength.c_str());
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
			//fprintf (stderr, "HandleSampling, 385\n");
			rp_setup.SetSamplingOnOff (str_to_bool (ToLower (jSampling["signal"].asString())));
			//fprintf (stderr, "HandleSampling, 387\n");
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
		//fprintf (stderr, "HandleSampling, 405\n");
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
    TPulseInfoVec piVec;

    if (g_rp_setup.GetSamplingOnOff ()) {
		//fprintf (stderr, "OnTimerTick, 501\n");
        if (GetNextPulse (vPulse)) {
			//fprintf (stderr, "OnTimerTick, 503\n");
			//fprintf (stderr, "Pulse read: %d\n", vPulse.size());
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
    int n, iStart;
    float fMax, fArea;
	double dBackground;
	TFloatVec::const_iterator it;

    try {
/*
		FILE *file = fopen ("buf.csv", "w+");
		for (it=vBuffer.begin() ; it != vBuffer.end() ; it++)
			fprintf (file, "%g\n", *it);
		fclose (file);
*/

        piVec.clear();
		n = 0;
        TFloatVec::const_iterator i=vBuffer.begin();
		dBackground = g_rp_setup.GetBackground (); 
        for (fInPulse=false, it=vBuffer.begin(); it != vBuffer.end() ; it++) {
			if (it == vBuffer.begin())
				fMax = *it;
			n++;
			//fprintf (stderr, "Item %d: %g\n", n, *it);
            if (!fInPulse) {
                if (*it > dBackground) { // pulse start
                //if (*it > g_rp_setup.GetTriggerLevel ()) { // pulse start
                    //fMax = fArea = *i;
                    fInPulse = true;
                    //vRawPulse.push_back (*i);
                }
            }
            if (fInPulse) { // in pulse
                fMax = max (fMax, *it);
                fArea += *it;
                vRawPulse.push_back (*it);
                if (*it < dBackground) {
                //if (*i < g_rp_setup.GetTriggerLevel ()) {
                    piNew.SetRawPulse (vRawPulse);
                    piNew.SetPulse (SmoothPulse (vRawPulse));
                    piNew.SetMaxVal (fMax);
                    piNew.SetArea (fArea);
                    piVec.push_back (piNew);
                    fInPulse = false;
                    vRawPulse.clear();
					fMax = fArea = 0;
                }
            }
        }
		//fprintf (stderr, "Pulse Points: %d\n", n);
        fExtractParams = true;
/*
        TPulseInfoVec::iterator iPulse;
        file = fopen ("pulse_info.csv", "w+");
        for (iPulse=piVec.begin() ; iPulse != piVec.end() ; iPulse++) {
            for (it=iPulse->GetRawPulseBegin() ; it != iPulse->GetRawPulseEnd() ; it++)
                fprintf (file, "%g\n", *it);
        }
        fclose (file);
*/
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
#ifdef  _RED_PITAYA_HW
    fPulse = ReadHardwareSamples (g_rp_setup, vPulse);
	//PrintBool ("GetNextPulse:616", fPulse);
#else
    fPulse = ReadVectorFromFile ("pulse.csv", vPulse);
#endif
    return (fPulse);
}

//-----------------------------------------------------------------------------
#ifdef  _RED_PITAYA_HW
bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TFloatVec &vPulse)
{
    uint32_t buff_size = 16384;
    float *buff = (float *)malloc(buff_size * sizeof(float));
	int16_t* auiBuffer = (int16_t*) calloc (buff_size, sizeof (auiBuffer[0]));
	rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;
	bool fTrigger=false, fTimeout=false;
	clock_t cStart;
	double dDiff;

	//fprintf (stderr, "--------------------------------------\n");
    //rp_AcqReset();
	//rp_AcqSetDecimation(RP_DEC_1);
	rp_AcqSetDecimation(rp_setup.GetHardwareDecimation());
	//fprintf (stderr, "ReadHardwareSamples, 644\n");
	float fLevel = rp_setup.GetHardwareTriggerLevel();
	//fprintf (stderr, "Trigger level (#642): %g\n", fLevel);
	rp_AcqSetTriggerLevel(rp_setup.GetHardwareTriggerChannel(), rp_setup.GetHardwareTriggerLevel());
	//fprintf (stderr, "ReadHardwareSamples, 648\n");
	//rp_AcqSetTriggerLevel(RP_T_CH_1, -15e-3);
	rp_AcqSetSamplingRate (rp_setup.GetHardwareSamplingRate());
	//fprintf (stderr, "ReadHardwareSamples, 651\n");
	//rp_AcqSetSamplingRate (RP_SMP_125M);
    rp_AcqSetTriggerDelay(0);
	//fprintf (stderr, "ReadHardwareSamples, 650\n");
    rp_AcqStart();
	usleep(1);
	rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
	cStart = clock();
	while((fTrigger == false) && (fTimeout == false)){
		rp_AcqGetTriggerState(&state);
		if (state == RP_TRIG_STATE_TRIGGERED)//{
			fTrigger =true;
		else
			fprintf (stderr, "No Trigger\r");
		dDiff = (clock() - cStart);
		dDiff /= (double) CLOCKS_PER_SEC;
		if (dDiff >= 3)
			fTimeout = true;
	}
    if (fTrigger) {
	    uint32_t uiTriggerPos, uiLen=buff_size, n;
        TFloatVec::iterator i;
	    rp_AcqGetWritePointerAtTrig (&uiTriggerPos);
	    rp_AcqGetDataV (RP_CH_1, uiTriggerPos, &uiLen, buff);
	    //rp_AcqGetDataRaw (RP_CH_1, uiTriggerPos, &uiLen, auiBuffer);
        vPulse.resize (buff_size, 0);
        for (n=0, i=vPulse.begin() ; n < (int) buff_size ; n++, i++) //{
			*i = buff[n];
			//*i = auiBuffer[n];
    }
    else
    	printf ("Timeout: %g\n", dDiff);
    rp_AcqStop();
	//fprintf (stderr, "--------------------------------------\n");
    return (fTrigger);
}
/*
*/
#endif

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
