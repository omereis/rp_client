/******************************************************************************\
|                               rp_server.cpp                                  |
\******************************************************************************/

#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>

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
#include "trim.h"
#include "pulse_index.h"
#include "trim.h"

#include "PulseFilterInfo.h"
#include "jsoncpp/json/json.h"
//-----------------------------------------------------------------------------
TFloatVecQueue g_qPulses;
TFloatVecQueue g_qFiltered;
TFloatVecQueue g_qDebug;
TFloatVec g_vRawSignal;

TPulseQueue g_qFilteredPulses;
TPulseInfoVec g_vPulsesInfo;
bool g_fRunning = false;
string g_strAppDate;
//TCalcMca g_mca_calculator;
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

bool HandleMessage (const string &strJson, Json::Value &jReply);//string &strReply)
void SendReply (void *responder, char *szRecvBuffer, Json::Value jReplyMessage);
Json::Value HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup);
Json::Value HandleReadData(Json::Value &jRead, TRedPitayaSetup &rp_setup, TFloatVec &vSignal); 
Json::Value HandleSampling(Json::Value &jSampling, TRedPitayaSetup &rp_setup, bool &fRun);
Json::Value HandleMCA(Json::Value &jMCA, TRedPitayaSetup &rp_setup);
Json::Value ContinueReadSignal (Json::Value jCmd, TFloatVec &vSignal);
void OnTimerTick ();
void ExportDebugPulse(TFloatVecQueue &qDebug);
void SafeStartStop (bool fCommand);
bool SafeGetStatus ();
size_t SafeQueueSize ();
void SafeQueueClear ();
//size_t SafeQueueExtract (TDoubleVec &vPulse, TDoubleVec &vFiltered);
size_t SafeQueueExtract (TDoubleVec &vPulse, TDoubleVec &vFiltered, TDoubleVec &vKernel, TPulseIndexVec &vIndices);
//size_t SafeQueueExtract (TDoubleVec &vPulse, TDoubleVec &vFiltered, TDoubleVec &vKernel);
//size_t SafeQueueExtract (TFloatVec &vPulse, TFloatVec &vFiltered);
//void SafeQueueAdd (const TFloatVec &vPulse, TFloatVec &vFiltered);
void SafeQueueAdd (const TPulseFilter &pulse_filter);
//void SafeQueueAdd (const TDoubleVec &vPulse, TDoubleVec &vFiltered);

void SafeSetMca (bool fOnOff);
bool SafeGetMca ();
TMcaParams SafeGetMcaParams ();
void SafeSetMcaParams (const TMcaParams &params);
//void SafeAddToMca (const TFloatVec &vPulse);
void SafeResetMca();
void SafeReadMca (Json::Value &jResult);
void SafeGetMcaSpectrum (TFloatVec &vSpectrum);
bool CountBraces (std::string &strJson);
int CountInString (const std::string &strJson, int c);
//size_t ReadSignal (double dLen, TFloatVec &vSignal);
Json::Value ReadSignal (TRedPitayaSetup &rp_setup, double dLen, bool fDebug);
//Json::Value ReadSignal (TRedPitayaSetup &rp_setup, double dLen, TFloatVec &vSignal, bool fDebug);
bool GetPulseParams (const TPulseFilter &pulse_filter, TPulseInfoVec &piVec);
//bool GetPulseParams (TFloatVec &vBuffer, TPulseInfoVec &piVec);
//bool GetPulseParams (const TPulseFilter &pulse_filter, TPulseIndexVec &vIndices);
//bool GetPulseParams (TDoubleVec &vBuffer, TPulseInfoVec &piVec);
void IdentifyPulses (const TPulseFilter &pulse_filter, TPulseIndexVec &vIndices);
TFloatVec SmoothPulse (const TFloatVec &vRawPulse);
//float VectorAverage (const TFloatVec &vec);
Json::Value ReadMca ();
//bool FindPulseStart (TFloatVec::const_iterator itBegin, TFloatVec::const_iterator itEnd, TFloatVecConstIterator &itFound, double dBackground, bool func (float f1, float f2));
bool FindPulseStartEnd (TFloatVec::iterator iBufferStart, TFloatVec::iterator iBufferEnd, double dBkgnd, TFloatVec::iterator &iStart, TFloatVec::iterator &iEnd);
bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TDoubleVec &vPulse);
//bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TFloatVec &vPulse);
//void AddPulse (TFloatVec::iterator iStart, TFloatVec::iterator iEnd, double dBkgnd, TPulseInfoVec &piVec);
void AddPulse (TDoubleVec::iterator iStart, TDoubleVec::iterator iEnd, double dBkgnd, TPulseInfoVec &piVec);
string GetMcaCounts ();
size_t CountPulsesInSignal (const TDoubleVec &vSignal, TPulseIndexVec &vIndices);
size_t CountPulsesInSignal (const TFloatVec &vSignal, TPulseIndexVec &vIndices);
Json::Value GetPulsesInSignal (const TFloatVec &vSignal);
void GetAppDateTime (char *szFileName);
void FilterPulse (const TDoubleVec &vPulse,double dBackground, TPulseFilter &pulse_filter, bool fFilterOnOff);

//-----------------------------------------------------------------------------
std::string ret_str()
{
	std::string s = "this is a string";
	fprintf (stderr, "ret_str, s-'%s'\n", s.c_str());
	return (s);
}

