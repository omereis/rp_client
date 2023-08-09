/******************************************************************************\
|                               sampling.cpp                                   |
\******************************************************************************/

#include "sampling.h"
#include "trim.h"
#include <time.h>

#include <unistd.h>

#include "MutexQueue.h"
#include "rp_server.h"
extern clock_t g_cStart;
extern const char *g_szSampling;

void FilterPulse (const TDoubleVec &vPulse,double dBackground, TPulseFilter &pulse_filter, bool fFilterOnOff, TRedPitayaSetup *pSetup);
//-----------------------------------------------------------------------------
void SamplingThread (TMutexDoubleVecQueue *qmtxRaw, TRedPitayaSetup *pSetup)
{
	static bool fTimerBusy = false;
    TDoubleVec vPulse;

	while (1) {
		if (!fTimerBusy) {
			fTimerBusy = true;
    		if (pSetup->GetSamplingOnOff ()) {
        		if (GetNextPulse (vPulse, *pSetup))
					qmtxRaw->AddItem (vPulse);
					//AddVectorToQueue (vPulse, pMutex);
				if (pSetup->IsRemoteProcessingOn())
					RemoteProcess (pSetup->GetRemoteProc(), vPulse);
			}
			fTimerBusy = false;
		}
		else
			printf ("Time busy\n");
		usleep (1);
	}
}

//-----------------------------------------------------------------------------
//void ProcessThread (TMutexDoubleVecQueue *pqmtxRaw, TMutexPulseFilterQueue *pqProcess, TRedPitayaSetup *pSetup)
//void ProcessThread (TMutexDoubleVecQueue *pqmtxRaw, TMutexPulseFilterQueue *pqProcess, TMcaParams *pMcaParams, TRedPitayaSetup *pSetup)
//void ProcessThread (TMutexDoubleVecQueue *pqmtxRaw, TMutexPulseFilterQueue *pqProcess, TMcaInfo *pMcaInfo, TRedPitayaSetup *pSetup)
void ProcessThread (TMutexDoubleVecQueue *pqmtxRaw, TMutexPulseFilterQueue *pqProcess, TMutexMcaInfo *pMutexMcaInfo, TRedPitayaSetup *pSetup)
{
    TDoubleVec vPulse;
	TPulseFilter pulse_filter;
    TPulseInfoVec piVec;
	int nCount=0;

	while (1) {
    	if (pqmtxRaw->GetSize() > 0) {
			pqmtxRaw->PopLastItem (vPulse);
    		if (vPulse.size() > 0) {
				try {
					FilterPulse (vPulse, pSetup->GetBackground(), pulse_filter, pSetup->IsFilterOn(), pSetup);
					if (GetPulseParams (*pSetup, pulse_filter, piVec))
						pulse_filter.SetPulsesInfo (piVec);
					//if (fDebug)
						//PrintVector (vPulse, "prc_pulse.txt");
					pqProcess->AddItem (pulse_filter);
					if (pMutexMcaInfo->GetMcaOnOff())
						pMutexMcaInfo->NewPulses (piVec);
					//if (piVec.size() > 0)
            			//pSetup->NewPulse (piVec);
				}
				catch (std::exception &exp) {
					fprintf (stderr, "Runtime Error in ProcessThread:\n%s\n", exp.what());
				}
			}
		}
		if (nCount++ > 5) {
			nCount = 0;
			if (pMutexMcaInfo->GetMcaOnOff()) {
				nCount = 0;
				double dMeasure = pMutexMcaInfo->GetMcaMeasureTime();
				double dMcaTime = pMutexMcaInfo->GetMcaTimeLimit();
				if (dMcaTime <= dMeasure)
					pMutexMcaInfo->StopMca();
				//printf ("MCA Time: %g\n", d);
			}
		}
		usleep(1);
	}
}

