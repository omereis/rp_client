/******************************************************************************\
|                                mca_info.h                                    |
\******************************************************************************/

#ifndef _MCA_INFO_H
#define _MCA_INFO_H
//-----------------------------------------------------------------------------
#include "rp_setup.h"
#include "bd_types.h"
#include "pulse_info.h"
#include <mutex>
//-----------------------------------------------------------------------------
class TMcaInfo {
public:
    TMcaInfo (TMcaParams *pParams=NULL);
    TMcaInfo (const TMcaInfo &other);
    void Clear (TMcaParams *pParams=NULL);
    void AssignAll (const TMcaInfo &other);

    void NewPulses (TPulseInfoVec &piVec);
    void NewPulse (TPulseInfo &pi);

    void HandleMcaCommands (Json::Value &jSampling);
	Json::Value ReadMca ();
	Json::Value HandleMCA (Json::Value &jMCA);
	Json::Value UpdateFromJson (Json::Value jMCA);
	Json::Value AsJson();

	void SetSpectrum (uint uiChannels);
	void ResetSpectrum();
	void  IncreaseCount ();
	size_t GetCount() const;
	void SetCount (size_t nCount);

    void SetStartTime (chrono_clock clkStart);
    void SetStartTime ();
    chrono_clock GetStartTime () const;
    //chrono_clock SetMcaStopTime (chrono_clock clk);
    //chrono_clock SetMcaStopTime ();
	chrono_clock SetStopTime ();
	chrono_clock SetStopTime (chrono_clock clk);
	chrono_clock GetStopTime() const;

	void SetMcaOnOff (const string &strOnOff, const string &strTime);
	void SetMcaOnOff (const string &strOnOff);
    void SetMcaOnOff (bool fOnOff);
    bool GetMcaOnOff () const;

    void SetMin (double dMin);
    void SetMax (double dMax);
    double GetMin () const;
    double GetMax () const;

	void SetMcaTimeLimit (const string &strTimeLimit);
	void SetMcaTimeLimit (double dMcaSeconds);
	double GetMcaTimeLimit () const;
	double GetMcaMeasureTime () const;
	void SetValid (bool fValid);
	bool GetValid () const;
	void StopMca();
//-----------------------------------------------------------------------------
protected:
//-----------------------------------------------------------------------------
private:
	chrono_clock m_crnMcaStart;
	chrono_clock m_crnMcaStop;
	//double m_dMcaTimeLimit;
	bool m_fMcaValid;
    bool m_fMcaOnOff;
	TDoubleVec m_vSpectrum;
	TPulseInfoVec m_vPulses;
	size_t m_nCount;
    TMcaParams *m_pParams;
    double m_dMin;
    double m_dMax;
	double m_dMcaTimeLimit;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class TMutexMcaInfo {
public:
    TMutexMcaInfo (TMcaInfo *pMcaInfo, mutex *pMutex);

    void NewPulses (TPulseInfoVec &piVec);
    void HandleMcaCommands (Json::Value &jSampling);
	void ResetSpectrum();
	Json::Value ReadMca ();
	void SetMcaOnOff (const string &strOnOff, const string &strTime);
	bool GetMcaOnOff ();
	size_t GetCount() const;
	double GetMcaMeasureTime () const;
	Json::Value HandleMCA (Json::Value &jMCA);
	Json::Value UpdateFromJson (Json::Value jMCA);
	Json::Value AsJson();
	double GetMcaTimeLimit () const;
	void StopMca();
protected:
private:
    TMcaInfo *m_pMcaInfo;
    mutex *m_pMutex;
};
#endif
