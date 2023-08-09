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

#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

#include <mutex>
#include <thread>

using namespace std;

#include "rp_setup.h"
#include "misc.h"
#include "timer.h"
#include "bd_types.h"
#include "calc_mca.h"
#include "mca_info.h"
#include "pulse_info.h"
#include "rp_server.h"
#include "trim.h"
#include "pulse_index.h"
#include "trim.h"

#include "PulseFilterInfo.h"
#include "jsoncpp/json/json.h"

#include "MutexQueue.h"

#include "sampling.h"
//-----------------------------------------------------------------------------
TDoubleVecQueue g_qPulses;
TFloatVecQueue g_qFiltered;
TFloatVecQueue g_qDebug;
TFloatVec g_vRawSignal;

TPulseFilterQueue g_qFilteredPulses;
TPulseInfoVec g_vPulsesInfo;
bool g_fRunning = false;
string g_strAppDate;
static const char *g_szReadData = "read_data";
static const char *g_szBufferLength = "buffer_length";
static const char *g_szReadMca   = "rea_dmca";
const char *g_szSampling  = "sampling";
const char *g_szStatus    = "status";
static const char *g_szAll       = "all";
static const char *g_szError     = "error";
static const char *g_szReset     = "reset";
static const char *g_szMCA       = "MCA";
static const char *g_szSaveMCA   = "SaveMca";
static const char *g_szDefaultSetup = "rp_setup.json";
bool g_fMca = false;
//-----------------------------------------------------------------------------

bool HandleMessage (const string &strJson, Json::Value &jReply, TRedPitayaSetup &rp_setup, TMutexMcaInfo *pMutexMca, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess, TMcaParams &mca_params);
void SendReply (void *responder, char *szRecvBuffer, Json::Value jReplyMessage, TRedPitayaSetup &rp_setup);
Json::Value HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup, TMutexMcaInfo *pMutexMca);
//Json::Value HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup, TMcaParams &mca_params);
Json::Value HandleReadData(Json::Value &jRead, TRedPitayaSetup &rp_setup, TMutexMcaInfo *pMutexMca, TFloatVec &vSignal, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess);
Json::Value HandleMCA(Json::Value &jMCA, TRedPitayaSetup &rp_setup);
Json::Value ContinueReadSignal (Json::Value jCmd, TFloatVec &vSignal);
void ExportDebugPulse(TFloatVecQueue &qDebug);
void SafeStartStop (bool fCommand);
bool SafeGetStatus (TRedPitayaSetup &rp_setup);
size_t SafeQueueSize ();
size_t SafeQueueExtract (TDoubleVec &vPulse, TDoubleVec &vFiltered, TDoubleVec &vKernel, TPulseIndexVec &vIndices);
void SafeQueueAdd (const TPulseFilter &pulse_filter);
//void SafeSetMca (bool fOnOff, TRedPitayaSetup &rp_setup);
//bool SafeGetMca ();
//TMcaParams SafeGetMcaParams ();
//void SafeSetMcaParams (const TMcaParams &params);
//void SafeResetMca(TRedPitayaSetup &rp_setup);
//void SafeReadMca (TRedPitayaSetup &rp_setup, Json::Value &jResult);
//void SafeGetMcaSpectrum (TRedPitayaSetup &rp_setup, TFloatVec &vSpectrum);
bool CountBraces (std::string &strJson);
int CountInString (const std::string &strJson, int c);