#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
string SaveMCA ();
//-----------------------------------------------------------------------------
int main (int argc, char *argv[])
{
#ifdef  _RED_PITAYA_HW
	rp_Init();
#endif
	GetAppDateTime (argv[0]);

    bool fRun=true, fUpdateHardware=false, fMessageRecieved;
    TFloatVec vSignal;
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
	Json::Value root, jReply;
	Json::Reader reader;
    string strReply;
    Timer t;
	int n, nBufferPoints;
	float *afBuffer;
	char *szRecvBuffer = new char [1024];

	fprintf (stderr, "Loading Setup\n");
#ifdef	_RED_PITAYA_HW
	fUpdateHardware = true;
	//fprintf (stderr, "Loading Hardware Setup\n");
	//g_rp_setup.LoadFromHardware (true);
#else
#endif
    g_rp_setup.LoadFromJson(g_szDefaultSetup);// "rp_setup.json");
    Json::Value jSetup = g_rp_setup.AsJson();
	//g_rp_setup.UpdateFromJson(jSetup, fUpdateHardware);
    printf ("Setup: %s\n", StringifyJson(jSetup).c_str());
    t.setInterval (OnTimerTick, 100);
    while (fRun) {
		fMessageRecieved = false;
        zmq_recv (responder, szRecvBuffer , 1024, 0);
		std::string strJson = ToLower(szRecvBuffer);
		strJson = ReplaceAll(strJson, "\'", "\"");

		fRun = HandleMessage (strJson, jReply);
		strReply = StringifyJson (jReply);
		SendReply (responder, szRecvBuffer, jReply);
    }
	delete[] szRecvBuffer;
    return 0;
}

//-----------------------------------------------------------------------------
bool HandleMessage (const string &strJson, Json::Value &jReply)//string &strReply)
{
	bool fRun=true;
	string strReply;

	//fprintf (stderr, "HandleMessage - Starting - Json message:\n%s\n", strJson.c_str());
	try {
		Json::Value root;//, jReply;
		Json::Reader reader;
		bool fMessageRecieved;
    	TFloatVec vSignal;

		if (reader.parse (strJson, root)) {
			fprintf (stderr, "Message Parsed\n");
			fMessageRecieved = true;
			jReply.clear();
			strReply  = "";
			if (!root["setup"].isNull()) {
				string s = StringifyJson (root["setup"]);
				printf ("Setup: %s\n", s.c_str());
				jReply["setup"] = HandleSetup(root["setup"], g_rp_setup);
			}
			if (!root[g_szReadData].isNull())
				jReply["pulses"] = HandleReadData(root[g_szReadData], g_rp_setup, vSignal);
			if (!root[g_szSampling].isNull()) {
				fprintf (stderr, "Reading sampling\n");
				jReply[g_szSampling] = HandleSampling(root[g_szSampling], g_rp_setup, fRun);
			}
			if (!root[g_szMCA].isNull())
				jReply[g_szMCA] = HandleMCA(root[g_szMCA], g_rp_setup);
			if (!root["signal"].isNull())
				jReply = ContinueReadSignal (root, vSignal);
		}
		else {
			fprintf (stderr, "Parsing error\n");
			strReply += strJson + "\nParsing Error";
		}
	}
	catch (std::exception &err) {
		fprintf (stderr, "Runtime error in 'HandleMessage':\n%s\n", err.what());
	}
	//fprintf (stderr, "HandleMessage - End - fRun=%d\nstrReply=%s\n\n", (int) fRun, strReply.c_str());
	return (fRun);
}

//-----------------------------------------------------------------------------
void SendReply (void *responder, char *szRecvBuffer, Json::Value jReplyMessage)
{
	try {
		Json::Value jReply;
		int nStart, nMessages, nPackSize, nCharsToSend, nTotalToSend;
	//char *szRecvBuffer = new char[1024];

			//fprintf (stderr, "Preparing Reply\n");
		string s, strReply = StringifyJson (jReplyMessage);
		strReply = trimString (strReply);
		nMessages = 1 + (int) (strReply.length() / g_rp_setup.GetPackageSize());//, nCharsToSend, nTotalToSend;
		nTotalToSend = strReply.length();
		nPackSize = g_rp_setup.GetPackageSize();
		while (nTotalToSend > 0) {
			jReply.clear();
			s = strReply.substr (nStart, nPackSize);
			nStart += nPackSize;//100;
			jReply["text"] = s;
			jReply["flag"] = nTotalToSend < nPackSize;//100;
			s = StringifyJson (jReply);
			int rc = zmq_send (responder, s.c_str(), s.length(), 0);
			nTotalToSend -= nPackSize;//100;
			if (nTotalToSend > 0) {
				//fprintf (stderr, "SendReply - Waiting...");
        		zmq_recv (responder, szRecvBuffer , 1024, 0);
				//fprintf (stderr, "SendReply - Message accepted\n");
				//rc = zmq_recv (responder, szRecvBuffer, 1024, 0);
			//rc = zmq_recv (responder, buffer, 1024, 0);
			}
		}
	//delete[] szRecvBuffer;
	}
	catch (std::exception &err) {
		fprintf (stderr, "Runtime error in 'SendReply':\n%s\n", err.what());
	}
	//fprintf (stderr, "End of sendReply: Reply send\n");
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
		else if (strCommand ==  "read_trapez") {
			jNew["trapez"] = rp_setup.GetTrapezAsJson();
		}
		else if (!jBkgnd .isNull()) {
			fprintf (stderr, "\n\nBackground Command\n");
			//TFloatVec vSignal;
			TDoubleVec vSignal;
			GetNextPulse (vSignal);
			jNew = rp_setup.HandleBackground (jBkgnd, vSignal);
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
		else if (strCommand == "read_mca_params") {
			jNew = rp_setup.McaAsJson();
		}
		else if (strCommand == "read_trigger") {
			fprintf (stderr, "Writing trigger\n");
			jNew = rp_setup.TriggerAsJson();
		}
		else if (strCommand == "update_trigger")
            jNew = rp_setup.UpdateFromJson(jSetup, fUpdateHardware);
		else {
        	jNew = rp_setup.AsJson();
			jNew["version"] = g_strAppDate;
			//fprintf (stderr, "\n+++++++++++++++++++++++++++\n");
			//fprintf (stderr, "Reading setup\n");
			//fprintf (stderr, "+++++++++++++++++++++++++++\n\n");
		}
		strReply = StringifyJson (jNew);
#ifdef  _RED_PITAYA_HW
#endif
		rp_setup.SaveToJson (g_szDefaultSetup);
		//fprintf (stderr, "Setup reply JSON:\n%s\n\n", strReply.c_str());
    }
    catch (std::exception &err) {
        jNew = rp_setup.AsJson();
        jNew["error"] = err.what();
    }
	strReply = StringifyJson (jNew);
	//fprintf (stderr, "\n+++++++++++++++++++++++++++\n");
	//fprintf (stderr, "Setup:\n%s\n", strReply.c_str());
	//fprintf (stderr, "\n+++++++++++++++++++++++++++\n");
    return (jNew);
}

