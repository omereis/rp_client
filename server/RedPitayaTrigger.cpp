/******************************************************************************\
|                           RedPitayaTrigger.cpp                               |
\******************************************************************************/

#include "RedPitayaTrigger.h"
#include "bd_types.h"
#include "misc.h"

#include <mutex>
#include <string>
//
// TRedPitayaSampling
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
	rp_Init();
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
void TRedPitayaTrigger::SetLevel (float fLevel)
{
/*
*/
	try {
		std::string strLevel = std::to_string (fLevel);
		SetLevel (strLevel);
	}
	catch (std::exception &e) {
		fprintf (stderr, "Runtime error in 'TRedPitayaTrigger::SetLevel '\n%s\n", e.what());
	}
}

std::string TriggerChannelName (rp_channel_trigger_t channel)
{
	std::string str;

    if (channel == RP_T_CH_1)
    	str = "RP_T_CH_1";
    else if (channel == RP_T_CH_2)
    	str = "RP_T_CH_2";
    else if (channel == RP_T_CH_EXT)
    	str = "RP_T_CH_EXT";  
    else
    	str = "RP_T_CH_1";
	return (str);
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetLevel (const string &strLevel)
{
    m_strLevel = strLevel;
/*
*/
#ifdef  _RED_PITAYA_HW
	try {
		float fLevel = stof (strLevel);
		rp_channel_trigger_t channel = GetChannel (GetSrc());
		//fprintf (stderr, "Trigger level: %g\n", fLevel);
		//fprintf (stderr, "Trigger Channel: %s\n", TriggerChannelName (channel).c_str());
		rp_AcqSetTriggerLevel (channel, fLevel);
	}
	catch (std::exception &exp) {
		PrintRuntimeError (exp, "TRedPitayaTrigger::SetLevel");
	}
#endif
}
//-----------------------------------------------------------------------------
string TRedPitayaTrigger::GetDir () const
{
    return (m_strDir);
}

#ifdef	_RED_PITAYA_HW

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetDir (rp_acq_trig_src_t dir)
{
	std::string strDir;

	if ((dir == RP_TRIG_SRC_CHA_PE) || (dir == RP_TRIG_SRC_CHB_PE) || (dir == RP_TRIG_SRC_EXT_PE) || (dir == RP_TRIG_SRC_AWG_PE))
		strDir = "Up";
	else if ((dir == RP_TRIG_SRC_CHA_NE) || (dir == RP_TRIG_SRC_CHB_NE) || (dir == RP_TRIG_SRC_EXT_NE) || (dir == RP_TRIG_SRC_AWG_NE))
		strDir = "Down";
	else
		strDir = "";
	if (strDir.length() > 0)
		SetDir (strDir);
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetSrc (rp_acq_trig_src_t src)
{
	string strSrc;

	if ((src == RP_TRIG_SRC_CHA_PE) || (src == RP_TRIG_SRC_CHA_NE))
		strSrc = "In1";
	else if ((src == RP_TRIG_SRC_CHB_PE) || (src == RP_TRIG_SRC_CHB_NE))
		strSrc = "In2";
	else
		strSrc = "";
	if (strSrc.length() > 0)
		SetSrc (strSrc);
}

//-----------------------------------------------------------------------------
bool TRedPitayaTrigger::GetTriggerSource (const std::string &strSrcSrc, const std::string &strDirSrc, rp_acq_trig_src_t &trigger_src)
{
	std::string strSrc = ToLower (strSrcSrc), strDir = ToLower (strDirSrc);
	bool fValid = true;
	
	if ((strSrc == "in1") && (strDir == "up"))
		trigger_src = RP_TRIG_SRC_CHA_PE;
	else if ((strSrc == "in1") && (strDir == "down"))
		trigger_src = RP_TRIG_SRC_CHA_NE;
	if ((strSrc == "in2") && (strDir == "up"))
		trigger_src = RP_TRIG_SRC_CHB_PE;
	else if ((strSrc == "in2") && (strDir == "down"))
		trigger_src = RP_TRIG_SRC_CHB_NE;
	else
		fValid = false;
	return (fValid);
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetTrigger (const std::string &strSrcSrc, const std::string &strDirSrc)
{
	rp_acq_trig_src_t trigger_src;

	if (GetTriggerSource (strSrcSrc, strDirSrc, trigger_src))
		rp_AcqSetTriggerSrc(trigger_src);
}
#endif

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetDir (const string &strDir)
{
    m_strDir = strDir;
#ifdef	_RED_PITAYA_HW
	SetTrigger (GetSrc(), GetDir());
#endif
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
		fprintf (stderr, "Setup Json:\n%s\n", StringifyJson(jSetup).c_str());
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
#ifdef	_RED_PITAYA_HW
bool TRedPitayaTrigger::LoadFromHardware ()
{
	bool fLoad;

	try {
		float f=0;
		rp_acq_trig_src_t dir;

		if (rp_AcqGetTriggerLevel(RP_T_CH_1, &f) == RP_OK)
    		SetLevel (f);
		if (rp_AcqGetTriggerSrc(&dir) == RP_OK)
			SetDir (dir);
		fLoad = true;
		fprintf (stderr, "Trigger loaded: %s, %s\n", GetSrc().c_str(), GetDir().c_str());
	}
	catch (std::exception &e) {
		fprintf (stderr, "Runtime error in 'TRedPitayaTrigger::LoadFromHardware':\n%s\n", e.what());
		fLoad = false;
	}
}

//-----------------------------------------------------------------------------
bool TRedPitayaTrigger::SetHardwareTrigger(const TRedPitayaTrigger &trigger)
{
	bool fSet;

	try {
		rp_acq_trig_src_t trigger_src;

		if (GetTriggerSource (GetSrc (), GetDir (), trigger_src)) {
			SetDir (trigger_src);
			SetSrc (trigger_src);
		}
		fSet = true;
	}
	catch (std::exception &e) {
		fSet = false;
		PrintRuntimeError (e, "TRedPitayaTrigger::SetHardwareTrigger");
	}
}

//-----------------------------------------------------------------------------
rp_channel_trigger_t TRedPitayaTrigger::GetChannel (const std::string &strDirSrc)
{
	std::string strDir = ToLower(strDirSrc);
	rp_channel_trigger_t channel;

	if (strDir == "in1")
		channel = RP_T_CH_1;
	else if (strDir == "in2")
		channel = RP_T_CH_2;
	else if (strDir == "ext")
		channel = RP_T_CH_EXT;
	else
		channel = RP_T_CH_1;
	return (channel);
}

//-----------------------------------------------------------------------------
std::string TRedPitayaTrigger::GetHardwareTriggerSource ()
{
	rp_acq_trig_src_t trigger_src;
	std::string strSource;

	rp_AcqGetTriggerSrc(&trigger_src);
    if (trigger_src == RP_TRIG_SRC_DISABLED)
		strSource = "RP_TRIG_SRC_DISABLED";
    else if (trigger_src == RP_TRIG_SRC_NOW)
		strSource = "RP_TRIG_SRC_NOW";
    else if (trigger_src == RP_TRIG_SRC_CHA_PE)
		strSource = "RP_TRIG_SRC_CHA_PE";
    else if (trigger_src == RP_TRIG_SRC_CHA_NE)
		strSource = "RP_TRIG_SRC_CHA_NE";
    else if (trigger_src == RP_TRIG_SRC_CHB_PE)
		strSource = "RP_TRIG_SRC_CHB_PE";
    else if (trigger_src == RP_TRIG_SRC_CHB_NE)
		strSource = "RP_TRIG_SRC_CHB_NE";
    else if (trigger_src == RP_TRIG_SRC_EXT_PE)
		strSource = "RP_TRIG_SRC_EXT_PE";
    else if (trigger_src == RP_TRIG_SRC_EXT_NE)
		strSource = "RP_TRIG_SRC_EXT_NE";
    else if (trigger_src == RP_TRIG_SRC_AWG_PE)
		strSource = "RP_TRIG_SRC_AWG_PE";
    else if (trigger_src == RP_TRIG_SRC_AWG_NE)
		strSource = "RP_TRIG_SRC_AWG_NE";
	else
		strSource = "";
	return (strSource);
}

//-----------------------------------------------------------------------------
float TRedPitayaTrigger::GetHardwareTriggerLevel ()
{
	rp_channel_trigger_t channel = GetHardwareTriggerChannel ();
	float fLevel = 0;
	rp_AcqGetTriggerLevel(channel, &fLevel);
	return (fLevel);
}

//-----------------------------------------------------------------------------
rp_channel_trigger_t TRedPitayaTrigger::GetHardwareTriggerChannel ()
{
	rp_channel_trigger_t trigger_channel;
	rp_acq_trig_src_t trigger_src;

	rp_AcqGetTriggerSrc(&trigger_src);
	if ((trigger_src == RP_TRIG_SRC_CHA_PE) || (trigger_src == RP_TRIG_SRC_CHA_NE))
		trigger_channel = RP_T_CH_1;
	else if ((trigger_src == RP_TRIG_SRC_CHB_PE) || (trigger_src == RP_TRIG_SRC_CHB_NE))
		trigger_channel = RP_T_CH_2;
	if ((trigger_src == RP_TRIG_SRC_EXT_PE) || (trigger_src == RP_TRIG_SRC_EXT_NE))
		trigger_channel = RP_T_CH_EXT;
	else
		trigger_channel = RP_T_CH_1;
	return (trigger_channel);
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::PrintHardwareSetup (FILE *file)
{
	fprintf (file, "Trigger:\n");
	fprintf (file, "    Level: %g\n", GetHardwareTriggerLevel ());
	fprintf (file, "    Source: %s\n", GetHardwareTriggerSource ().c_str());
}
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
