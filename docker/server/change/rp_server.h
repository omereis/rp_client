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
//---------------------------------------------------------------------------
#ifdef	_RED_PITAYA_HW
#include "rp.h"
bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TFloatVec &vPulse);
#endif
#endif