//-----------------------------------------------------------------------------
Json::Value HandleReadData(Json::Value &jRead, TRedPitayaSetup &rp_setup, TFloatVec &vSignal)
{
    TFloatVec::iterator i;
	mutex mtx;
    std::string strReply, strNumber, strPulses, strPulse;
    char szNum[128];
    Json::Value jAllPulses, jPulse(Json::arrayValue), jPulseData(Json::arrayValue);
    int n, nPulses, j;
	bool fReset = false;
	TPulseIndexVec vIndices;
    
    try {
        vSignal.clear();
		strReply = StringifyJson (jRead);
		if (jRead["buffer"].isNull() == false) {
			string str = jRead["buffer"].asString();
			fprintf (stderr, "Buffer Command: %s\n", str.c_str());
			if (str == "reset")
				fReset = true;
		}
		if (fReset) {
			SafeQueueClear ();
		}
		else {
			if (jRead["signal"].isNull() == false) {
				bool fDebug = !jRead["debug"].isNull();
				string strSignalLength = jRead["signal"].asString();
				fprintf (stderr, "Required length: %s\n", strSignalLength.c_str());
				double dLen = stod (strSignalLength);
            	if (dLen > 0) {
            		//jAllPulses["signal"] = ReadSignal (rp_setup, dLen, vSignal, fDebug);
					jAllPulses["signal"] = ReadSignal (rp_setup, dLen, fDebug);
            	}
			}
        		if (!jRead["mca"].isNull()) {
					Json::Value jMca = jRead["mca"];
					if (jRead["mca"].asBool()) {
						fprintf (stderr, "\nMCA read\n");
            			jAllPulses["mca"] = ReadMca ();
				}
        	}
		}
        jAllPulses["buffer_length"] = to_string (SafeQueueSize ());
        jAllPulses["mca_length"] = GetMcaCounts ();
        jAllPulses["background"] = g_rp_setup.GetBackground();

    }
    catch (std::exception &exp) {
        strReply = std::string("Runtime error in '': ") + std::string (exp.what());
    }
    strReply = StringifyJson(jAllPulses);
    return (jAllPulses);
}

//-----------------------------------------------------------------------------
string GetMcaCounts ()
{
	TFloatVec vSpectrum;
	TFloatVec::iterator i;
	long lnSum=0;
	char sz[100];

	g_rp_setup.GetMcaSpectrum (vSpectrum);
	for (i=vSpectrum.begin() ; i != vSpectrum.end() ; i++)
		lnSum += *i;
	sprintf (sz, "Pulses: %d, spectrum sum: %ld", g_rp_setup.GetMcaPulses(), lnSum);
	return (string (sz));
}

