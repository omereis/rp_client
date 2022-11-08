/******************************************************************************\
|                                   misc.h                                     |
\******************************************************************************/
//---------------------------------------------------------------------------

#ifndef miscH
#define miscH
//---------------------------------------------------------------------------
#include <string>
#include "bd_types.h"
#include "jsoncpp/json/json.h"
//---------------------------------------------------------------------------
#ifdef	_RED_PITAYA_HW
#include "rp.h"
#endif
//#include "rp_setup.h"
enum EPulseDir {EPS_UP, EPS_DOWN, EPS_Undecided};
//---------------------------------------------------------------------------
std::string StringifyJson (const Json::Value &val);
std::string read_file_as_line(const std::string &strFile);
std::string ReadFileAsString (const std::string &strFile);
std::string ToLower (const std::string &str);
bool ReadVectorFromFile (const std::string &strFile, TFloatVec &vDate);
std::string ReplaceAll(const std::string &strSrc, const std::string& from, const std::string& to);
float VectorAverage (const TFloatVec &vec);
void PrintRuntimeError (const char szMessage[], FILE *file=stderr);
void PrintRuntimeError (std::exception &exp, const char szChar[], FILE *file=stderr);
void PrintRuntimeError (std::exception &exp, const std::string &strProcedure, FILE *file=stderr);
void PrintBool (const char *szTitle, bool fValue);
std::string BoolToString (bool f);
//float VectorAverage (const TFloatVec &vPulse);
std::string PulseDirName (EPulseDir pulse_dir);
EPulseDir PulseDirFromString (const std::string &strSource);
#ifdef	_RED_PITAYA_HW
std::string GetHardwareTriggerName (rp_acq_trig_src_t trigger_src);
//std::string GetHardwareTriggerName ();
//std::string GetHardwareTriggerName (rp_acq_trig_src_t trigger_src);
void PrintTriggerSource (const char sz[]);
rp_acq_decimation_t GetHardwareDecimationFromName(const std::string &strDecimationSrc);
//bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TFloatVec &vPulse);
#endif
#endif