Json::Value ReadSignal (TRedPitayaSetup &rp_setup, bool fFiltered, bool fDeriv, double dLen, bool fDebug, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess);
void IdentifyPulses (const TPulseFilter &pulse_filter, TPulseIndexVec &vIndices);
TFloatVec SmoothPulse (const TFloatVec &vRawPulse);
Json::Value ReadMca (TRedPitayaSetup &rp_setup, TMcaParams &mca_params);
//Json::Value ReadMca (TRedPitayaSetup &rp_setup);
bool FindPulseStartEnd (TFloatVec::iterator iBufferStart, TFloatVec::iterator iBufferEnd, double dBkgnd, TFloatVec::iterator &iStart, TFloatVec::iterator &iEnd);
void AddPulse (TDoubleVec::iterator iStart, TDoubleVec::iterator iEnd, double dBkgnd, TPulseInfoVec &piVec);
string GetMcaCounts (TRedPitayaSetup &rp_setup);
size_t CountPulsesInSignal (const TDoubleVec &vSignal, const TRedPitayaSetup &rp_setup, TPulseIndexVec &vIndices);
size_t CountPulsesInSignal (const TFloatVec &vSignal, const TRedPitayaSetup &rp_setup, TPulseIndexVec &vIndices);
Json::Value GetPulsesInSignal (const TFloatVec &vSignal, TRedPitayaSetup &rp_setup);
void GetAppDateTime (char *szFileName);
void AddVectorToQueue (const TDoubleVec &vPulse, mutex *pMutex);
void VectorQueueClear();
size_t VectorQueueSize();
size_t GetNextQueuePulse (TDoubleVec &vPulse);
size_t ReadNextQueuePulse (TPulseFilter &pulse_filter);
size_t FilteredPulseQueueSize();
string SaveMCA (TRedPitayaSetup &rp_setup);

