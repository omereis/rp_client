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
#include "mca_info.h"

//-----------------------------------------------------------------------------
#ifndef	TMutexDoubleVecQueue
typedef	TMutexQueue<TDoubleVec> TMutexDoubleVecQueue;
#endif
//-----------------------------------------------------------------------------
#ifndef	TMutexPulseFilterQueue
typedef TMutexQueue<TPulseFilter> TMutexPulseFilterQueue;
#endif

using namespace std;

void SamplingThread (TMutexDoubleVecQueue *qmtxRaw, TRedPitayaSetup *pSetup);
void ProcessThread (TMutexDoubleVecQueue *pqmtxRaw, TMutexPulseFilterQueue *pqProcess, TMutexMcaInfo *pMutexMcaInfo, TRedPitayaSetup *pSetup);
//void ProcessThread (TMutexDoubleVecQueue *pqmtxRaw, TMutexPulseFilterQueue *pqProcess, TMcaParams *pMcaParams, TRedPitayaSetup *pSetup);
Json::Value HandleSampling(Json::Value &jSampling, TRedPitayaSetup &rp_setup, bool &fRun, TMutexMcaInfo *pMutexMca, TMutexDoubleVecQueue &qRaw, TMutexPulseFilterQueue &qProcess);
size_t SafeQueueSize (mutex *mtxPulsesQueue);
bool GetNextPulse (TDoubleVec &vPulse, TRedPitayaSetup &rp_setup);

#endif
