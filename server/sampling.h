/******************************************************************************\
|                                sampling.h                                    |
\******************************************************************************/
#ifndef  __SAMPLING_H_INC
#define   __SAMPLING_H_INC

#include <mutex>
#include <thread>

#include "jsoncpp/json/json.h"
#include "bd_types.h"
#include "rp_setup.h"
#include "MutexQueue.h"
#include "PulseFilterInfo.h"

//-----------------------------------------------------------------------------
#ifndef	TMutexDoubleVecQueue
typedef	TMutexQueue<TDoubleVec> TMutexDoubleVecQueue;
#endif
//-----------------------------------------------------------------------------
#ifndef	TMutexPulseFilterQueue
typedef TMutexQueue<TPulseFilter> TMutexPulseFilterQueue;
#endif

using namespace std;

void SamplingThread (void *pParam1, void *pParam2);
void ProcessThread (void *pParam1, void *pParam2, void *pParam3);
//void ProcessThread (TMutexQueue<TDoubleVecQueue> *qmtxRaw, TMutexQueue<TDoubleVecQueue> *qmtxProcess);
//size_t VectorQueueSize(mutex *pMutex);
//size_t GetNextQueuePulse (TDoubleVec &vPulse, mutex *pMutex);
//void VectorQueueClear(mutex *mtxPulsesQueue);
//Json::Value HandleSampling(Json::Value &jSampling, TRedPitayaSetup &rp_setup, bool &fRun, mutex *mtxPulsesQueue);
Json::Value HandleSampling(Json::Value &jSampling, TRedPitayaSetup &rp_setup, bool &fRun, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess);
//void SafeQueueClear (mutex *mtxPulsesQueue);
size_t SafeQueueSize (mutex *mtxPulsesQueue);
bool GetNextPulse (TDoubleVec &vPulse, TRedPitayaSetup &rp_setup);

#endif
