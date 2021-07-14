/******************************************************************************\
|                                 rp_setup.h                                   |
\******************************************************************************/
#ifndef  __RP_SETUP_H
//-----------------------------------------------------------------------------

#include "jsoncpp/json/json.h"

#include <string>

using namespace std;

class TRedPitayaSampling {
public:
    TRedPitayaSampling ();
    TRedPitayaSampling (const TRedPitayaSampling &other);

    TRedPitayaSampling operator= (const TRedPitayaSampling &other);
    bool operator== (const TRedPitayaSampling &other) const;
    bool operator!= (const TRedPitayaSampling &other) const;
    void Clear ();

    bool LoadFromJson (Json::Value jSampling);
    Json::Value AsJson();

    string GetRate () const;
    void SetRate (const string &strRate);
    string GetDecimation () const;
    void  SetDecimation (const string &str);
protected:
    void AssignAll (const TRedPitayaSampling &other);
private:
    string m_strRate;
    string m_strDecimation;
};
//-----------------------------------------------------------------------------

class TRedPitayaTrigger {
public:
    TRedPitayaTrigger ();
    TRedPitayaTrigger (const TRedPitayaTrigger &other);

    TRedPitayaTrigger operator= (const TRedPitayaTrigger &other);
    bool operator== (const TRedPitayaTrigger &other) const;
    bool operator!= (const TRedPitayaTrigger &other) const;
    void Clear ();

    bool LoadFromJson (Json::Value jTrigger);
	Json::Value AsJson();

    string GetLevel () const;
    void SetLevel (const string &strLevel);
    string GetDir () const;
    void SetDir (const string &strDir);
    string GetSrc () const;
    void SetSrc (const string &strSrc);
protected:
    void AssignAll (const TRedPitayaTrigger &other);
private:
    string m_strLevel;
    string m_strDir;
    string m_strSrc;

};
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
    Json::Value AsJson();

    TRedPitayaTrigger GetTrigger() const;
    void SetTrigger (const TRedPitayaTrigger &trigger);
    TRedPitayaSampling GetSampling () const;
    void SetSampling (const TRedPitayaSampling &sampling);
protected:
	void AssignAll (const TRedPitayaSetup &other);
private:
    TRedPitayaTrigger m_trigger;
    TRedPitayaSampling m_sampling;
};
//-----------------------------------------------------------------------------
#define  __RP_SETUP_H
#endif
