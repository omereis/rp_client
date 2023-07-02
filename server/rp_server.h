/******************************************************************************\
|                                rp_server.h                                   |
\******************************************************************************/
//---------------------------------------------------------------------------

#ifndef __RP_SERVER_H
#define __RP_SERVER_H
//---------------------------------------------------------------------------
#include <string>
#include "bd_types.h"
#include "jsoncpp/json/json.h"
#include "PulseFilterInfo.h"
#include "remote_proc.h"
//---------------------------------------------------------------------------
#ifdef	_RED_PITAYA_HW
#include "rp.h"
bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TFloatVec &vPulse);
#endif
//bool GetNextPulse (TFloatVec &vPulse);
//bool GetNextPulse (TDoubleVec &vPulse);
void RemoteProcess (const TRemoteProcessing &remote_processing, TPulseFilter &pulse_filter);
void RemoteProcess (const TRemoteProcessing &remote_processing, const TDoubleVec &vPulse);
//bool GetPulseParams (const TPulseFilter &pulse_filter, TPulseInfoVec &piVec);
bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TDoubleVec &vPulse);
bool GetPulseParams (const TRedPitayaSetup &rp_setup, const TPulseFilter &pulse_filter, TPulseInfoVec &piVec);
//---------------------------------------------------------------------------
#endif
