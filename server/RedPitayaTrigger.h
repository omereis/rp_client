/******************************************************************************\
|                             RedPitayaTrigger.h                               |
\******************************************************************************/
#ifndef  __RP_TRIGGER_H
#define  __RP_TRIGGER_H
//-----------------------------------------------------------------------------

//#define  __RP_SETUP_H
#ifdef	_RED_PITAYA_HW
#include "rp.h"
#endif

#include "jsoncpp/json/json.h"

#include <string>
#include "mca_params.h"
using namespace std;

//-----------------------------------------------------------------------------

class TRedPitayaTrigger {
public:
    TRedPitayaTrigger ();
    TRedPitayaTrigger (const TRedPitayaTrigger &other);

	void Print (const char sz[]=NULL, FILE *file=stderr);
#ifdef	_RED_PITAYA_HW
	bool LoadFromHardware ();
	void SetDir (rp_acq_trig_src_t dir);
	void SetSrc (rp_acq_trig_src_t src);
	bool GetTriggerSource (const std::string &strSrcSrc, const std::string &strDirSrc, rp_acq_trig_src_t &trigger_src);

	void SetTriggerFromHardware ();
	void SetTriggerFromHardware (rp_acq_trig_src_t trigger_src);

	//bool SetHardwareTrigger(const TRedPitayaTrigger &trigger);
	bool SetHardwareTrigger();
	rp_channel_trigger_t GetChannel (const std::string &strDirSrc);
	void PrintHardwareSetup (FILE *file=stderr);
	std::string GetHardwareTriggerSource ();
	std::string GetHardwareTriggerSource (rp_acq_trig_src_t trigger_src);
	float GetHardwareTriggerLevel () const;
	rp_channel_trigger_t GetHardwareTriggerChannel () const;
	rp_acq_trig_src_t GetRequiredTriggerSource ();
	rp_channel_trigger_t GetRequiredTriggerChannel ();
#endif

	void SetTrigger (Json::Value &jTrigger);
	void SetTrigger (const std::string &strSrcSrc, const std::string &strDirSrc);

	void TriggerNow ();

    TRedPitayaTrigger operator= (const TRedPitayaTrigger &other);
    bool operator== (const TRedPitayaTrigger &other) const;
    bool operator!= (const TRedPitayaTrigger &other) const;
    void Clear ();

    Json::Value LoadFromJson (Json::Value &jTrigger);
	Json::Value AsJson();
    Json::Value UpdateFromJson(Json::Value &jSetup);

    string GetLevel () const;
	void SetLevel (float fLevel);
    void SetLevel (const string &strLevel);
    string GetDir () const;
    void SetDir (const string &strDir);
    string GetSrc () const;
    void SetSrc (const string &strSrc);
	string GetType() const;
	void SetType (const string &strType);
	bool GetTrigger() const;
	void SetTrigger (bool fTrigger);
	bool GetNow () const;
	void SetNow (bool fNow);
protected:
    void AssignAll (const TRedPitayaTrigger &other);
private:
    string m_strLevel;
    string m_strDir;
    string m_strSrc;
	string m_strType;
	bool m_fTriggerOn;
	bool m_fNow;
};
#endif
