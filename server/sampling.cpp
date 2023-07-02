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
void SamplingThread (void *pParam1, void *pParam2)
{
	TMutexDoubleVecQueue *qmtxRaw = (TMutexDoubleVecQueue *) pParam1;
	//TMutexQueue<TDoubleVec> *qmtxRaw = (TMutexQueue<TDoubleVec> *) pParam1;
	static bool fTimerBusy = false;
    TDoubleVec vPulse;
	TRedPitayaSetup *pSetup = (TRedPitayaSetup *) pParam2;

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
		usleep (10);
	}
}

//-----------------------------------------------------------------------------
//void ProcessThread (mutex *pMutex)
//void ProcessThread (TMutexQueue<TDoubleVecQueue> *qmtxRaw)
void ProcessThread (void *pParam1, void *pParam2, void *pParam3)
{
    TDoubleVec vPulse;
	TPulseFilter pulse_filter;
    TPulseInfoVec piVec;
	mutex mtx;
	//TMutexQueue<TDoubleVec> q (&mtx);
	//TMutexQueue<TDoubleVec> *pqmtxRaw = (TMutexQueue<TDoubleVec> *) pParam1;
	TMutexDoubleVecQueue *pqmtxRaw = (TMutexDoubleVecQueue *) pParam1;
	//TMutexQueue<TPulseFilter> *pqProcess = (TMutexQueue<TPulseFilter> *) pParam2;
	TMutexPulseFilterQueue *pqProcess = (TMutexPulseFilterQueue *) pParam2;
	//TMutexQueue<TPulseFilter> *pqProcess = (TMutexQueue<TPulseFilter> *) pParam2;
	TRedPitayaSetup *pSetup = (TRedPitayaSetup *) pParam3;

	while (1) {
    	if (pqmtxRaw->GetSize() > 0) {
			pqmtxRaw->PopLastItem (vPulse);
    		if (vPulse.size() > 0) {
			//if (GetNextQueuePulse (vPulse) > 0) {
				try {
					//FilterPulse (vPulse, g_rp_setup.GetBackground(), pulse_filter, false);
					FilterPulse (vPulse, pSetup->GetBackground(), pulse_filter, pSetup->IsFilterOn(), pSetup);
					//FilterPulse (vPulse, g_rp_setup.GetBackground(), pulse_filter, g_rp_setup.IsFilterOn());
					if (GetPulseParams (pulse_filter, piVec))
						pulse_filter.SetPulsesInfo (piVec);
					pqProcess->AddItem (pulse_filter);
					//AddFiteredPulseToQueue (pulse_filter);
					if (piVec.size() > 0)
            			pSetup->NewPulse (piVec);
            			//g_rp_setup.NewPulse (piVec);
				}
				catch (std::exception &exp) {
					fprintf (stderr, "Runtime Error in ProcessThread:\n%s\n", exp.what());
				}
			}
		}
		usleep(10);
	}
}

/*
//-----------------------------------------------------------------------------
size_t VectorQueueSize(mutex *pMutex)
{
    //mutex mtx;
	size_t n;

	while (pMutex->try_lock())
		usleep(1);
	pMutex->lock();
    //mtx.lock ();
	n = g_qPulses.size();
	//mtx.unlock();
	pMutex->unlock();
	return (n);
}
*/

/*
//-----------------------------------------------------------------------------
size_t GetNextQueuePulse (TDoubleVec &vPulse, mutex *pMutex)
{

	vPulse.clear();
	while (pMutex->try_lock())
		usleep(1);
	pMutex->lock();
    //mtx.lock ();
	if (g_qPulses.size () > 0) {
		vPulse = g_qPulses.back ();
		g_qPulses.pop();
	}
	//mtx.unlock();
	pMutex->unlock();
	return (vPulse.size());
}
*/

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

/*
//-----------------------------------------------------------------------------
void VectorQueueClear (mutex *mtxPulsesQueue)
{
    //mutex mtx;
	while (mtxPulsesQueue->try_lock())
		usleep(1);
	mtxPulsesQueue->lock();
    //mtx.lock ();
	while (g_qPulses.size() > 0)
		g_qPulses.pop();
	//mtx.unlock();
	mtxPulsesQueue->unlock();
}
*/

//-----------------------------------------------------------------------------
Json::Value HandleSampling(Json::Value &jSampling, TRedPitayaSetup &rp_setup, bool &fRun, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess)
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
			if (!jSampling["mca_time"].isNull()) {
				string sTime = jSampling["mca_time"].asString();
				double d = atof (sTime.c_str());
				printf ("\nMCA Required Time: %s, %g seconds\n\n", sTime.c_str(), d);
			}
			string strMca = jSampling["mca_time"].asString();
			printf ("MCA Time: %s\n", strMca.c_str());
			rp_setup.SetMcaOnOff (ToLower (jSampling["mca"].asString()), jSampling["mca_time"].asString());
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
		jStatus["mca"] = rp_setup.GetMcaOnOff ();
		jStatus["psd"] = rp_setup.GetPsdOnOff ();
		jResult["status"] = jStatus;
		//jResult["buffer"] = to_string(SafeQueueSize());//g_vRawSignal.size());
		jResult["raw_buffer"] = to_string(qRaw.GetSize());//SafeQueueSize(mtxPulsesQueue));//VectorQueueSize());
		jResult["process_buffer"] = to_string(qProcess.GetSize());//SafeQueueSize(mtxPulsesQueue));//VectorQueueSize());
		jResult["mca_buffer"] = to_string(rp_setup.GetMcaCount());//g_vRawSignal.size());
		jResult["mca_time"] = rp_setup.GetMcaMeasureTime ();
		//if (nCount % 10 == 0)
			//fprintf (stderr, "HandleSampling, Sampling: %s, MCA: %s, Queue size: %d\r", BoolToString(rp_setup.GetSamplingOnOff ()).c_str(), BoolToString  (rp_setup.GetMcaOnOff()).c_str(), SafeQueueSize());
		nCount++;
    }
    catch (std::exception &exp) {
		fprintf (stderr, "Runtime error in 'HandleSampling':\n%s\n", exp.what());
        jResult[g_szSampling] = exp.what();
    }
    return (jResult);
}

/*
//-----------------------------------------------------------------------------
void SafeQueueClear (mutex *mtxPulsesQueue)
{
    //mutex mtx;

	while (mtxPulsesQueue->try_lock())
		usleep(1);
    //mtx.lock ();mutex 
	mtxPulsesQueue->lock();
	while (!g_qFilteredPulses.empty())
    //while (!g_qPulses.empty())
		//g_qPulses.pop();
		g_qFilteredPulses.pop_back ();
	mtxPulsesQueue->unlock();
    //mtx.unlock ();
}
*/

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
/*
//-----------------------------------------------------------------------------
size_t SafeQueueSize (mutex *mtxPulsesQueue)
{
    size_t s;
    //mutex mtx;

    //mtx.lock ();
	while (mtxPulsesQueue->try_lock())
		usleep(1);
	//WaitForMutex (g_mtxPulsesQueue, 1);
	mtxPulsesQueue->lock();
	s = g_qFilteredPulses.size();
    //s = g_qPulses.size();
	//s = g_vPulsesInfo.size();
    //mtx.unlock ();
	mtxPulsesQueue->unlock();
    return (s);
}
*/