//-----------------------------------------------------------------------------
void FilterPulse (const TDoubleVec &vPulse,double dBackground, TPulseFilter &pulse_filter, bool fFilterOnOff, TRedPitayaSetup *pSetup)
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
		vTrapez = pSetup->GetTrapez();
		//vTrapez = g_rp_setup.GetTrapez();
		pulse_filter.SetKernel (vTrapez);
		convolution(vSrc, vTrapez, vFiltered, vSrc.size());
		vDiff.resize (vFiltered.size());
		id1 = id = vFiltered.begin();
		for (iDiff=vDiff.begin(), id1++ ; id1 != vFiltered.end() ; id++, id1++, iDiff++)
			*iDiff = *id - *id1;
	}
	pulse_filter.SetData (vSrc, vFiltered, vTrapez, vDiff);
}

//-----------------------------------------------------------------------------
Json::Value HandleSampling(Json::Value &jSampling, TRedPitayaSetup &rp_setup, bool &fRun, TMutexMcaInfo *pMutexMca, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess)
{
    std::string strResult;
    Json::Value jResult, jStatus;
    bool fCommandOK;
	static int nCount;

    try {
        fCommandOK = true;
		string str = StringifyJson(jSampling);
		strResult = trimString (str);
		//fprintf (stderr, "HandleSampling, JSON='%s'\n", strResult.c_str());
		if (!jSampling["signal"].isNull()) {
			bool fOnOff = jSampling["signal"].asBool();
			if ((fOnOff) && (rp_setup.GetSamplingOnOff () == false))
				g_cStart = clock();
			//fprintf (stderr, "HandleSampling, 385\n");
			//fprintf (stderr, "signal command: %d\n", jSampling.asBool());
			//bool fSignal = str_to_bool (ToLower (jSampling["signal"].asString()));
			//fprintf (stderr, "HandleSampling, 415\n");
			rp_setup.SetSamplingOnOff (fOnOff);
			//rp_setup.SetSamplingOnOff (str_to_bool (ToLower (jSampling["signal"].asString())));
			//fprintf (stderr, "HandleSampling, Sampling is '%s'\n", BoolToString (fOnOff).c_str());
		}
		if (!jSampling["mca"].isNull()) {
			pMutexMca->HandleMcaCommands (jSampling["mca"]);
			if (!jSampling["mca_time"].isNull()) {
				string sTime = jSampling["mca_time"].asString();
				double d = atof (sTime.c_str());
				printf ("\nMCA Required Time: %s, %g seconds\n\n", sTime.c_str(), d);
			}
			string strMca = jSampling["mca_time"].asString();
			printf ("MCA Time: %s\n", strMca.c_str());
			pMutexMca->SetMcaOnOff (ToLower (jSampling["mca"].asString()), jSampling["mca_time"].asString());
			//rp_setup.SetMcaOnOff (ToLower (jSampling["mca"].asString()), jSampling["mca_time"].asString());
			//rp_setup.SetMcaOnOff (str_to_bool (ToLower (jSampling["mca"].asString())), jSampling["mca_time"].asString());
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
		jStatus["mca"] = pMutexMca->GetMcaOnOff ();
		jStatus["psd"] = rp_setup.GetPsdOnOff ();
		jResult["status"] = jStatus;
		jResult["raw_buffer"] = to_string(qRaw.GetSize());//SafeQueueSize(mtxPulsesQueue));//VectorQueueSize());
		jResult["process_buffer"] = to_string(qProcess.GetSize());//SafeQueueSize(mtxPulsesQueue));//VectorQueueSize());
		jResult["mca"] = pMutexMca->ReadMca();//GetMcaMeasureTime ();
		string strMca = StringifyJson (jResult["mca"]);
		strMca += "";
		nCount++;
    }
    catch (std::exception &exp) {
		fprintf (stderr, "Runtime error in 'HandleSampling':\n%s\n", exp.what());
        jResult[g_szSampling] = exp.what();
    }
    return (jResult);
}

//-----------------------------------------------------------------------------
bool GetNextPulse (TDoubleVec &vPulse, TRedPitayaSetup &rp_setup)
{
    bool fPulse = false;
	static int n;
#ifdef  _RED_PITAYA_HW
	clock_t c = clock();
	double d = (c - g_cStart) / CLOCKS_PER_SEC;
	static double dLast;
	if (d != dLast) {
		dLast = d;
	}
    fPulse = ReadHardwareSamples (rp_setup, vPulse);
#else
    fPulse = ReadVectorFromFile ("pulse.csv", vPulse);
#endif
    return (fPulse);
}
