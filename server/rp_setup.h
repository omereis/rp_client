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
#include <iostream>
#include <chrono>
#include <ctime>

#include "mca_params.h"
#include "pulse_info.h"
using namespace std;

#include "TrpzInfo.h"
#include "remote_proc.h"
//-----------------------------------------------------------------------------

class TRedPitayaSetup {
public:
    TRedPitayaSetup (TMcaParams *pMcaParams=NULL);
    TRedPitayaSetup (const TRedPitayaSetup &other);

    TRedPitayaSetup operator= (const TRedPitayaSetup &other);
    bool operator== (const TRedPitayaSetup &other) const;
    bool operator!= (const TRedPitayaSetup &other) const;
    void Clear (TMcaParams *pMcaParams);

	void TriggerNow ();
	
	static std::string PreTriggerAsString (double dPreTrigger);

	void GetMcaSpectrum (TFloatVec &vSpectrum);
	void ResetMcaSpectrum ();

	Json::Value HandleBackground (Json::Value &jBkgnd, const TDoubleVec &vSignal);
	//Json::Value HandleBackground (Json::Value &jBkgnd, const TFloatVec &vSignal);
	//void NewPulse (const TPulseInfoVec &vPulsesInfo);
	//void NewPulse (const TPulseInfo &pi);
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
	double GetSamplingPeriod() const;
    bool LoadFromJson(const string &strFile);
    bool IsFilterOn() const;
	TDoubleVec GetTrapez() const;
	size_t GetTrapezSize() const;
	Json::Value GetTrapezAsJson();
    bool SaveToJson (const std::string &strFile);
    Json::Value AsJson();
	Json::Value TriggerAsJson();
    Json::Value UpdateFromJson(Json::Value &jSetup, bool fUpdateHardware=false);
	Json::Value McaAsJson();

    TRedPitayaTrigger GetTrigger() const;
    void SetTrigger (const TRedPitayaTrigger &trigger);
    TRedPitayaSampling GetSampling () const;
    void SetSampling (const TRedPitayaSampling &sampling);

	double CalculateBackground (const TDoubleVec &vPulse);
	//double CalculateBackground (const TFloatVec &vPulse);

	TFloatVec::const_iterator FindFirstTrigger(const TFloatVec &vSignal);
	double GetSignalBackground(const TFloatVec &vSignal);

    TMcaParams GetMcaParams () const;
    void SetMcaParams (const TMcaParams &mca_params);
    Json::Value AppsAsJson();

    float GetTriggerLevel ();
    void SetSamplingOnOff (bool fSampling);
    bool GetSamplingOnOff () const;
	//void SetMcaOnOff (const string &strOnOff);
	//void SetMcaOnOff (const string &strOnOff, const string &strTime);
	//void SetMcaOnOff (Json::Value &jMcaCmd);
    //void SetMcaOnOff (bool fMca);
    //bool GetMcaOnOff () const;
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

	//void SetMcaTimeLimit (double dMinutes);
	//void SetMcaTimeLimit (const string &strTime);
	//double GetMcaTimeLimit () const;
	//double GetMcaMeasureTime () const;

	chrono_clock SetMcaStopTime (chrono_clock clk);
	chrono_clock GetMcaStopTime() const;
	chrono_clock SetMcaStopTime ();
	chrono_clock GetMcaStartTime() const;
	chrono_clock SetMcaStartTime ();
	void SetMcaStartTime (const chrono_clock &crnStart);
	void ResetMcaStartTime ();

	void SetMcaValid (bool fValid);
	bool IsMcaValid () const;

	TRemoteProcessing GetRemoteProc() const;
	void GetRemoteProc(const TRemoteProcessing &remote_proc);
    bool IsRemoteProcessingOn() const;
	size_t GetMcaCount() const;
	void ClearMca();
	double GetMcaMin() const;
	double GetMcaMax() const;
	double GetMcaMinVoltage() const;
	double GetMcaMaxVoltage() const;
	double GetFilteredThreshold () const;
	void SetFilteredThreshold (double dFiltThreshold);
	double SetFilteredThreshold (Json::Value &jFiltTh);
protected:
	void AssignAll (const TRedPitayaSetup &other);
private:
/*
*/
    TTRapezInfo m_trapez;
    TRedPitayaTrigger m_trigger;
    TRedPitayaSampling m_sampling;
    TMcaParams *m_pMcaParams;
    double m_dBackground;
    bool m_fSamplingOnOff;
    bool m_fMcaOnOff;
    bool m_fPsdOnOff;
    int m_nPackageSize;
    float m_fPreTriggerNs;

	chrono_clock m_crnMcaStart;
	chrono_clock m_crnMcaStop;
	//double m_dMcaTimeLimit;
	bool m_fMcaValid;

	double m_dFiltThreshold;
	TRemoteProcessing m_remote_proc;
};
//-----------------------------------------------------------------------------
#endif