//-----------------------------------------------------------------------------
//size_t ReadSignal (double dLen, TFloatVec &vSignal)
//Json::Value ReadSignal (TRedPitayaSetup &rp_setup, double dLen, TFloatVec &vSignal, bool fDebug)
Json::Value ReadSignal (TRedPitayaSetup &rp_setup, double dLen, bool fDebug)
{
    Json::Value jSignal, jSignalData, jFiltered, jKernel, jFiltDeriv;
	int nVectorPoints;
	TDoubleVec vPulse, vFiltered, vKernel;
	TDoubleVec::iterator it, itPrev;
	//TFloatVec vPulse, vFiltered;
	//TFloatVec::iterator it;
	std::string strNumber;
	size_t sPulse, n;
	static int nCount=0;
	TPulseIndexVec vIndices;

    try {
		nVectorPoints = int ((dLen / 8e-9) + 0.5);
		//vSignal.clear();
		fprintf (stderr, "length: i%g\nBuffer: %dx\n", dLen, nVectorPoints);
        while ((jSignal.size() < nVectorPoints) && (SafeQueueSize () > 0)) {
			sPulse = SafeQueueExtract (vPulse, vFiltered, vKernel, vIndices);
			//PrintVector (vPulse, "p_debug.csv");
			//PrintVector (vFiltered, "f_debug.csv");
			if (sPulse > 0) {
				for (it=vPulse.begin() ; (it != vPulse.end()) && (jSignal.size() < nVectorPoints) ; it++)
					jSignal.append(*it);
				if (vFiltered.size() > 0)
					for (it=vFiltered.begin(), n=0 ; (it != vFiltered.end() && (jFiltered.size() < nVectorPoints)) ; n++) {
						jFiltered.append(*it);
						itPrev = it;
						it++;
						if (n < vFiltered.size() - 1)
							jFiltDeriv.append (*it - *itPrev);
						else
							jFiltDeriv.append (0);
					}
				for (it=vKernel.begin() ; (it != vKernel.end() && (jKernel.size() < nVectorPoints)) ; it++)
					jKernel.append(*it);
			}
			//rValue += 0;
        }
        //jSignal["signal_length"] = to_string (vSignal.size());
        //jSignal["package_size"] = to_string(rp_setup.GetPackageSize());
		//PrintVector (vSignal, "out_debug.csv");
		string sSignal = StringifyJson (jSignal);
		fprintf (stderr, "Signal Read: %d\n", nCount++);
		jSignalData["data"] = jSignal;
		if (jFiltered.size() > 0)
			jSignalData["filtered"] = jFiltered;
		if (jFiltDeriv.size() > 0)
			jSignalData["filt_deriv"] = jFiltDeriv;
		if (jKernel.size() > 0)
			jSignalData["kernel"] = jKernel;
		if (fDebug) {
			if (vIndices.size() > 0) {
				Json::Value jIndices;
				TPulseIndexVec::iterator iIndex;
				for (iIndex=vIndices.begin() ; iIndex != vIndices.end() ; iIndex++) {
					jIndices.append (iIndex->AsJson());
				}
				jSignalData["detector_pulse"] = jIndices;//vIndices;//GetPulsesInSignal (vSignal);
			}
		}
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error in 'ReadSignal':\n%s\n", exp.what());
    }
	//printf ("rp_server.cpp:315, signal length:%d\n", jSignal.size());
    //return (vSignal.size());
	strNumber = StringifyJson (jSignalData);
/*
	FILE *f = fopen ("reply.txt", "w+");
	fprintf (f, "----------------------------------------\n");
	fprintf (stderr, "%s\n", strNumber.c_str());
	fprintf (f, "%s\n", strNumber.c_str());
	fprintf (f, "----------------------------------------\n");
	fflush(f);
	fclose (f);
*/
    return (jSignalData);
}


//-----------------------------------------------------------------------------
/*
Pseudocode for negative pulses
in_pulse := false
for n := 1 to signal.length
	if signal[n] > background
		above++
		below := 0
	else if signal[n] < background
		above := 0
		below++
	if below >= 3 and not in_pulse // pulse start
		in_pulse := true
	else if above >= 3 and in_pulse // end of pulse
		in_pulse = false
*/
size_t CountPulsesInSignal (const TDoubleVec &vSignal, TPulseIndexVec &vIndices)
{
	TDoubleVec::const_iterator i;
	bool fInPulse;
	double dBackground = g_rp_setup.GetBackground();
	TPulseIndex pi;
	int n, nBelow, nAbove;

	vIndices.clear();
	fInPulse = false;
	for (i=vSignal.begin(), n=0 ; i != vSignal.end() ; i++, n++) {
		if (*i < dBackground) { // below background
			nBelow++;
			nAbove = 0;
		}
		else {
			nBelow = 0;
			nAbove++;
		}
		if (nBelow >= 3) {
			if (fInPulse == false) { // pulse start
				fInPulse = true;
				pi.SetStart (n);
			}
		}
		else if (nAbove >= 3) {
			if (fInPulse) { // pulse end
				fInPulse = false;
				pi.SetSteps (n - pi.GetStart() - 3);
				vIndices.push_back (pi);
				pi.Clear ();
			}
		}
	}
	return (vIndices.size());
}

//-----------------------------------------------------------------------------
/*
Pseudocode for negative pulses
in_pulse := false
for n := 1 to signal.length
	if signal[n] > background
		above++
		below := 0
	else if signal[n] < background
		above := 0
		below++
	if below >= 3 and not in_pulse // pulse start
		in_pulse := true
	else if above >= 3 and in_pulse // end of pulse
		in_pulse = false
*/
size_t CountPulsesInSignal (const TFloatVec &vSignal, TPulseIndexVec &vIndices)
{
	TFloatVec::const_iterator i;
	bool fInPulse;
	double dBackground = g_rp_setup.GetBackground();
	TPulseIndex pi;
	int n, nBelow, nAbove;

	vIndices.clear();
	fInPulse = false;
	for (i=vSignal.begin(), n=0 ; i != vSignal.end() ; i++, n++) {
		if (*i < dBackground) { // below background
			nBelow++;
			nAbove = 0;
		}
		else {
			nBelow = 0;
			nAbove++;
		}
		if (nBelow >= 3) {
			if (fInPulse == false) { // pulse start
				fInPulse = true;
				pi.SetStart (n);
			}
		}
		else if (nAbove >= 3) {
			if (fInPulse) { // pulse end
				fInPulse = false;
				pi.SetSteps (n - pi.GetStart() - 3);
				vIndices.push_back (pi);
				pi.Clear ();
			}
		}
	}
	return (vIndices.size());
}

