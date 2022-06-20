/******************************************************************************\
|                                rp_setup.cpp                                  |
\******************************************************************************/

#include "rp_setup.h"
#include "bd_types.h"
#include "misc.h"

//
// TRedPitayaSampling
//-----------------------------------------------------------------------------
TRedPitayaSampling::TRedPitayaSampling ()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TRedPitayaSampling::TRedPitayaSampling (const TRedPitayaSampling &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
TRedPitayaSampling TRedPitayaSampling::operator= (const TRedPitayaSampling &other)
{
    AssignAll (other);
    return (*this);
}
//-----------------------------------------------------------------------------
bool TRedPitayaSampling::operator== (const TRedPitayaSampling &other) const
{
    if (GetRate()       != other.GetRate())
        return (false);
    if (GetDecimation() != other.GetDecimation())
        return (false);
    if (GetBufferSize() != other.GetBufferSize())
        return (false);
    if (GetSignalPoints() != other.GetSignalPoints())
        return (false);
    return (true);
}
//-----------------------------------------------------------------------------
bool TRedPitayaSampling::operator!= (const TRedPitayaSampling &other) const
{
    return (!(*this == other));
}
//-----------------------------------------------------------------------------
void TRedPitayaSampling::Clear ()
{
    SetRate ("125000000");
    SetDecimation ("1");
    SetBufferSize (1000);
    SetSignalPoints (200);
}
//-----------------------------------------------------------------------------
void TRedPitayaSampling::AssignAll (const TRedPitayaSampling &other)
{
    SetRate (other.GetRate());
    SetDecimation (other.GetDecimation());
    SetBufferSize (other.GetBufferSize());
    SetSignalPoints (other.GetSignalPoints());
}
//-----------------------------------------------------------------------------
string TRedPitayaSampling::GetRate () const
{
    return (m_strRate);
}
//-----------------------------------------------------------------------------
void TRedPitayaSampling::SetRate (const string &strRate)
{
    m_strRate = strRate;
}
//-----------------------------------------------------------------------------
string TRedPitayaSampling::GetDecimation () const
{
    return (m_strDecimation);
}
//-----------------------------------------------------------------------------
void  TRedPitayaSampling::SetDecimation (const string &str)
{
    m_strDecimation = str;
}
//-----------------------------------------------------------------------------
Json::Value TRedPitayaSampling::LoadFromJson (Json::Value jSampling)
{
    Json::Value jNew;

    try {
        Json::Value jRate = jSampling["rate"], jDecimation = jSampling["decimation"], jBufferSize = jSampling["buffer_size"];

        if (!jRate.isNull())
            SetRate (jRate.asString());
        if (!jDecimation.isNull())
            SetDecimation (jDecimation.asString());
        if (!jBufferSize.isNull())
            SetBufferSize (jBufferSize.asString());
        jNew = AsJson();
    }
    catch (std::exception &exp) {
        jNew["error"] = exp.what();
    }
    return (jNew);
}
//-----------------------------------------------------------------------------

Json::Value TRedPitayaSampling::AsJson()
{
    Json::Value jSampling;

    jSampling["rate"] = GetRate();
    jSampling["decimation"] = GetDecimation();
    jSampling["buffer_length"] = std::to_string (GetBufferSize ());
    jSampling["signal_points"] = std::to_string (GetSignalPoints());
    return (jSampling);
}
//-----------------------------------------------------------------------------

Json::Value TRedPitayaSampling::UpdateFromJson(Json::Value &jSetup)
{
    Json::Value jNew;
    
    try {
        SetRate (jSetup["rate"].asString());
        SetDecimation (jSetup["decimation"].asString());
        if (!jSetup["buffer_length"].isNull())
            SetBufferSize(jSetup["buffer_length"].asString());
        if (!jSetup["signal_points"].isNull())
            SetBufferSize(jSetup["signal_points"].asString());
        jNew = AsJson();
    }
    catch (std::exception exp) {
        jNew["error"] = exp.what();
    }
    return (jNew);
}
//-----------------------------------------------------------------------------

int TRedPitayaSampling::GetBufferSize () const
{
    return (m_nBufferSize);
}
//-----------------------------------------------------------------------------
void TRedPitayaSampling::SetBufferSize (const std::string &strSize) {
    SetBufferSize(std::stoi(strSize));
}
//-----------------------------------------------------------------------------
void TRedPitayaSampling::SetBufferSize (int nSize)
{
    m_nBufferSize = nSize;
}
//-----------------------------------------------------------------------------
int TRedPitayaSampling::GetSignalPoints () const
{
    return (m_nSignalPoints);
}
//-----------------------------------------------------------------------------
void TRedPitayaSampling::SetSignalPoints (int nPoints)
{
    m_nSignalPoints = max (1, nPoints);
}
//-----------------------------------------------------------------------------
void TRedPitayaSampling::SetSignalPoints (const std::string &strPoints)
{
    int nPoints;

    try {
        nPoints = std::stoi (strPoints);
    }
    catch (...) {
        nPoints = 200;
    }
    SetSignalPoints (nPoints);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
// TRedPitayaTrigger
//-----------------------------------------------------------------------------
TRedPitayaTrigger::TRedPitayaTrigger ()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TRedPitayaTrigger::TRedPitayaTrigger (const TRedPitayaTrigger &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
TRedPitayaTrigger TRedPitayaTrigger::operator= (const TRedPitayaTrigger &other)
{
    AssignAll (other);
    return (*this);
}
//-----------------------------------------------------------------------------
bool TRedPitayaTrigger::operator== (const TRedPitayaTrigger &other) const
{
    if (GetLevel() != other.GetLevel())
        return (false);
    if (GetDir()   != other.GetDir())
        return (false);
    if (GetSrc()   != other.GetSrc())
        return (false);
    return (true);
}
//-----------------------------------------------------------------------------
bool TRedPitayaTrigger::operator!= (const TRedPitayaTrigger &other) const
{
    return (!(*this == other));
}
//-----------------------------------------------------------------------------
void TRedPitayaTrigger::Clear ()
{
    SetLevel ("0.005");
    SetDir ("up");
    SetSrc ("in1");
}
//-----------------------------------------------------------------------------
void TRedPitayaTrigger::AssignAll (const TRedPitayaTrigger &other)
{
    SetLevel (other.GetLevel());
    SetDir (other.GetDir());
    SetSrc (other.GetSrc());
}
//-----------------------------------------------------------------------------
string TRedPitayaTrigger::GetLevel () const
{
    return (m_strLevel);
}
//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetLevel (const string &strLevel)
{
    m_strLevel = strLevel;
}
//-----------------------------------------------------------------------------
string TRedPitayaTrigger::GetDir () const
{
    return (m_strDir);
}
//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetDir (const string &strDir)
{
    m_strDir = strDir;
}
//-----------------------------------------------------------------------------
string TRedPitayaTrigger::GetSrc () const
{
    return (m_strSrc);
}
//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetSrc (const string &strSrc)
{
    m_strSrc = strSrc;
}
//-----------------------------------------------------------------------------
Json::Value TRedPitayaTrigger::LoadFromJson (Json::Value &jTrigger)
{
    Json::Value jNew;

    try {
        Json::Value jLevel = jTrigger["level"], jDir = jTrigger["dir"], jSrc = jTrigger["src"];

        if (!jLevel.isNull())
            SetLevel (jLevel.asString());
        if (!jDir.isNull())
            SetDir (jDir.asString());
        if (!jDir.isNull())
            SetSrc (jSrc.asString());
		jNew = AsJson();
    }
    catch (std::exception &exp) {
		jNew["error"] = exp.what();
    }
    return (jNew);
}
//-----------------------------------------------------------------------------
Json::Value TRedPitayaTrigger::AsJson()
{
    Json::Value jTrigger;

    jTrigger["level"] = GetLevel();
    jTrigger["dir"] = GetDir();
    jTrigger["src"] = GetSrc();
    return(jTrigger);
}
//-----------------------------------------------------------------------------

Json::Value TRedPitayaTrigger::UpdateFromJson(Json::Value &jSetup)
{
    Json::Value jNew;

    try {
        SetLevel (jSetup["level"].asString());
        SetDir (jSetup["dir"].asString());
        SetSrc (jSetup["src"].asString());
        jNew = AsJson();
    }
    catch (std::exception exp) {
        jNew["error"] = exp.what();
    }
    return (jNew);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// TRedPitayaSetup
//-----------------------------------------------------------------------------
TRedPitayaSetup::TRedPitayaSetup ()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TRedPitayaSetup::TRedPitayaSetup (const TRedPitayaSetup &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
TRedPitayaSetup TRedPitayaSetup::operator= (const TRedPitayaSetup &other)
{
    AssignAll (other);
    return (*this);
}
//-----------------------------------------------------------------------------
bool TRedPitayaSetup::operator== (const TRedPitayaSetup &other) const
{
    return (true);
}
//-----------------------------------------------------------------------------
bool TRedPitayaSetup::operator!= (const TRedPitayaSetup &other) const
{
    return (!(*this == other));
}
//-----------------------------------------------------------------------------
void TRedPitayaSetup::Clear ()
{
    m_trigger.Clear();
    m_sampling.Clear ();
    m_mca_params.Clear();
    SetSamplingOnOff (false);
    SetMcaOnOff (false);
    SetPsdOnOff (false);
}
//-----------------------------------------------------------------------------
void TRedPitayaSetup::AssignAll (const TRedPitayaSetup &other)
{
    SetTrigger (other.GetTrigger());
    SetSampling (other.GetSampling());
    m_mca_params = other.m_mca_params;
    SetSamplingOnOff (other.GetSamplingOnOff ());
    SetMcaOnOff (other.GetMcaOnOff ());
    SetPsdOnOff (other.GetPsdOnOff ());
}
//-----------------------------------------------------------------------------
TRedPitayaTrigger TRedPitayaSetup::GetTrigger() const
{
    return (m_trigger);
}
//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetTrigger (const TRedPitayaTrigger &trigger)
{
    m_trigger = trigger;
}
//-----------------------------------------------------------------------------
TRedPitayaSampling TRedPitayaSetup::GetSampling () const
{
    return (m_sampling);
}
//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetSampling (const TRedPitayaSampling &sampling)
{
    m_sampling = sampling;
}
//-----------------------------------------------------------------------------
TMcaParams TRedPitayaSetup::GetMcaParams () const
{
    return (m_mca_params);
}
//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaParams (const TMcaParams &mca_params)
{
    m_mca_params = mca_params;
}
//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::AppsAsJson()
{
    Json::Value jAppsMca;

    jAppsMca["mca"] = McaAsJson();
    jAppsMca["psd"] = "no psd yet";
    return (jAppsMca);
}
//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::McaAsJson()
{
    Json::Value jMCA;

    jMCA["mca"] = m_mca_params.AsJson();
    return (jMCA);
}
//-----------------------------------------------------------------------------
bool TRedPitayaSetup::LoadFromJson(const string &strFile)
{
	Json::Value root, jSampling, jTrigger;
	Json::Reader reader;
    string strJson = ReadFileAsString (strFile);

    Clear ();
    if (reader.parse (strJson, root)) {
        jSampling = root["sampling"];
        jTrigger = root["trigger"];
		Json::Value jMCA = root["mca"];
        if (!jSampling.isNull())
            m_sampling.LoadFromJson (jSampling);
        if (!jTrigger.isNull())
            m_trigger.LoadFromJson (jTrigger);
        if (!jMCA.isNull())
            m_mca_params.LoadFromJson (jMCA);
    }

    return (true);
}
//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::AsJson()
{
    Json::Value jSetup;

    jSetup["sampling"] = m_sampling.AsJson();
    jSetup["trigger"] = m_trigger.AsJson();
    return (jSetup);
}
//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::UpdateFromJson(Json::Value &jSetup)
{
    Json::Value jNewSetup;

    try {
        jNewSetup["sampling"] = m_sampling.UpdateFromJson(jSetup["sampling"]);
        jNewSetup["trigger"] = m_trigger.UpdateFromJson(jSetup["trigger"]);
        jNewSetup["mca"] = m_mca_params.LoadFromJson (jSetup["mca"]);
    }
    catch (std::exception &exp) {
		jNewSetup["error"] = exp.what();
    }
    return (jNewSetup);
}
//-----------------------------------------------------------------------------
bool TRedPitayaSetup::SaveToJson (const std::string &strFile)
{
    bool fLoad;

    try {
        string strJson = StringifyJson (AsJson());
        FILE *file = fopen (strFile.c_str(), "w+");
        fprintf (file, "%s\n", strJson.c_str());
        fclose (file);
        fLoad = true;
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error in 'SaveToJson':\n%s\n", exp.what());
        fLoad = false;
    }
    return (fLoad);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetSamplingOnOff (bool fSampling)
{
    m_fSamplingOnOff = fSampling;
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::GetSamplingOnOff () const
{
    return (m_fSamplingOnOff);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaOnOff (bool fMca)
{
    m_fMcaOnOff = fMca;
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::GetMcaOnOff () const
{
    return (m_fMcaOnOff);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPsdOnOff (bool fPsd)
{
    m_fPsdOnOff = fPsd;
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::GetPsdOnOff () const
{
    return (m_fPsdOnOff);
}
//-----------------------------------------------------------------------------
