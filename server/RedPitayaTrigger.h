/******************************************************************************\
|                             RedPitayaTrigger.h                               |
\******************************************************************************/
#ifndef  __RP_TRIGGER_H
#define  __RP_TRIGGER_H
//-----------------------------------------------------------------------------

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

#ifdef	_RED_PITAYA_HW
	bool LoadFromHardware ();
#endif
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
#ifdef	_RED_PITAYA_HW
	void SetDir (rp_acq_trig_src_t dir);
	void SetSrc (rp_acq_trig_src_t src);
	bool GetTriggerSource (const std::string &strSrcSrc, const std::string &strDirSrc, rp_acq_trig_src_t &trigger_src);
	void SetTrigger (const std::string &strSrcSrc, const std::string &strDirSrc);
#endif
    string GetSrc () const;
    void SetSrc (const string &strSrc);
protected:
    void AssignAll (const TRedPitayaTrigger &other);
private:
    string m_strLevel;
    string m_strDir;
    string m_strSrc;

};
#define  __RP_SETUP_H
#endif
