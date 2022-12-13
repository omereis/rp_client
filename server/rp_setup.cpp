/******************************************************************************\
|                                rp_setup.cpp                                  |
\******************************************************************************/

#include "rp_setup.h"
#include "bd_types.h"
#include "misc.h"

#include <mutex>

static const char *g_szPackageSize = "package_size";
static const int g_nDefaultPackageSize = 1024;
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
/*
	rp_Init();
*/
    m_trigger.Clear();
    m_sampling.Clear ();
    m_mca_params.Clear();
    SetSamplingOnOff (false);
    SetMcaOnOff (false);
    SetPsdOnOff (false);
    SetBackground (0.1);
    SetPackageSize (g_nDefaultPackageSize);
    SetPreTriggerNs (100);
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
    SetBackground (other.GetBackground());
    SetPackageSize (other.GetPackageSize());
    SetPreTriggerNs (other.GetPreTriggerNs());
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
	Json::Value root, jResult;//, jSampling, jTrigger;
	Json::Reader reader;
    string strJson = ReadFileAsString (strFile);

    Clear ();
    if (reader.parse (strJson, root)) {
        jResult = UpdateFromJson(root);
    }
	fprintf (stderr, "'TRedPitayaSetup::LoadFromJson' end:\n");
    //m_trigger.Print("Called from rp_setup.cpp:123");
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
Json::Value TRedPitayaSetup::TriggerAsJson()
{
    Json::Value jSetup;

    jSetup["trigger"] = m_trigger.AsJson();
    return (jSetup);
}

//-----------------------------------------------------------------------------
std::string TRedPitayaSetup::PreTriggerAsString (double dPreTrigger)
{
	char *sz = new char[50];
	sprintf (sz, "%.2f", dPreTrigger);
	std::string s (sz);
	delete[] sz;
	return (s);
}

//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::AsJson()
{
    Json::Value jSetup;

	//fprintf (stderr, "TRedPitayaSetup::AsJson\n");
    jSetup["sampling"] = m_sampling.AsJson();
    jSetup["trigger"] = m_trigger.AsJson();
	//fprintf (stderr, "TRedPitayaSetup::AsJson, trigger read\n");
    jSetup["mca"] = m_mca_params.AsJson();
	//fprintf (stderr, "TRedPitayaSetup::AsJson, MCA read\n");
    jSetup["background"] = DoubleAsString (GetBackground());
    jSetup["package_size"] = to_string (GetPackageSize());
	jSetup["pre_trigger_ns"] = PreTriggerAsString (GetPreTriggerNs());// to_string (GetPreTriggerNs());
	//fprintf (stderr, "TRedPitayaSetup::AsJson, Setup set\n");
	//fprintf (stderr, "TRedPitayaSetup::AsJson, sampling read\n%s\n\n", StringifyJson (jSetup["sampling"]).c_str());
    //fprintf (stderr, "TRedPitayaSetup::AsJson\n%s\n\n", StringifyJson (jSetup).c_str());
    return (jSetup);
}

