/******************************************************************************\
|                                rp_setup.cpp                                  |
\******************************************************************************/

#include "rp_setup.h"
#include "bd_types.h"
#include "misc.h"

#include <mutex>

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
    SetBackground (0.1);

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
    SetBackground (GetBackground());
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
        if (!root["background"].isNull())
            SetBackgroundFromJson (root["background"]);
    }

    return (true);
}

//-----------------------------------------------------------------------------
std::string DoubleAsString (double dValue)
{
    char sz[20];

    sprintf (sz, "%.3f", dValue);
    std::string str = std::string(sz);
    return (str);
}

//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::AsJson()
{
    Json::Value jSetup;

    jSetup["sampling"] = m_sampling.AsJson();
    jSetup["trigger"] = m_trigger.AsJson();
    jSetup["background"] = DoubleAsString (GetBackground());
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
    mutex mtx;

    mtx.lock();
    m_fSamplingOnOff = fSampling;
    mtx.unlock();
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::GetSamplingOnOff () const
{
    mutex mtx;

    mtx.lock();
    bool f = m_fSamplingOnOff;
    mtx.unlock();
    return (f);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaOnOff (bool fMca)
{
    mutex mtx;

    mtx.lock();
    m_fMcaOnOff = fMca;
    mtx.unlock();
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::GetMcaOnOff () const
{
    mutex mtx;

    mtx.lock();
    bool f = m_fMcaOnOff;
    mtx.unlock();
    return (f);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPsdOnOff (bool fPsd)
{
    mutex mtx;

    mtx.lock();
    m_fPsdOnOff = fPsd;
    mtx.unlock();
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::GetPsdOnOff () const
{
    mutex mtx;

    mtx.lock();
    bool f = m_fPsdOnOff;
    mtx.unlock();
    return (f);
}

//-----------------------------------------------------------------------------
float TRedPitayaSetup::GetTriggerLevel ()
{
    std::string strTrigger = GetTrigger().GetLevel();
    float fLevel;

    try {
        fLevel = (float) stod (strTrigger);
    }
    catch (std::exception &exp) {
        fLevel = -1;
        fprintf (stderr, "Runtime error in 'GetTriggerLevel':\n%s\n", exp.what());
    }
    return (fLevel);
}

//-----------------------------------------------------------------------------
double TRedPitayaSetup::GetBackground() const
{
    return (m_dBackground);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetBackground (double dBackground)
{
    m_dBackground = dBackground;
}
    
//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetBackgroundFromJson (Json::Value jBkgnd)
{
    try {
        string str = jBkgnd.asString();
        SetBackground (stod(str));
    }
    catch (std::exception &exp) {
		fprintf (stderr, "Runtime error on 'TRedPitayaSetup::SetBackgroundFromJson'\n%s\n", exp.what());
    }
}
//-----------------------------------------------------------------------------