//-----------------------------------------------------------------------------
std::string ret_str()
{
	std::string s = "this is a string";
	fprintf (stderr, "ret_str, s-'%s'\n", s.c_str());
	return (s);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int main (int argc, char *argv[])
{
#ifdef  _RED_PITAYA_HW
	rp_Init();
#endif
	GetAppDateTime (argv[0]);

    bool fRun=true, fUpdateHardware=false;
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
	Json::Value /*root, */jReply;
    string strReply;
	char *szRecvBuffer = new char [1024];

	TMutexDoubleVecQueue qRaw;// (&mtxRaw);
	TMutexPulseFilterQueue qProcess;// (&mtxProcess);
	mutex mtxMcaParams, mtxMcaInfo;
	TMcaParams mca_params (&mtxMcaParams);
	TRedPitayaSetup rp_setup (&mca_params);
	TMcaInfo mca_info (&mca_params);
	TMutexMcaInfo mutex_mca (&mca_info, &mtxMcaInfo);

	fprintf (stderr, "Loading Setup\n");
#ifdef	_RED_PITAYA_HW
	fUpdateHardware = true;
	//fprintf (stderr, "Loading Hardware Setup\n");
	//g_rp_setup.LoadFromHardware (true);
#else
#endif
    rp_setup.LoadFromJson(g_szDefaultSetup);
    Json::Value jSetup = rp_setup.AsJson();
    printf ("Setup: %s\n", StringifyJson(jSetup).c_str());

	std::thread threadSampling (SamplingThread, &qRaw, &rp_setup);
	//std::thread threadProcess (ProcessThread, &qRaw, &qProcess, &mca_params, &rp_setup);
	std::thread threadProcess (ProcessThread, &qRaw, &qProcess, &mutex_mca, &rp_setup);
    while (fRun) {
        zmq_recv (responder, szRecvBuffer , 1024, 0);
		std::string strJson = ToLower(szRecvBuffer);
		strJson = ReplaceAll(strJson, "\'", "\"");

		fRun = HandleMessage (strJson, jReply, rp_setup, &mutex_mca, qRaw, qProcess, mca_params);
		strReply = StringifyJson (jReply);
		SendReply (responder, szRecvBuffer, jReply, rp_setup);
    }
	delete[] szRecvBuffer;
	threadSampling.join();
	threadProcess.join();
    return 0;
}

//-----------------------------------------------------------------------------
bool HandleMessage (const string &strJson, Json::Value &jReply, TRedPitayaSetup &rp_setup, TMutexMcaInfo *pMutexMca, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess, TMcaParams &mca_params)
{
	bool fRun=true;
	string strReply;

	try {
		Json::Value root;
		Json::Reader reader;
		bool fMessageRecieved;
    	TFloatVec vSignal;

		if (reader.parse (strJson, root)) {
			fMessageRecieved = true;
			jReply.clear();
			strReply  = "";
			if (!root["setup"].isNull()) {
				string s = StringifyJson (root["setup"]);
				jReply["setup"] = HandleSetup(root["setup"], rp_setup, pMutexMca);
				//jReply["setup"] = HandleSetup(root["setup"], rp_setup, mca_params);
			if (!root["setup"][g_szMCA].isNull())
				jReply[g_szMCA] = pMutexMca->HandleMCA(root["setup"][g_szMCA]);
			}
			if (!root[g_szReadData].isNull())
				jReply["pulses"] = HandleReadData(root[g_szReadData], rp_setup, pMutexMca, vSignal, qRaw, qProcess);
				//jReply["pulses"] = HandleReadData(root[g_szReadData], rp_setup, mca_params, vSignal, qRaw, qProcess);
			if (!root[g_szSampling].isNull()) {
				jReply[g_szSampling] = HandleSampling(root[g_szSampling], rp_setup, fRun, pMutexMca, qRaw, qProcess);
			}
			if (!root[g_szMCA].isNull())
				jReply[g_szMCA] = pMutexMca->HandleMCA(root[g_szMCA]);
				//jReply[g_szMCA] = HandleMCA(root[g_szMCA], rp_setup);
		}
		else {
			fprintf (stderr, "\nParsing error\n");
			fprintf (stderr, "%s\n\n", strJson.c_str());
			strReply += strJson + "\nParsing Error";
		}
	}
	catch (std::exception &err) {
		fprintf (stderr, "Runtime error in 'HandleMessage':\n%s\n", err.what());
	}
	return (fRun);
}

//-----------------------------------------------------------------------------
void SendReply (void *responder, char *szRecvBuffer, Json::Value jReplyMessage, TRedPitayaSetup &rp_setup)
{
	try {
		Json::Value jReply;
		int nStart, nMessages, nPackSize, nCharsToSend, nTotalToSend;
		bool fDebug;
		string s1;
		FILE *file = NULL;


		string s, strReply = StringifyJson (jReplyMessage);
		strReply = trimString (strReply);
		nMessages = 1 + (int) (strReply.length() / rp_setup.GetPackageSize());
		nTotalToSend = strReply.length();
		nPackSize = rp_setup.GetPackageSize();
		fDebug = nTotalToSend > nPackSize;
		printf ("Total to send: %d (%d)\n", nTotalToSend, (int) strReply.length());
		nStart = 0;
		if (fDebug)
			file = fopen ("sd.csv", "w+");
		while (nTotalToSend > 0) {
			jReply.clear();
			s = strReply.substr (nStart, nPackSize);
			if (fDebug)
				s1 = s;
			nStart += nPackSize;
			jReply["text"] = s;
			jReply["flag"] = nTotalToSend < nPackSize;
			s = StringifyJson (jReply);
			int rc = zmq_send (responder, s.c_str(), s.length(), 0);
			nTotalToSend -= nPackSize;
			if (nTotalToSend > 0) {
        		zmq_recv (responder, szRecvBuffer , 1024, 0);
			}
			if (fDebug) {
				//fprintf (file, "%n\n", s1.length());
				fprintf (file, "%s\n", s1.c_str());
			}
		}
		if (file != NULL)
			fclose (file);
	}
	catch (std::exception &err) {
		fprintf (stderr, "Runtime error in 'SendReply':\n%s\n", err.what());
	}
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
Json::Value HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup, TMutexMcaInfo *pMutexMca)
//Json::Value HandleSetup(Json::Value &jSetup, TRedPitayaSetup &rp_setup, TMcaParams &mca_params)
{
    std::string strReply, strCommand;
    Json::Value jRead, jNew;

    try {
		bool fUpdateHardware = false;
#ifdef	_RED_PITAYA_HW
		fUpdateHardware = true;
#endif
		std::string strSetup = StringifyJson (jSetup);
        strCommand = ToLower(jSetup["command"].asString());
		Json::Value jBkgnd = jSetup["background"];
		if (strCommand == "update") {
#ifdef	_RED_PITAYA_HW
    		rp_AcqReset();
#endif
            jNew = rp_setup.UpdateFromJson(jSetup, fUpdateHardware);
			//if (!jSetup["mca"].isNull())
				//jNew["mca"] = pMutexMca->UpdateFromJson (jSetup["mca"]);
            //jNew = rp_setup.UpdateFromJson(jSetup, fUpdateHardware);
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
			TDoubleVec vSignal;
			GetNextPulse (vSignal, rp_setup);
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
		else if (strCommand == "read_mca_params") {
			jNew = pMutexMca->AsJson();//rp_setup.McaAsJson();
			//jNew = mca_params.AsJson();//rp_setup.McaAsJson();
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
		}
		strReply = StringifyJson (jNew);
#ifdef  _RED_PITAYA_HW
#endif
		rp_setup.SaveToJson (g_szDefaultSetup);
    }
    catch (std::exception &err) {
        jNew = rp_setup.AsJson();
        jNew["error"] = err.what();
    }
    return (jNew);
}

//-----------------------------------------------------------------------------
//Json::Value HandleReadData(Json::Value &jRead, TRedPitayaSetup &rp_setup, TMcaParams &mca_params, TFloatVec &vSignal, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess)
Json::Value HandleReadData(Json::Value &jRead, TRedPitayaSetup &rp_setup, TMutexMcaInfo *pMutexMca, TFloatVec &vSignal, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess)
{
    TFloatVec::iterator i;
    std::string strReply, strNumber, strPulses, strPulse;
    char szNum[128];
    Json::Value jAllPulses, jPulse(Json::arrayValue), jPulseData(Json::arrayValue), jSignal;
    int n, nPulses, j;
	bool fReset = false;
	TPulseIndexVec vIndices;

    try {
        vSignal.clear();
		strReply = StringifyJson (jRead);
		jSignal = jRead["signal"];
		string strSignal = StringifyJson (jSignal);
		if (jRead["buffer"].isNull() == false) {
			string str = jRead["buffer"].asString();
			fprintf (stderr, "Buffer Command: %s\n", str.c_str());
			if (str == "reset")
				fReset = true;
		}
		if (fReset) {
			qRaw.Clear();
			qProcess.Clear();
			pMutexMca->ResetSpectrum();
		}
		else {
			if (jRead["signal"].isNull() == false) {
				bool fDebug = jSignal["debug"].asBool();
				string strSignalLength = jSignal["length"].asString();
				fprintf (stderr, "Required length: %s\n", strSignalLength.c_str());
				double dLen = stod (strSignalLength);
            	if (dLen > 0) {
					jAllPulses["signal"] = ReadSignal (rp_setup, jSignal["filtered"].asBool(), jSignal["deriv"].asBool(), dLen, fDebug, qRaw, qProcess);
            	}
			}
        	if (!jRead["mca"].isNull()) {
				if (jRead["mca"].asBool())
            		jAllPulses["mca"] = pMutexMca->ReadMca ();//rp_setup, mca_params);
        	}
		}
        jAllPulses["buffer_length"] = to_string (qRaw.GetSize());//SafeQueueSize ());
        jAllPulses["mca_length"] = to_string (pMutexMca->GetCount());//GetMcaCounts (rp_setup);
        jAllPulses["background"] = rp_setup.GetBackground();

    }
    catch (std::exception &exp) {
        strReply = std::string("Runtime error in '': ") + std::string (exp.what());
    }
    strReply = StringifyJson(jAllPulses);
    return (jAllPulses);
}

/*
//-----------------------------------------------------------------------------
string GetMcaCounts (TRedPitayaSetup &rp_setup)
{
	TFloatVec vSpectrum;
	TFloatVec::iterator i;
	long lnSum=0;
	char sz[100];

	rp_setup.GetMcaSpectrum (vSpectrum);
	for (i=vSpectrum.begin() ; i != vSpectrum.end() ; i++)
		lnSum += *i;
	sprintf (sz, "Pulses: %d, spectrum sum: %ld", rp_setup.GetMcaPulses(), lnSum);
	return (string (sz));
}
*/

//-----------------------------------------------------------------------------
Json::Value ReadSignal (TRedPitayaSetup &rp_setup, bool fFiltered, bool fDeriv, double dLen, bool fDebug, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess)
{
    Json::Value jSignal, jSignalData, jFiltered, jKernel, jFiltDeriv;
	int nVectorPoints;
	TDoubleVec::const_iterator it, itPrev;
	std::string strNumber;
	size_t sPulse, n;
	static int nCount=0;
	TPulseFilter pulse_filter;
	TPulseIndexVec vIndices;

    try {
		double dRate = rp_setup.GetSamplingPeriod();
		if (dRate <= 0) {
			jSignalData["error"] = "ilegal sampling rate: " + to_string (dRate);
    		return (jSignalData);
		}
		nVectorPoints = int ((dLen / dRate) + 0.5);
		fprintf (stderr, "length: i%g\nBuffer: %dx\n", dLen, nVectorPoints);
		while ((jSignal.size() < nVectorPoints) && (qProcess.GetSize() > 0)) {
			qProcess.PopLastItem(pulse_filter);
			sPulse = pulse_filter.GetPulseSize();
			if (sPulse > 0) {
				for (it=pulse_filter.GetPulseBegin() ; (it != pulse_filter.GetPulseEnd()) && (jSignal.size() < nVectorPoints) ; it++)
					jSignal.append(*it);
				if (fFiltered) {
					if (pulse_filter.GetFilteredSize() > 0) {
						for (it=pulse_filter.GetFilteredBegin(), n=0 ; (it != pulse_filter.GetFilteredEnd() && (jFiltered.size() < nVectorPoints)) ; n++) {
							jFiltered.append(*it);
							itPrev = it;
							it++;
							if (fDeriv) {
								if (n < pulse_filter.GetFilteredSize() - 1)
									jFiltDeriv.append (*it - *itPrev);
								else
									jFiltDeriv.append (0);
							}
						}
					}
				}
			}
        }
		string sSignal = StringifyJson (jSignal);
		fprintf (stderr, "Signal Read: %d\n", nCount++);
		jSignalData["data"] = jSignal;
		if ((fFiltered) && (jFiltered.size() > 0))
			jSignalData["filtered"] = jFiltered;
		if ((fDeriv) && (jFiltDeriv.size() > 0))
			jSignalData["filt_deriv"] = jFiltDeriv;
		if (fDebug) {
			pulse_filter.GetPulsesIndices (vIndices);
			if (vIndices.size() > 0) {
				Json::Value jIndices;
				TPulseIndexVec::iterator iIndex;
				for (iIndex=vIndices.begin() ; iIndex != vIndices.end() ; iIndex++) {
					jIndices.append (iIndex->AsJson());
				}
				jSignalData["detector_pulse"] = jIndices;
			}
		}
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error in 'ReadSignal':\n%s\n", exp.what());
    }
	strNumber = StringifyJson (jSignalData);
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
size_t CountPulsesInSignal (const TDoubleVec &vSignal, const TRedPitayaSetup &rp_setup, TPulseIndexVec &vIndices)
{
	TDoubleVec::const_iterator i;
	bool fInPulse;
	double dBackground = rp_setup.GetBackground();
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
size_t CountPulsesInSignal (const TFloatVec &vSignal, const TRedPitayaSetup &rp_setup, TPulseIndexVec &vIndices)
{
	TFloatVec::const_iterator i;
	bool fInPulse;
	double dBackground = rp_setup.GetBackground();
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
Json::Value GetPulsesInSignal (const TFloatVec &vSignal, TRedPitayaSetup &rp_setup)
{
	TPulseIndexVec vIndices;

	CountPulsesInSignal (vSignal, rp_setup, vIndices);
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
{
	TPulseFilter pulse_filter;
    mutex mtx;
    size_t s;

    vPulse.clear();
    mtx.lock ();
	pulse_filter = g_qFilteredPulses.back();
	g_qFilteredPulses.pop_back ();
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
void SafeQueueAdd (const TPulseFilter &pulse_filter)
{
    mutex mtx;

    mtx.lock ();
	g_qFilteredPulses.push_front (pulse_filter);
    while (g_qFilteredPulses.size() > 100) {
        g_qFilteredPulses.pop_back();
	}
    mtx.unlock ();
}

//-----------------------------------------------------------------------------
void WaitForMutex (mutex &mtx, useconds_t usec)
{
	while (mtx.try_lock())
		usleep(usec);
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

bool SafeGetStatus (TRedPitayaSetup &rp_setup)
{
    return (rp_setup.GetSamplingOnOff ());
}
//-----------------------------------------------------------------------------

void SafeSetMca (bool fOnOff, TRedPitayaSetup &rp_setup)
{
    rp_setup.SetSamplingOnOff (fOnOff);
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

clock_t g_cStart;

/*
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
            SafeSetMca(true, rp_setup);
            jResult[g_szStatus] = SafeGetMca();
        }
        else if (str == "false") {
            SafeSetMca(false, rp_setup);
            jResult[g_szStatus] = SafeGetMca();
        }
        else if (str == g_szReset) {
            SafeResetMca(rp_setup);
            jResult[g_szStatus] = g_szReset;
			jResult["pulse_count"] = to_string(g_qDebug.size());
        }
        else if (str == g_szReadMca)
            SafeReadMca (rp_setup, jResult);
        else if (str == g_szSaveMCA)
            jResult = SaveMCA (rp_setup);
        else
            jResult[g_szMCA] = "Command Unknown";
    }
    catch (std::exception &exp) {
        jResult[g_szError] = exp.what();
    }
    return (jResult);
}
*/

/*
//-----------------------------------------------------------------------------
void SafeGetMcaSpectrum (TRedPitayaSetup &rp_setup, TFloatVec &vSpectrum)
{
    mutex mtx;

    mtx.lock();
	rp_setup.GetMcaSpectrum (vSpectrum);
    mtx.unlock();
}

//-----------------------------------------------------------------------------
void SafeReadMca (TRedPitayaSetup &rp_setup, Json::Value &jResult)
{
    TFloatVec::iterator i;
    TFloatVec vSpectrum;

    SafeGetMcaSpectrum (rp_setup, vSpectrum);
    for (i=vSpectrum.begin() ; i != vSpectrum.end() ; i++)
        jResult[g_szMCA].append(*i);
}

//-----------------------------------------------------------------------------
void SafeResetMca(TRedPitayaSetup &rp_setup)
{
    mutex mtx;

    mtx.lock();
	rp_setup.ResetMcaSpectrum ();
    mtx.unlock();
}
*/

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
void AddVectorToQueue (const TDoubleVec &vPulse, mutex *pMutex)
{
    //mutex mtx;

    //mtx.lock ();
	while (pMutex->try_lock())
		usleep(1);
	pMutex->lock();
	g_qPulses.push (vPulse);
	while (g_qPulses.size() > 500) {
		g_qPulses.pop();
		//printf ("Max Number\n");
	}
	////mtx.unlock();
	pMutex->unlock();
}

//-----------------------------------------------------------------------------
size_t ReadNextQueuePulse (TPulseFilter &pulse_filter)
{
    mutex mtx;
	static size_t n;

	pulse_filter.Clear();
    mtx.lock ();
	n = g_qFilteredPulses.size();
	pulse_filter = g_qFilteredPulses.back ();
	g_qFilteredPulses.pop_back ();
	mtx.unlock();
	n = g_qFilteredPulses.size();
	return (pulse_filter.GetPulseSize());
}

//-----------------------------------------------------------------------------
void AddFiteredPulseToQueue (const TPulseFilter &pulse_filter)
{
    mutex mtx;

    mtx.lock ();
	g_qFilteredPulses.push_front (pulse_filter);
	while (g_qFilteredPulses.size() > 100) {
		g_qFilteredPulses.pop_back();
	}
	mtx.unlock();
}

//-----------------------------------------------------------------------------
size_t FilteredPulseQueueSize()
{
    mutex mtx;
	size_t n;

    mtx.lock ();
	n = g_qFilteredPulses.size();
	mtx.unlock();
	return (n);
}

TDoubleVec g_adFilter, g_adProcess;

bool g_fTimerBusy = false;
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
bool GetPulseParams (const TRedPitayaSetup &rp_setup, const TPulseFilter &pulse_filter, TPulseInfoVec &piVec)
// calculate filtered height
{
	TDoubleVec::const_iterator i;
	bool fInPulse=false;
	double dMax, dAvg;
	size_t n, nAvg;
	TPulseIndex pi;
	TPulseInfo pulse_info;

	i = pulse_filter.GetFilteredBegin();
	for (n=0 ; n < rp_setup.GetTrapezSize() ; n++)
		i++;
	dAvg = 0;
	nAvg = 0;
	while (n < (int) (0.9 * rp_setup.GetPreTriggerNs())) {
		dAvg += *i;
		nAvg++;
		i++;
		n++;
	}
	dAvg /= nAvg;
	dMax = *i;
	static bool fDebug=false;
	if (fDebug)
		//PrintVector (pulse_filter.GetFilteredBegin(), pulse_filter.GetFilteredEnd(), "filt.txt");
		PrintVector (pulse_filter.GetFilteredBegin(), pulse_filter.GetFilteredEnd(), "filt.csv");
	for (i=pulse_filter.GetFilteredBegin(), n=0 ; i != pulse_filter.GetFilteredEnd() ; i++, n++) {
		if (fInPulse) {
			dMax = min (*i, dMax);
			//if (*i > dAvg - 0.02) { // pulse end
			//if (*i > dAvg - rp_setup.GetFilteredThreshold ()) { // pulse end
			if (*i > rp_setup.GetFilteredThreshold ()) { // pulse end
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
			//if (*i < dAvg - 0.02) { // pulse start
			//if (*i < (dAvg - rp_setup.GetFilteredThreshold ())) { // pulse start
			if (*i < rp_setup.GetFilteredThreshold ()) { // pulse start
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
bool GetPulseParams_no_filter (TDoubleVec &vBuffer, TRedPitayaSetup &rp_setup, TPulseInfoVec &piVec)
{
	TPulseIndexVec vIndices;
	TPulseIndexVec::iterator iPulse;
	TDoubleVec::iterator iStart, iEnd;

	CountPulsesInSignal (vBuffer, rp_setup, vIndices);
	for (iPulse=vIndices.begin() ; iPulse != vIndices.end() ; iPulse++) {
		iStart = vBuffer.begin() + iPulse->GetStart();
		iEnd = iStart + iPulse->GetSteps();
		AddPulse (iStart, iEnd, rp_setup.GetBackground(), piVec);
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
            *iSmooth = f;
        }
    }
    return (vSmooth);
}

//-----------------------------------------------------------------------------
#ifdef  _RED_PITAYA_HW
bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TDoubleVec &vPulse)
//bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TFloatVec &vPulse)
{
    uint32_t buff_size = 16384;
    static float *buff = NULL;//(float *)malloc(buff_size * sizeof(float));
	if (buff == NULL)
    	buff = (float *)malloc(buff_size * sizeof(float));
	//int16_t* auiBuffer = (int16_t*) calloc (buff_size, sizeof (auiBuffer[0]));
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
	rp_AcqSetArmKeep (true);
	//float rHyst;
	rp_AcqSetTriggerHyst (1e-3);
	//rp_AcqGetTriggerHyst (&rHyst);
	//printf ("Hysterezis: %g mV\n", 1e3 * rHyst);
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
		usleep(1);
		rp_AcqGetTriggerState(&state);
		if (state == RP_TRIG_STATE_TRIGGERED)//{
			fTrigger = true;
		//else
			//fprintf (stderr, "No Trigger\r");
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
	//free (buff);
	//free (auiBuffer);
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

/*
//-----------------------------------------------------------------------------
string SaveMCA (TRedPitayaSetup &rp_setup)
{
    string strResult;
    TFloatVec vSpectrum;
    TFloatVec::iterator i;

    try {

        strResult = "mca.csv";
        SafeGetMcaSpectrum (rp_setup, vSpectrum);
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
*/

/*
//-----------------------------------------------------------------------------
Json::Value ReadMca (TRedPitayaSetup &rp_setup, TMcaParams &mca_params)
{
    Json::Value jMCA, jMcaData;
    TFloatVec vSpectrum;
    TFloatVec::iterator i;

	try {
		rp_setup.GetMcaSpectrum (vSpectrum);
    	for (i=vSpectrum.begin() ; i < vSpectrum.end() ; i++) {
        	jMcaData.append (*i);
    	}
		jMCA["mca_count"] = (double) mca_params.GetCount();//rp_setup.GetMcaCount();
		jMCA["mca_min"] = mca_params.GetMin();//rp_setup.GetMcaMin();
		jMCA["mca_max"] = mca_params.GetMax();//rp_setup.GetMcaMax();
		jMCA["mca_min_v"] = mca_params.GetMinVoltage();//.rp_setup.GetMcaMinVoltage();
		jMCA["mca_max_v"] = mca_params.GetMaxVoltage();//rp_setup.GetMcaMaxVoltage();
		jMCA["mca_time"] = rp_setup.GetMcaMeasureTime ();
		jMCA["mca_data"] = jMcaData;
		string s;
		s = StringifyJson (jMCA);
	}
    catch (std::exception exp) {
		fprintf (stderr, "Runtime error in 'ReadMca':\n%s\n", exp.what());
		jMCA["error"] = exp.what();
	}
    return (jMCA);
}
*/

//-----------------------------------------------------------------------------
void GetAppDateTime (char *szFileName)
{
	struct stat t_stat;
	stat (szFileName, &t_stat);
	struct tm *timeinfo = localtime (&t_stat.st_ctime);
	g_strAppDate = asctime (timeinfo);
}

//-----------------------------------------------------------------------------
Json::Value GetVectorAsJson (TDoubleVec::const_iterator iStart, TDoubleVec::const_iterator iEnd)
{
	Json::Value jVec;
	TDoubleVec::const_iterator i;

	for (i=iStart ; i != iEnd ; i++)
		jVec.append (*i);
	return (jVec);
}

void *g_socket;
bool g_fRecv;
//-----------------------------------------------------------------------------
void WaitForRecv ()
{
	g_fRecv = false;
	char szBuff[1024];
	int nConnect = zmq_recv (g_socket, szBuff, 0, ZMQ_NOBLOCK);
	fprintf (stderr, "%d bytes recieved:\n%s\n", nConnect, szBuff);
	g_fRecv = true;
}

//-----------------------------------------------------------------------------
void RemoteProcess (const TRemoteProcessing &remote_processing, const TDoubleVec &vPulse)
{
    int n, nPort;
    void *context = zmq_ctx_new ();
	char szRemoteAddress[256], szHost[100], szBuff[1024];
	Json::Value j;
    Timer t;

	g_socket = zmq_socket (context, ZMQ_REQ);
	sprintf (szRemoteAddress, "tcp://%s:%d", remote_processing.GetHost().c_str(), remote_processing.GetPort());
	int nConnect = zmq_connect(g_socket, szRemoteAddress);
	j["pulse"] = GetVectorAsJson (vPulse.begin(), vPulse.end());
	string str = StringifyJson (j);
	str = trimString (str);

	nConnect = zmq_send (g_socket, str.c_str(), str.length(), 0);

	t.setInterval (WaitForRecv, 1000);
	usleep(1000);
	zmq_close (g_socket);
	zmq_ctx_destroy(context);
}

//-----------------------------------------------------------------------------
void RemoteProcess (const TRemoteProcessing &remote_processing, TPulseFilter &pulse_filter)
{
    int n, nPort;
    void *context = zmq_ctx_new ();
    //void *socket;
	char szRemoteAddress[256], szHost[100], szBuff[1024];
	Json::Value j;
    Timer t;

	//socket = zmq_socket (context, ZMQ_REQ);
	g_socket = zmq_socket (context, ZMQ_REQ);
	sprintf (szRemoteAddress, "tcp://%s:%d", remote_processing.GetHost().c_str(), remote_processing.GetPort());
	//int nConnect = zmq_connect(socket, szRemoteAddress);
	int nConnect = zmq_connect(g_socket, szRemoteAddress);
	j["pulse"] = GetVectorAsJson (pulse_filter.GetPulseBegin(), pulse_filter.GetPulseEnd());
    if (pulse_filter.GetFilteredSize() > 0)
		j["filtered"] = GetVectorAsJson (pulse_filter.GetFilteredBegin(), pulse_filter.GetFilteredEnd());
	string str = StringifyJson (j);
	str = trimString (str);
	//nConnect = zmq_send (socket, str.c_str(), str.length(), 0);
	nConnect = zmq_send (g_socket, str.c_str(), str.length(), 0);
	//printf ("\n%d Sent bytes\n", nConnect);

	t.setInterval (WaitForRecv, 1000);
	usleep(1000);
	//nConnect = zmq_recv (socket, szBuff, 0, ZMQ_NOBLOCK);
	//zmq_close (socket);
	zmq_close (g_socket);
	zmq_ctx_destroy(context);
}
