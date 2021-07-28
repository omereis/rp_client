/******************************************************************************\
|                                 rp_setup.h                                   |
\******************************************************************************/
#ifndef  __RP_SETUP_H
//-----------------------------------------------------------------------------

#include "jsoncpp/json/json.h"

#include <string>
#include "mca_params.h"
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
    void UpdateFromJson(Json::Value &jSetup);

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
    void UpdateFromJson(Json::Value &jSetup);

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
    bool SaveToJson (const std::string &strFile);
    Json::Value AsJson();
    bool  UpdateFromJson(Json::Value &jSetup);

    TRedPitayaTrigger GetTrigger() const;
    void SetTrigger (const TRedPitayaTrigger &trigger);
    TRedPitayaSampling GetSampling () const;
    void SetSampling (const TRedPitayaSampling &sampling);

    TMcaParams GetMcaParams () const;
    void SetMcaParams (const TMcaParams &mca_params);
protected:
	void AssignAll (const TRedPitayaSetup &other);
private:
    TRedPitayaTrigger m_trigger;
    TRedPitayaSampling m_sampling;
    TMcaParams m_mca_params;
};
//-----------------------------------------------------------------------------
#define  __RP_SETUP_H
#endif
