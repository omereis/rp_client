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
	rp_acq_decimation_t GetHardwareDecimation();
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
protected:
    void AssignAll (const TRedPitayaSampling &other);
private:
    string m_strRate;
    string m_strDecimation;
    int m_nBufferSize;
    int m_nSignalPoints;
};
//-----------------------------------------------------------------------------
#endif