//-----------------------------------------------------------------------------
Json::Value GetPulsesInSignal (const TFloatVec &vSignal)
{
	TPulseIndexVec vIndices;

	CountPulsesInSignal (vSignal, vIndices);
	return (TPulseIndex::ToJson (vIndices));
}

//-----------------------------------------------------------------------------
Json::Value GetPulsesInSignal1 (const TFloatVec &vSignal)
{
	TPulseIndexVec vIndices;
	TPulseIndexVec::iterator j;
	TPulseIndex pi;
	TFloatVec::const_iterator i, iPrev;
	int n, nCons, iStart;
	Json::Value jPulses, js;

	pi.Clear();
	i = iPrev = vSignal.begin();
	n = 0;
	iStart = -1;
	for (i=vSignal.begin(), n=0 ; i != vSignal.end() ; i++, n++) {
		if (*i < *iPrev) {
			if (iStart < 0) { // begining of a decline
				iStart = n;
				nCons = 0;
			}
			else
				nCons++;
		}
		else {
			if (iStart > 0) { // end of the decline
				if (nCons >= 5) {
					pi.SetStart (iStart);
					pi.SetSteps (nCons);
					vIndices.push_back (pi);
					pi.Clear ();
				}
			}
			iStart = -1;
			nCons = 0;
		}
		iPrev = i;
	}
	for (j=vIndices.begin() ; j != vIndices.end() ; j++) {
		printf ("Pulse start: %d, length: %d\n", j->GetStart(), j->GetSteps());
		js["start"] = j->GetStart();
		js["length"] = j->GetSteps();
		jPulses.append(js);
		js.clear();
	}
	printf ("%d pulses found\n", (int) vIndices.size());
	//getchar();
	return (jPulses);
}

//-----------------------------------------------------------------------------
size_t SafeQueueExtract (TDoubleVec &vPulse, TDoubleVec &vFiltered, TDoubleVec &vKernel, TPulseIndexVec &vIndices)
//size_t SafeQueueExtract (TDoubleVec &vPulse, TDoubleVec &vFiltered, TDoubleVec &vKernel)
//size_t SafeQueueExtract (TFloatVec &vPulse, TFloatVec &vFiltered)
{
	TPulseFilter pulse_filter;
    mutex mtx;
    size_t s;

    vPulse.clear();
    mtx.lock ();
	pulse_filter = g_qFilteredPulses.back();
	g_qFilteredPulses.pop();
	pulse_filter.GetPulse (vPulse);
	pulse_filter.GetFiltered (vFiltered);
	pulse_filter.GetKernel(vKernel);
	pulse_filter.GetPulsesIndices (vIndices);
    s = vPulse.size();
    mtx.unlock ();
	double d=0;
    return (vPulse.size());
}

//-----------------------------------------------------------------------------
void FilterPulse (const TDoubleVec &vPulse,double dBackground, TPulseFilter &pulse_filter, bool fFilterOnOff)
{
	TDoubleVec vSrc, vFiltered, vDiff;
	TDoubleVec v, vTrapez;
	TDoubleVec::iterator id, id1, iDiff;
	TDoubleVec::const_iterator i;
	static int nCount=0;

	pulse_filter.Clear();
	vSrc.resize (vPulse.size());
	for (i=vPulse.begin(), id=vSrc.begin() ; i != vPulse.end() ; i++, id++)
		*id = *i;// - dBackground;
	if (fFilterOnOff) {
		vTrapez = g_rp_setup.GetTrapez();
		pulse_filter.SetKernel (vTrapez);
		convolution(vSrc, vTrapez, vFiltered, vSrc.size());
		vDiff.resize (vFiltered.size());
		id1 = id = vFiltered.begin();
		for (iDiff=vDiff.begin(), id1++ ; id1 != vFiltered.end() ; id++, id1++, iDiff++)
			*iDiff = *id - *id1;
	}
	pulse_filter.SetData (vSrc, vFiltered, vTrapez, vDiff);
	if (nCount == 0) {
			nCount++;
		PrintVector (vSrc, "filt_src.csv");
		PrintVector (vTrapez, "filt_ker.csv");
		PrintVector (vFiltered, "filt_res.csv");
		PrintVector (vDiff, "filt_dif.csv");
	}
}

//-----------------------------------------------------------------------------
void SafeQueueAdd (const TPulseFilter &pulse_filter)
//void SafeQueueAdd (const TDoubleVec &vPulse, TDoubleVec &vFiltered)
//void SafeQueueAdd (const TFloatVec &vPulse, TFloatVec &vFiltered)
{
/*
	TDoubleVec v, vTrapez;
	TPulseFilter pulse_filter;
	TFloatVec::iterator i;
	TDoubleVec::iterator iv;

	pulse_filter.SetPulse (vPulse);
	vTrapez = g_rp_setup.GetTrapez();
	pulse_filter.SetKernel (vTrapez);
	pulse_filter.Filter();
*/
    mutex mtx;

    mtx.lock ();
	g_qFilteredPulses.push(pulse_filter);
    while (g_qFilteredPulses.size() > 1000)
        g_qFilteredPulses.pop();
    mtx.unlock ();
}

//-----------------------------------------------------------------------------
void SafeQueueClear ()
{
    mutex mtx;

    mtx.lock ();
	while (!g_qFilteredPulses.empty())
    //while (!g_qPulses.empty())
		//g_qPulses.pop();
		g_qFilteredPulses.pop();
    mtx.unlock ();
}

