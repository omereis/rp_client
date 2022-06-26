/******************************************************************************\
|                                 rp_setup.h                                   |
\******************************************************************************/
#ifndef  __RP_SETUP_H
#define	__RP_SETUP_H
//-----------------------------------------------------------------------------

#include "jsoncpp/json/json.h"
#include "RedPitayaTrigger.h"
#include "RedPitayaSampling.h"

#include <string>
#include "mca_params.h"
using namespace std;

//-----------------------------------------------------------------------------

class TRedPitayaSetup {
public:
    TRedPitayaSetup ();
    TRedPitayaSetup (const TRedPitayaSetup &other);

    TRedPitayaSetup operator= (const TRedPitayaSetup &other);
    bool operator== (const TRedPitayaSetup &other) const;
    bool operator!= (const TRedPitayaSetup &other) const;
    void Clear ();

    bool LoadFromJson(const string &strFile);
    bool SaveToJson (const std::string &strFile);
    Json::Value AsJson();
    Json::Value UpdateFromJson(Json::Value &jSetup);

    TRedPitayaTrigger GetTrigger() const;
    void SetTrigger (const TRedPitayaTrigger &trigger);
    TRedPitayaSampling GetSampling () const;
    void SetSampling (const TRedPitayaSampling &sampling);

    TMcaParams GetMcaParams () const;
    void SetMcaParams (const TMcaParams &mca_params);
    Json::Value McaAsJson();
    Json::Value AppsAsJson();

    float GetTriggerLevel ();
    void SetSamplingOnOff (bool fSampling);
    bool GetSamplingOnOff () const;
    void SetMcaOnOff (bool fMca);
    bool GetMcaOnOff () const;
    void SetPsdOnOff (bool fPsd);
    bool GetPsdOnOff () const;

    double GetBackground() const;
    void SetBackground (double dBackground);
    void SetBackgroundFromJson (Json::Value jBkgnd);
protected:
	void AssignAll (const TRedPitayaSetup &other);
private:
    TRedPitayaTrigger m_trigger;
    TRedPitayaSampling m_sampling;
    TMcaParams m_mca_params;
    double m_dBackground;
    bool m_fSamplingOnOff;
    bool m_fMcaOnOff;
    bool m_fPsdOnOff;
};
//-----------------------------------------------------------------------------
#endif
