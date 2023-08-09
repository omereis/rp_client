/******************************************************************************\
|                                mca_params.h                                  |
\******************************************************************************/
//-----------------------------------------------------------------------------
#ifndef  __MCA_PARAMS_H
#define  __MCA_PARAMS_H
//-----------------------------------------------------------------------------
#include "jsoncpp/json/json.h"
#include "bd_types.h"
#include "misc.h"
#include "pulse_info.h"
#include <mutex>
//-----------------------------------------------------------------------------
class TMcaParams {
public:
    TMcaParams (mutex *mtx);
    TMcaParams (const TMcaParams &other);
    TMcaParams operator= (const TMcaParams &other);
    bool operator== (const TMcaParams &other) const;
    bool operator!= (const TMcaParams &other) const;
    void Clear (mutex *mtx);
	void ClearParams ();

	void ResetSpectrum ();
	int HeightIndex (float fSignalMin, float fSignalMax);
	//void SetSpectrum (uint uiChannels);
	//void NewPulse (const TFloatVec &vPulse);
	//void NewPulse (const TPulseInfo &pulse_info);
	//void NewPulse (const TPulseInfoVec &vPulsesInfo);
	//int GetMcaPulses() const;
//-----------------------------------------------------------------------------
    //Json::Value LoadFromJson (Json::Value jMCA);
	Json::Value UpdateFromJson (Json::Value jMCA);
    Json::Value AsJson () const;
//-----------------------------------------------------------------------------
    uint GetChannels () const;
    void SetChannels (uint uiChannels);

    double GetMinVoltage () const;
    void SetMinVoltage (double dMinVoltage);

    double GetMaxVoltage () const;
    void SetMaxVoltage (double dMaxVoltage);
	//void SetSpectrum (const TFloatVec &vSpectrum);
	//size_t GetSpectrum (TFloatVec &vSpectrum);

	size_t GetCount() const;
	void SetCount (size_t n);

	//void ClearMca ();

    void SetMax (double dMax);
    double GetMax() const;
    void SetMin (double dMin);
    double GetMin() const;
    //void IncreaseCount ();

	//void SetMcaOnOff (Json::Value &jMcaCmd);
protected:
    void AssignAll (const TMcaParams &other);
private:
    uint m_uiChannels;
    double m_dMinVoltage;
    double m_dMaxVoltage;
/*
    double m_dMin;
    double m_dMax;

	TFloatVec m_vSpectrum;
	TPulseInfoVec m_vPulses;
	size_t m_nCount;
	bool m_fMcaOnOff;

	chrono_clock m_crnMcaStart;
	chrono_clock m_crnMcaStop;
*/
	double m_dMcaTimeLimit;
	bool m_fMcaValid;
	mutex *m_pMutex;
};
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