//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::UpdateFromJson(Json::Value &jSetup, bool fUpdateHardware)
{
    Json::Value jNewSetup;

    try {
		std::string str = StringifyJson (jSetup);
        jNewSetup["sampling"] = m_sampling.UpdateFromJson(jSetup["sampling"]);
        jNewSetup["trigger"] = m_trigger.UpdateFromJson(jSetup["trigger"]);
        //m_trigger.Print("Called from UpdateFromJson #165");
        jNewSetup["mca"] = m_mca_params.LoadFromJson (jSetup["mca"]);
		if (!jSetup["background"].isNull()) {
			std::string strBkgnd = jSetup["background"].asString();
			//fprintf (stderr, "UpdateFromJson, #170, strBkgnd: %s\n", strBkgnd.c_str());
			double d = stod (strBkgnd);
			SetBackground (d);
			//fprintf (stderr, "New Background: %g\n", GetBackground());
		}
        if (!jSetup[g_szPackageSize].isNull()) {
			int nPkgSize=GetPackageSize();
            if (jSetup[g_szPackageSize].isInt())
                nPkgSize = jSetup[g_szPackageSize].asInt();
			else if (jSetup[g_szPackageSize].isString())
                nPkgSize = atoi (jSetup[g_szPackageSize].asString().c_str());
			SetPackageSize (nPkgSize);
		}
        SetPreTriggerNs (jSetup["pre_trigger_ns"]);
/*
*/
		if (fUpdateHardware) {
#ifdef	_RED_PITAYA_HW
			//fprintf (stderr, "=================++++++++++++++++++++==================\n");
			//fprintf (stderr, "rp_setup.cpp:178\n");
			//fprintf (stderr, "=================++++++++++++++++++++==================\n");
			m_trigger.SetHardwareTrigger();
#else
			;
#endif
        	//m_trigger.Print("Called from rp_setup.cpp (UpdateFromJson): 177");
		}
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

	//fprintf (stderr, "TRedPitayaSetup::SetSamplingOnOff, fSampling:%d\n", (int) fSampling);
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
void TRedPitayaSetup::SetBackground (const string &strBackground)
{
	try {
		double dBackground = stod (strBackground);
		SetBackground (dBackground);
	}
    catch (std::exception &exp) {
		PrintRuntimeError (exp, "'TRedPitayaSetup::SetBackground'");
    }
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
void TRedPitayaSetup::TriggerNow ()
{
	m_trigger.TriggerNow ();
}

#include "rp_server.h"
//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::HandleBackground (Json::Value &jBkgnd)
{
	Json::Value jSetup;
	Json::Value jSet = jBkgnd["set"];


	if (!jBkgnd.isNull()) {
		if (!jSet.isNull()) {
			SetBackground (jSet.asString());
		}
		if (jBkgnd.isString()) {
			string strCommand = ToLower (jBkgnd.asString());
			//if (strCommand == "read")
    			//jSetup["background"] = DoubleAsString (GetBackground());
#ifdef	_RED_PITAYA_HW
			if (strCommand == "measure") {
                TFloatVec vPulse;
                if (ReadHardwareSamples (*this, vPulse)) {
                    SetBackground (VectorAverage (vPulse));
                }
            }
#endif
		}
	}
    jSetup["background"] = DoubleAsString (GetBackground());
	return (jSetup);
}

//-----------------------------------------------------------------------------
int TRedPitayaSetup::GetPackageSize() const
{
    return (m_nPackageSize);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPackageSize (int nPackageSize)
{
    m_nPackageSize = nPackageSize;
}

//-----------------------------------------------------------------------------
float TRedPitayaSetup::GetPreTriggerNs() const
{
    return (m_fPreTriggerNs);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPreTriggerNs (float fDelayNs)
{
    m_fPreTriggerNs = fDelayNs;
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPreTriggerNs (const std::string &str)
{
    try {
        double dPre = std::stod (str);
        SetPreTriggerNs (dPre);
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error setting pre trigger:\n%s\n", exp.what());
    }
}
//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPreTriggerNs (Json::Value jPreTrigger)
{
    if (!jPreTrigger.isNull()) {
        if (jPreTrigger.isString())
            SetPreTriggerNs (jPreTrigger.asString());
        else if (jPreTrigger.isDouble())
            SetPreTriggerNs (jPreTrigger.asDouble());
    }
}

//-----------------------------------------------------------------------------

#ifdef	_RED_PITAYA_HW
bool TRedPitayaSetup::LoadFromHardware (bool fInit)
{
	bool fLoad;
	//fprintf (stderr, "rp_setup.cpp:326, before calling rp_AcqGetTriggerSrc\n");
	//if (fInit)
		////rp_Init();
	rp_acq_trig_src_t dir;

	//fprintf (stderr, "rp_setup.cpp:326, before calling rp_AcqGetTriggerSrc\n");
	//rp_Init();
	try {
		rp_AcqGetTriggerSrc(&dir);
    	m_trigger.LoadFromHardware();
		fLoad = true;
	}
	catch (std::exception &exp) {
		fprintf (stderr, "Runtime error in 'TRedPitayaSetup::LoadFromHardware':\n%s", exp.what());
		fLoad = false;
	}
	//fprintf (stderr, "rp_setup.cpp:329, AFTER calling rp_AcqGetTriggerSrc\n");
	//string strDir = m_trigger.GetHardwareTriggerSource (dir);
	//fprintf (stderr, "rp_setup.cpp:331, AFTER calling GetHardwareTriggerSource \n");
	//fprintf (stderr, "'TRedPitayaTrigger::LoadFromHardware', after calling 'rp_AcqGetTriggerSrc', trigger source: %s\n", m_trigger.GetHardwareTriggerSource (dir).c_str());
	//m_trigger.Print("'TRedPitayaSetup::LoadFromHardware', before LoadFromhardware");
	//PrintTriggerSource ("rp_setup.cpp:336");
	//m_trigger.Print("'TRedPitayaSetup::LoadFromHardware', AFTER LoadFromhardware");
	return (fLoad);
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::SetHardwareTrigger()
{
	return (SetHardwareTrigger(m_trigger));
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::SetHardwareTrigger(const TRedPitayaTrigger &trigger)
{
	bool fSet;

	try {
		m_trigger = trigger;
		m_trigger.SetHardwareTrigger ();
		fSet = true;
	}
	catch (std::exception exp) {
		fprintf (stderr, "Runtime error in 'TRedPitayaSetup::SetHardwareTrigger':\n%s", exp.what());
		fSet = false;
	}
	return (fSet);
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::PrintHardwareSetup (FILE *file)
{
	return (m_trigger.Print ("PrintHardwareSetup", file));
	//m_trigger.PrintHardwareSetup (file);
}

//-----------------------------------------------------------------------------
rp_acq_decimation_t TRedPitayaSetup::GetHardwareDecimation() const
{
	return (m_sampling.GetHardwareDecimation());
}

//-----------------------------------------------------------------------------
rp_channel_trigger_t TRedPitayaSetup::GetHardwareTriggerChannel() const
{
	return (m_trigger.GetHardwareTriggerChannel());
}

//-----------------------------------------------------------------------------
float TRedPitayaSetup::GetHardwareTriggerLevel() const
{
	return (m_trigger.GetHardwareTriggerLevel());
}

//-----------------------------------------------------------------------------
rp_acq_sampling_rate_t TRedPitayaSetup::GetHardwareSamplingRate() const
{
	return (m_sampling.GetHardwareSamplingRate());
}

//-----------------------------------------------------------------------------
rp_acq_trig_src_t TRedPitayaSetup::GetHardwareTriggerSource() const
{
	return (m_trigger.GetHardwareTriggerSource());
}
#endif
//-----------------------------------------------------------------------------
