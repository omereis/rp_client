/******************************************************************************\
|                            RedPitayaSampling.h                               |
\******************************************************************************/
#ifndef  __RP_SAMPLING_H
#define	__RP_SAMPLING_H
//-----------------------------------------------------------------------------

#ifdef	_RED_PITAYA_HW
#include "rp.h"
#endif

#include <string>
using namespace std;
#include "jsoncpp/json/json.h"
#include "RedPitayaTrigger.h"

#include "mca_params.h"

class TRedPitayaSampling {
public:
    TRedPitayaSampling ();
    TRedPitayaSampling (const TRedPitayaSampling &other);

    TRedPitayaSampling operator= (const TRedPitayaSampling &other);
    bool operator== (const TRedPitayaSampling &other) const;
    bool operator!= (const TRedPitayaSampling &other) const;
    void Clear ();

    Json::Value LoadFromJson (Json::Value jSampling);
    Json::Value AsJson();
    Json::Value UpdateFromJson(Json::Value &jSetup);

#ifdef	_RED_PITAYA_HW
	rp_acq_decimation_t GetHardwareDecimation() const;
	rp_acq_sampling_rate_t GetHardwareSamplingRate() const;
#endif

    string GetRate () const;
    void SetRate (const string &strRate);
    string GetDecimation () const;
    void  SetDecimation (const string &str);
    int GetBufferSize () const;
    void SetBufferSize (int nSize);
    void SetBufferSize (const std::string &strSize);
    int GetSignalPoints () const;
    void SetSignalPoints (int nPoints);
    void SetSignalPoints (const std::string &strPoints);
	EPulseDir GetPulseDir () const;
	void SetPulserDir (EPulseDir pulse_dir);
	void  SetPulseDir (const string &strDir);
	void  SetPulseDir (Json::Value jDir);
protected:
    void AssignAll (const TRedPitayaSampling &other);
private:
    string m_strRate;
    string m_strDecimation;
    int m_nBufferSize;
    int m_nSignalPoints;
	EPulseDir m_pulse_dir;
};
//-----------------------------------------------------------------------------
#endif
