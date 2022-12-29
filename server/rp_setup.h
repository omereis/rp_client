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
#include "pulse_info.h"
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

	void TriggerNow ();
	
	static std::string PreTriggerAsString (double dPreTrigger);

	void GetMcaSpectrum (TFloatVec &vSpectrum);
	void ResetMcaSpectrum ();

	Json::Value HandleBackground (Json::Value &jBkgnd);
	void NewPulse (const TPulseInfoVec &vPulsesInfo);
	void NewPulse (const TPulseInfo &pi);
	//void NewPulse (const TFloatVec &vPulse);
	int GetMcaPulses() const;
#ifdef	_RED_PITAYA_HW
	bool LoadFromHardware (bool fInit=true);
	bool SetHardwareTrigger();
	bool SetHardwareTrigger(const TRedPitayaTrigger &trigger);
	bool PrintHardwareSetup (FILE *file=stderr);
	rp_acq_decimation_t GetHardwareDecimation() const;
	rp_channel_trigger_t GetHardwareTriggerChannel() const;
	float GetHardwareTriggerLevel() const;
	rp_acq_sampling_rate_t GetHardwareSamplingRate() const;
	rp_acq_trig_src_t GetHardwareTriggerSource() const;
#endif
    bool LoadFromJson(const string &strFile);
    bool SaveToJson (const std::string &strFile);
    Json::Value AsJson();
	Json::Value TriggerAsJson();
    Json::Value UpdateFromJson(Json::Value &jSetup, bool fUpdateHardware=false);
	Json::Value McaAsJson();

    TRedPitayaTrigger GetTrigger() const;
    void SetTrigger (const TRedPitayaTrigger &trigger);
    TRedPitayaSampling GetSampling () const;
    void SetSampling (const TRedPitayaSampling &sampling);

    TMcaParams GetMcaParams () const;
    void SetMcaParams (const TMcaParams &mca_params);
    Json::Value AppsAsJson();

    float GetTriggerLevel ();
    void SetSamplingOnOff (bool fSampling);
    bool GetSamplingOnOff () const;
    void SetMcaOnOff (bool fMca);
    bool GetMcaOnOff () const;
    void SetPsdOnOff (bool fPsd);
    bool GetPsdOnOff () const;

    double GetBackground() const;
	void SetBackground (const string &strBackground);
    void SetBackground (double dBackground);
    void SetBackgroundFromJson (Json::Value jBkgnd);
    int GetPackageSize() const;
    void SetPackageSize (int nPackageSize);
    float GetPreTriggerNs() const;
    void SetPreTriggerNs (float fDelayNs);
    void SetPreTriggerNs (const std::string &str);
    void SetPreTriggerNs (Json::Value jPreTrigger);
protected:
	void AssignAll (const TRedPitayaSetup &other);
private:
/*
*/
    TRedPitayaTrigger m_trigger;
    TRedPitayaSampling m_sampling;
    TMcaParams m_mca_params;
    double m_dBackground;
    bool m_fSamplingOnOff;
    bool m_fMcaOnOff;
    bool m_fPsdOnOff;
    int m_nPackageSize;
    float m_fPreTriggerNs;
};
//-----------------------------------------------------------------------------
#endif