//-----------------------------------------------------------------------------
size_t SafeQueueSize ()
{
    size_t s;
    mutex mtx;

    mtx.lock ();
	s = g_qFilteredPulses.size();
    //s = g_qPulses.size();
	//s = g_vPulsesInfo.size();
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
		string str = StringifyJson(jSampling);
		strResult = trimString (str);
		//fprintf (stderr, "HandleSampling, JSON='%s'\n", strResult.c_str());
		if (!jSampling["signal"].isNull()) {
			bool fOnOff = jSampling["signal"].asBool();
			//fprintf (stderr, "HandleSampling, 385\n");
			//fprintf (stderr, "signal command: %d\n", jSampling.asBool());
			//bool fSignal = str_to_bool (ToLower (jSampling["signal"].asString()));
			//fprintf (stderr, "HandleSampling, 415\n");
			rp_setup.SetSamplingOnOff (fOnOff);
			//rp_setup.SetSamplingOnOff (str_to_bool (ToLower (jSampling["signal"].asString())));
			//fprintf (stderr, "HandleSampling, Sampling is '%s'\n", BoolToString (fOnOff).c_str());
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
		jResult["buffer"] = to_string(SafeQueueSize());//g_vRawSignal.size());
		fprintf (stderr, "HandleSampling, Queue size: %d\n", SafeQueueSize());
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
    //g_mca_calculator.GetSpectrum (vSpectrum);
	g_rp_setup.GetMcaSpectrum (vSpectrum);
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
    //g_mca_calculator.ResetSpectrum ();
	g_rp_setup.ResetMcaSpectrum ();
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
    TDoubleVec vPulse, vFiltered;
    TFloatVec::iterator i;
    TPulseInfoVec piVec;
    TPulseInfoVec::iterator iPulseInfo;
	static int nCount=1;
	TPulseFilter pulse_filter;

    if (g_rp_setup.GetSamplingOnOff ()) {
        if (GetNextPulse (vPulse)) {
			nCount++;
			if (nCount % 10 == 0)
				g_rp_setup.CalculateBackground (vPulse);
			FilterPulse (vPulse, g_rp_setup.GetBackground(), pulse_filter, g_rp_setup.IsFilterOn());
			if (GetPulseParams (pulse_filter, piVec))
				pulse_filter.SetPulsesInfo (piVec);
			SafeQueueAdd (pulse_filter);
			if (piVec.size() > 0)
            	g_rp_setup.NewPulse (piVec);
/*
					if (GetPulseParams (pulse_filter, piVec)) {
						pulse_filter.SetPulsesInfo (piVec);
						for (iPulseInfo=piVec.begin() ; iPulseInfo != piVec.end() ; iPulseInfo++)
							g_vPulsesInfo.push_back (*iPulseInfo);//.insert (g_vPulsesInfo.end(), piVec.begin(), piVec.end());
						g_rp_setup.NewPulse (piVec);
					}
		*/
				}
			}
		}

//-----------------------------------------------------------------------------
bool IsF1GreaterThenF2 (float f1, float f2)
{
	return (f1 >= f2);
}

//-----------------------------------------------------------------------------
bool IsF1LessThenF2 (float f1, float f2)
{
	return (f1 >= f2);
}

//-----------------------------------------------------------------------------
bool GetVectorMinMax (TFloatVec &vBuffer, double &dMin, double &dMax)
{
	TFloatVec::iterator i;
	bool fMinMax = false;

	try {
		if (vBuffer.size() > 0) {
			i = vBuffer.begin();
			dMin = dMax = (double) *i;
			for (i++ ; i != vBuffer.end() ; i++) {
				dMin = min (dMin, (double) *i);
				dMax = max (dMax, (double) *i);
			}
		}
	}
	catch (std::exception &err) {
		fprintf (stderr, "Runtime error in 'GetVectorMinMax ':\n%s\n", err.what());
		fMinMax= false;
	}
	return (fMinMax);
}

//-----------------------------------------------------------------------------
void Normalize (TFloatVec &vSource, double dMin, double dMax, TFloatVec &vResult)
{
	TFloatVec::iterator iSource, iResult;
	vResult.resize(vSource.size());
	iResult = vResult.begin();
	for (iSource=vSource.begin() ; iSource != vSource.end() ; iSource++, iResult++) {
		*iResult = -(*iSource) + dMax;
	}
}

//-----------------------------------------------------------------------------
bool FindPulseStartEnd (TFloatVec::iterator iBufferStart, TFloatVec::iterator iBufferEnd, double dBkgnd, TFloatVec::iterator &iStart, TFloatVec::iterator &iEnd)
{
	TFloatVec::iterator i, iSignalStart;
	int n, nStart=-1, nFound = -1, nEnd = -1;
	bool f = iStart == iBufferEnd;

	iEnd = iBufferEnd;
	iStart = iBufferStart;
	f = i != iBufferEnd;
	for (i=iBufferStart, n=0 ; (iStart == iBufferStart) && (i != iBufferEnd) ; i++, n++) {
		if (*i > dBkgnd) {
			if (nStart < 0) {
				iSignalStart = i;
				nStart = n;
			}
			else if ((n - nStart) >= 3) {
				iStart = iSignalStart;
				nFound = n;
			}
		}
		else
			if (nStart >= 0)
				nStart = -1;
	}
	if (iStart != iBufferStart) {
		for (i=iStart ; (iEnd == iBufferEnd) && (i != iBufferEnd) ;  n++, i++) {
			if (*i < dBkgnd) {
				iEnd = i;
				nEnd = n;
			}
		}
	}
	f = (iStart != iBufferStart) && (iEnd != iBufferEnd);
	return (f);
}

//-----------------------------------------------------------------------------
bool GetPulseParams (const TPulseFilter &pulse_filter, TPulseInfoVec &piVec)
// calculate filtered height
{
	TDoubleVec::const_iterator i;
	bool fInPulse=false;
	double dMax;
	size_t n;
	TPulseIndex pi;
	TPulseInfo pulse_info;

	i = pulse_filter.GetFilteredBegin();
	dMax = *i;
	for (i=pulse_filter.GetFilteredBegin(), n=0 ; i != pulse_filter.GetFilteredEnd() ; i++, n++) {
		if (fInPulse) {
			dMax = min (*i, dMax);
			if (*i > -0.02) { // pulse end
				fInPulse = false;
				pi.SetSteps (n - pi.GetStart());
				pulse_info.SetMaxVal (dMax);
				pulse_info.SetIndices (pi);
				piVec.push_back (pulse_info);
				pulse_info.Clear();
				fInPulse = false;
			}
		}
		else {
			if (*i < -0.02) { // pulse start
				dMax = fabs (*i);
				pi.SetStart (n);
				fInPulse = true;
			}
		}
	}
	return (piVec.size() > 0);
}
//-----------------------------------------------------------------------------
bool GetPulseParams_from_source (const TPulseFilter &pulse_filter, TPulseInfoVec &piVec)
{
	TPulseIndexVec vIndices;
	TPulseIndexVec::iterator iPulse;
	TPulseInfo pi;
	size_t n;
	double dMax, dArea, v;

	IdentifyPulses (pulse_filter, vIndices);
	for (iPulse=vIndices.begin() ; iPulse != vIndices.end() ; iPulse++) {
		dMax = dArea = 0;
		for (n=iPulse->GetStart() ; n < iPulse->GetEnd() ; n++) {
			v = pulse_filter.GetValueAt (n);
			dMax = max (dMax, v);
			dArea += v;
		}
		pi.SetArea (dArea);
		pi.SetMaxVal (dMax);
		pi.SetLength (iPulse->GetEnd() - iPulse->GetStart());
		pi.SetIndices (*iPulse);
		piVec.push_back (pi);
	}
	return (piVec.size() > 0);
}

//-----------------------------------------------------------------------------
void IdentifyPulses (const TPulseFilter &pulse_filter, TPulseIndexVec &vIndices)
{
	TDoubleVec vDiff;
	TDoubleVec::iterator i, iStart, iEnt;
	int nStart, nEnd, n;
	bool fInPulse=false;
	TPulseIndex pi;

	pulse_filter.GetDiff(vDiff);
	for (i=vDiff.begin(), n=0 ; i != vDiff.end() ; i++, n++) {
		if (!fInPulse) {
			if (*i > 0.001) {
				nStart = n;
				fInPulse = true;
			}
		}
		else {
			if (*i < 0.001) {
				nEnd = n;
				pi.SetPulse (nStart, n, n - nStart);
				fInPulse = false;
				vIndices.push_back (pi);
				pi.Clear();
			}
		}
	}
}

//-----------------------------------------------------------------------------
bool GetPulseParams_no_filter (TDoubleVec &vBuffer, TPulseInfoVec &piVec)
{
	TPulseIndexVec vIndices;
	TPulseIndexVec::iterator iPulse;
	TDoubleVec::iterator iStart, iEnd;

	CountPulsesInSignal (vBuffer, vIndices);
	for (iPulse=vIndices.begin() ; iPulse != vIndices.end() ; iPulse++) {
		iStart = vBuffer.begin() + iPulse->GetStart();
		iEnd = iStart + iPulse->GetSteps();
		AddPulse (iStart, iEnd, g_rp_setup.GetBackground(), piVec);
	}
	return (piVec.size() > 0);
}

//-----------------------------------------------------------------------------
void AddPulse (TDoubleVec::iterator iStart, TDoubleVec::iterator iEnd, double dBkgnd, TPulseInfoVec &piVec)
//void AddPulse (TFloatVec::iterator iStart, TFloatVec::iterator iEnd, double dBkgnd, TPulseInfoVec &piVec)
{
	TPulseInfo pi;
	double dArea, dMax;
	TDoubleVec::iterator i;
	//TFloatVec::iterator i;

	pi.AddPulse (iStart, iEnd);
	dArea = dMax = *iStart - dBkgnd;
	for (i=iStart + 1 ; i < iEnd ; i++) {
		dArea += (*iStart - dBkgnd);
		dMax = max (dMax, (double) (*i - dBkgnd));
	}
	pi.SetArea (dArea);
    pi.SetMaxVal (dMax);
	pi.SetBackground (dBkgnd);
	piVec.push_back (pi);
}

//-----------------------------------------------------------------------------
TFloatVec::iterator FindPulseEnd (TFloatVec::iterator itBegin, TFloatVec::iterator itEnd, double dBackground)
{
    TFloatVec::iterator it, itFound=itEnd, itStart=itEnd;
    size_t idxStart, idx;
    int nCount;

    idxStart = 0;
    for (it=itBegin, idx=1 ; (it != itEnd) && (itFound == itEnd) ; it++, idx++) {
        if (*it <= dBackground) {
            if (itStart == itEnd) {
                itStart = it;
                idxStart = idx;
                nCount = 1;
            }
            else if (idx == (idxStart + 1))
                nCount++;
            else {
                itStart = it;
                idxStart = idx;
                nCount = 1;
            }
        }
        else {
            idxStart = nCount = 0;
        }
        if (nCount >= 3)
            itFound = it;
    }
    return (itFound);
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
bool GetNextPulse (TDoubleVec &vPulse)
//bool GetNextPulse (TFloatVec &vPulse)
{
    bool fPulse = false;
	static int n;
#ifdef  _RED_PITAYA_HW
    fPulse = ReadHardwareSamples (g_rp_setup, vPulse);
	//PrintBool ("GetNextPulse:616", fPulse);
#else
    fPulse = ReadVectorFromFile ("pulse.csv", vPulse);
#endif
	if (n < 10) {
		n++;
		AddToCsv(vPulse, "hardware.csv");
	}
    return (fPulse);
}

//-----------------------------------------------------------------------------
#ifdef  _RED_PITAYA_HW
bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TDoubleVec &vPulse)
//bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TFloatVec &vPulse)
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
	//fprintf (stderr, "Trigger Source: %s\n", GetHardwareTriggerName (rp_setup.GetHardwareTriggerSource()).c_str());
	usleep(1);
	rp_AcqSetTriggerSrc(rp_setup.GetHardwareTriggerSource());
	//rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
	//fprintf (stderr, "Trigger Source Set: %s\n", GetHardwareTriggerName (rp_setup.GetHardwareTriggerSource()).c_str()); 
	cStart = clock();
	int nDebug=0;
	while((fTrigger == false) && (fTimeout == false)){
		//fprintf (stderr, "debug: %d\n", nDebug);
		nDebug++;
		rp_AcqGetTriggerState(&state);
		if (state == RP_TRIG_STATE_TRIGGERED)//{
			fTrigger =true;
		else
			fprintf (stderr, "No Trigger\r");
		dDiff = (clock() - cStart);
		dDiff /= (double) CLOCKS_PER_SEC;
		//fprintf (stderr, "fDiff = %g\n", dDiff);
		if (dDiff >= 3)
			fTimeout = true;
		//fprintf (stderr, "fDiff = %g\nfTimeout=%s\n", dDiff, BoolToString(fTimeout).c_str());
	}
    if (fTrigger) {
	    uint32_t uiTriggerPos, uiLen=buff_size, n;
		int64_t time_ns;
        TDoubleVec::iterator i;
        //TFloatVec::iterator i;
	    rp_AcqGetWritePointerAtTrig (&uiTriggerPos);
	    //rp_AcqGetDataV (RP_CH_1, uiTriggerPos, &uiLen, buff);
	    //rp_AcqGetDataV (RP_CH_1, uiTriggerPos - 100, &uiLen, buff);
	    rp_AcqGetDataV (RP_CH_1, uiTriggerPos - (int) rp_setup.GetPreTriggerNs() , &uiLen, buff);
	    //rp_AcqGetDataRaw (RP_CH_1, uiTriggerPos, &uiLen, auiBuffer);
        vPulse.resize (buff_size, 0);
        for (n=0, i=vPulse.begin() ; n < (int) buff_size ; n++, i++) //{
			*i = (double) buff[n];
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

/*
//-----------------------------------------------------------------------------
void SafeAddToMca (const TFloatVec &vPulse)
{
	mutex mtx;

    mtx.lock ();
	g_rp_setup.NewPulse (vPulse);
    //g_mca_calculator.NewPulse (vPulse);
    mtx.unlock ();
}
*/

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

	g_rp_setup.GetMcaSpectrum (vSpectrum);
    //g_mca_calculator.GetSpectrum (vSpectrum);
    for (i=vSpectrum.begin() ; i <= vSpectrum.end() ; i++) {
		if (*i > 0)
			*i += 0;
        sprintf (sz, "%.3f", *i);
        jMCA.append (sz);
    }
    return (jMCA);
}

//-----------------------------------------------------------------------------
Json::Value ContinueReadSignal (Json::Value jCmd, TFloatVec &vSignal)
{
	Json::Value jReply;
	TFloatVec::iterator it;

	try {
		int n, nSize = jCmd["signal"].asInt();
		int nStart = jCmd["start"].asInt();
		for (n=0, it=vSignal.begin() ; (n < nStart) && (it != vSignal.end()) ; n++)
			it++;
		for (n=0 ; (n < nSize) && (it != vSignal.end()) ; n++, it++)
			jReply["signal"].append (*it);
		jReply["length"] = to_string(n);
	}
    catch (std::exception exp) {
		fprintf (stderr, "Runtime error in 'ContinueReadSignal ':\n%s\n", exp.what());
		jReply["error"] = exp.what();
	}
/*
	string strReply = StringifyJson (jReply);
	fprintf (stderr, "+----------------------------------------------------+\n");
	fprintf (stderr, "Reply:\n%s\n", strReply.c_str());
	fprintf (stderr, "+----------------------------------------------------+\n");
*/
	return (jReply);
}

//-----------------------------------------------------------------------------
void GetAppDateTime (char *szFileName)
{
	struct stat t_stat;
	stat (szFileName, &t_stat);
	struct tm *timeinfo = localtime (&t_stat.st_ctime);
	g_strAppDate = asctime (timeinfo);
}
