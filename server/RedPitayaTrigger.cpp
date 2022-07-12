/******************************************************************************\
|                           RedPitayaTrigger.cpp                               |
\******************************************************************************/

#include "RedPitayaTrigger.h"
#include "bd_types.h"
#include "misc.h"
#include <string.h>

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
	if (GetType() != other.GetType())
        return (false);
	if (GetTrigger () != other.GetTrigger())
        return (false);
	if (GetNow() != other.GetNow())
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
	SetType ("edge");
	SetTrigger(false);
	SetNow (false);
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::AssignAll (const TRedPitayaTrigger &other)
{
    SetLevel (other.GetLevel());
    SetDir (other.GetDir());
    SetSrc (other.GetSrc());
	SetType (GetType());
	SetTrigger(other.GetTrigger());
	SetNow (other.GetNow ());
}
//-----------------------------------------------------------------------------
string TRedPitayaTrigger::GetLevel () const
{
    return (m_strLevel);
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetLevel (float fLevel)
{
	try {
		std::string strLevel = std::to_string (fLevel);
		SetLevel (strLevel);
	}
	catch (std::exception &e) {
		fprintf (stderr, "Runtime error in 'TRedPitayaTrigger::SetLevel '\n%s\n", e.what());
	}
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetLevel (const string &strLevel)
{
    m_strLevel = strLevel;
/*
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
*/
}

//-----------------------------------------------------------------------------
string TRedPitayaTrigger::GetDir () const
{
    return (m_strDir);
}

//-----------------------------------------------------------------------------
bool TRedPitayaTrigger::GetNow () const
{
	return (m_fNow);
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetNow (bool fNow)
{
	m_fNow = fNow;
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::Print (const char szTitle[], FILE *file)
{
	if (szTitle != NULL)
		if (strlen(szTitle) > 0) {
			fprintf (stderr, "\n+++++++++++++++++++++++++++++++++++++++++++++++\n");
			fprintf (file, "%s\n", szTitle);
		}
	fprintf (stderr, "+++++++++++++++++++++++++++++++++++++++++++++++\n");
	fprintf (stderr, "Trigger:\n");
    fprintf (stderr, "Level: %s\n", GetLevel ().c_str());
	fprintf (stderr, "Direction: %s\n", GetDir ().c_str());
	fprintf (stderr, "Source: %s\n", GetSrc ().c_str());
	fprintf (stderr, "Type: %s\n", GetType().c_str());
	fprintf (stderr, "On/Off: %s\n", GetTrigger() ? "On" : "Off");
	fprintf (stderr, "Now: %s\n", GetNow () ? "true" : "false");
	fprintf (stderr, "\n+++++++++++++++++++++++++++++++++++++++++++++++\n");
}

//-----------------------------------------------------------------------------

#ifdef	_RED_PITAYA_HW

//-----------------------------------------------------------------------------
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

	if (GetTriggerSource (strSrcSrc, strDirSrc, trigger_src)) {
		//fprintf (stderr, "'TRedPitayaTrigger::SetTrigger', before calling 'rp_AcqSetTriggerSrc', trigger source: %s\n", GetHardwareTriggerSource (trigger_src).c_str());
		rp_AcqSetTriggerSrc(trigger_src);
		//rp_AcqSetTriggerSrc (trigger_src);
		rp_AcqGetTriggerSrc(&trigger_src);
		//fprintf (stderr, "'TRedPitayaTrigger::SetTrigger', AFTER calling 'rp_AcqGetTriggerSrc', trigger source: %s\n", GetHardwareTriggerSource (trigger_src).c_str());
	}
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

        if (!jLevel.isNull()) {
			fprintf (stderr, "jLevel not null: %s\n", jLevel.asString().c_str());
            SetLevel (jLevel.asString());
		}
		else
			fprintf (stderr, "jLevel is null\n");
            SetLevel (jLevel.asString());
        if (!jDir.isNull())
            SetDir (jDir.asString());
        if (!jDir.isNull())
            SetSrc (jSrc.asString());
		SetTrigger (jTrigger["enabled"]);
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
    jTrigger["enabled"] = GetTrigger(); // on/off
    //jTrigger["trigger"] = GetTrigger(); // on/off
    jTrigger["now"] = GetNow();
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
    	SetTrigger (jSetup["enabled"]);
    	//SetTrigger (jSetup["trigger"]);
		//Print ("From 'TRedPitayaTrigger::UpdateFromJson', before AsJson");
        jNew = AsJson();
		//Print ("From 'TRedPitayaTrigger::UpdateFromJson', AFTER AsJson");
    }
    catch (std::exception exp) {
        jNew["error"] = exp.what();
    }
    return (jNew);
}
//-----------------------------------------------------------------------------

string TRedPitayaTrigger::GetType() const
{
	return (m_strType);
}
//-----------------------------------------------------------------------------

void TRedPitayaTrigger::SetType (const string &strType)
{
	m_strType = strType;
}
//-----------------------------------------------------------------------------

bool TRedPitayaTrigger::GetTrigger() const
{
	return (m_fTriggerOn);
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetTrigger (Json::Value &jTrigger)
{
	if (!jTrigger.isNull()) {
		//fprintf (stderr, "SetTrigger, value is not null: %s\n", jTrigger.asString().c_str());
		try {
			string str = jTrigger.asString();
			m_fTriggerOn = (str == "true" ? true : false);
			//fprintf (stderr, "SetTrigger argument: %s\n", str.c_str());
		}
		catch (std::exception &e) {
			fprintf (stderr, "Runtime error in 'TRedPitayaTrigger::SetTrigger':\n%s\n", e.what());
		}
	}
	else
		fprintf (stderr, "SetTrigger, value IS null\n");
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetTrigger(bool fTrigger)
{
	m_fTriggerOn = fTrigger;
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::TriggerNow ()
{
	SetNow (true);
#ifdef	_RED_PITAYA_HW
	rp_AcqSetTriggerSrc (RP_TRIG_SRC_NOW);
#endif
}

//-----------------------------------------------------------------------------
#ifdef	_RED_PITAYA_HW
bool TRedPitayaTrigger::LoadFromHardware ()
{
	bool fLoad;

	try {
		float f=0;
		rp_acq_trig_src_t dir;

		if (rp_AcqGetTriggerSrc(&dir) == RP_OK)
			SetDir (dir);
		fprintf (stderr, "RedPitayaTrigger.cpp:389, Trigger level: %s\n", GetLevel().c_str());
		f = TRedPitayaTrigger::GetHardwareTriggerLevel ();
    	SetLevel (f);
		fLoad = true;
	}
	catch (std::exception &e) {
		fprintf (stderr, "Runtime error in 'TRedPitayaTrigger::LoadFromHardware':\n%s\n", e.what());
		fLoad = false;
	}
}

//-----------------------------------------------------------------------------
rp_acq_trig_src_t TRedPitayaTrigger::GetRequiredTriggerSource ()
{
	rp_acq_trig_src_t trigger_src = RP_TRIG_SRC_DISABLED;

	if (GetTrigger()) {
		if (GetNow ())
			trigger_src = RP_TRIG_SRC_NOW;
		else {
			if (GetSrc() == "in1") {
				if (GetDir() == "up")
					trigger_src = RP_TRIG_SRC_CHA_PE;
				else
					trigger_src = RP_TRIG_SRC_CHA_NE;
			}
			else {
				if (GetDir() == "up")
					trigger_src = RP_TRIG_SRC_CHB_PE;
				else
					trigger_src = RP_TRIG_SRC_CHB_NE;
			}
		}
	}
	return (trigger_src);
}

//-----------------------------------------------------------------------------
rp_channel_trigger_t TRedPitayaTrigger::GetRequiredTriggerChannel ()
{
	rp_channel_trigger_t trigger_channel = RP_T_CH_1;
	if (GetSrc() == "in2")
		trigger_channel = RP_T_CH_2;
	return (trigger_channel);
}

//-----------------------------------------------------------------------------
bool TRedPitayaTrigger::SetHardwareTrigger()
{
	bool fSet;

	try {
		rp_acq_trig_src_t trigger_src = GetRequiredTriggerSource ();
		if (rp_AcqSetTriggerSrc (trigger_src) != RP_OK) {
			PrintRuntimeError ("'rp_AcqSetTriggerSrc' failed in TRedPitayaTrigger::SetHardwareTrigger");
			return (false);
		}
		rp_channel_trigger_t trigger_channel = GetRequiredTriggerChannel ();
		float fLevel = stod(GetLevel());
		if (rp_AcqSetTriggerLevel(trigger_channel, fLevel) != RP_OK) {
			PrintRuntimeError ("'rp_AcqSetTriggerLevel' failed in TRedPitayaTrigger::SetHardwareTrigger");
			return (false);
		}
		fSet = true;
	}
	catch (std::exception &e) {
		fSet = false;
		PrintRuntimeError (e, "TRedPitayaTrigger::SetHardwareTrigger");
	}
	return (fSet);
}

/*
//-----------------------------------------------------------------------------
bool TRedPitayaTrigger::SetHardwareTrigger(const TRedPitayaTrigger &trigger)
{
	bool fSet;

	try {
		AssignAll (trigger);
		rp_acq_trig_src_t trigger_src;
		if (GetTrigger() == false)
			trigger_src = RP_TRIG_SRC_DISABLED;
		else if (GetNow ())
			trigger_src = RP_TRIG_SRC_NOW;
		else {
			if (GetSrc() == "in1") {
				if (GetDir() == "up")
					trigger_src = RP_TRIG_SRC_CHA_PE;
				else
					trigger_src = RP_TRIG_SRC_CHA_NE;
			}
			else {
				if (GetDir() == "up")
					trigger_src = RP_TRIG_SRC_CHB_PE;
				else
					trigger_src = RP_TRIG_SRC_CHB_NE;
			}
		}
		//Print (" TRedPitayaTrigger::SetHardwareTrigger");
		//fprintf (stderr, "RedPitayaTrigger.cpp:435, Trigger: %s\n", GetHardwareTriggerSource (trigger_src).c_str());
		GetHardwareTriggerSource (trigger_src);
		//PrintTriggerSource ("TRedPitayaTrigger::SetHardwareTriggerr', before calling 'rp_AcqSetTriggerSrc'");
		//fprintf (stderr, "Target trigger source: %s\n", GetHardwareTriggerName (trigger_src).c_str());
		//fprintf (stderr, "'TRedPitayaTrigger::SetHardwareTrigger', before calling 'rp_AcqSetTriggerSrc', trigger source: %s\n", GetHardwareTriggerSource (trigger_src).c_str());
		//rp_AcqSetTriggerSrc (trigger_src);
		//rp_AcqGetTriggerSrc(&trigger_src);
		//fprintf (stderr, "TRedPitayaTrigger::SetHardwareTriggerr', AFTER calling 'rp_AcqSetTriggerSrc'\n");
		//PrintTriggerSource ("TRedPitayaTrigger::SetHardwareTriggerr', AFTER calling 'rp_AcqSetTriggerSrc'");
		//fprintf (stderr, "Line 433\n");
		//string str = GetHardwareTriggerName (trigger_src);
		//fprintf (stderr, "\n\nLine #444, Trigger: %s\n", str.c_str());//GetHardwareTriggerSource (trigger_src).c_str());
		//fprintf (stderr, "'TRedPitayaTrigger::LoadFromHardware', AFTER calling 'rp_AcqGetTriggerSrc', trigger source: %s\n", GetHardwareTriggerSource (trigger_src).c_str());
		fSet = true;
	}
	catch (std::exception &e) {
		fSet = false;
		PrintRuntimeError (e, "TRedPitayaTrigger::SetHardwareTrigger");
	}
	return (fSet);
}
*/

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
void TRedPitayaTrigger::SetTriggerFromHardware ()
{
	rp_acq_trig_src_t trigger_src;

	rp_AcqGetTriggerSrc(&trigger_src);
	SetTriggerFromHardware (trigger_src);
}

//-----------------------------------------------------------------------------
void TRedPitayaTrigger::SetTriggerFromHardware (rp_acq_trig_src_t trigger_src)
{
	std::string strSource;

    if (trigger_src == RP_TRIG_SRC_DISABLED)
		SetTrigger (false);
	else {
		SetTrigger (true);
		SetNow (false);
    	if (trigger_src == RP_TRIG_SRC_NOW)
			SetNow (true);
    	else if (trigger_src == RP_TRIG_SRC_CHA_PE) {
			SetType ("edge");
			SetDir ("up");
			SetSrc ("in1");
		}
    	else if (trigger_src == RP_TRIG_SRC_CHA_NE) {
			SetType ("edge");
			SetDir ("down");
			SetSrc ("in1");
		}
    	else if (trigger_src == RP_TRIG_SRC_CHB_PE) {
			SetType ("edge");
			SetDir ("up");
			SetSrc ("in2");
		}
    	else if (trigger_src == RP_TRIG_SRC_CHB_NE) {
			SetType ("edge");
			SetDir ("down");
			SetSrc ("in2");
		}
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
	fprintf (stderr, "Trigger is %s\n", strSource.c_str());
	}
}

//-----------------------------------------------------------------------------
std::string TRedPitayaTrigger::GetHardwareTriggerSource ()
{
	rp_acq_trig_src_t trigger_src;

	rp_AcqGetTriggerSrc(&trigger_src);
	return (GetHardwareTriggerSource (trigger_src));
}

//-----------------------------------------------------------------------------
std::string TRedPitayaTrigger::GetHardwareTriggerSource (rp_acq_trig_src_t trigger_src)
{
	//rp_acq_trig_src_t trigger_src;
	std::string strSource;

	//rp_AcqGetTriggerSrc(&trigger_src);
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
	char *sz = new char[30];
	sprintf (sz, "%.3f", fLevel);
	fLevel = std::stof(sz);
	delete[] sz;
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
	//fprintf (stderr, "======================================================\n");
	//fprintf (stderr, "Printing Trigger Setup\n");
	//fprintf (stderr, "file: %x\n", file);
	fprintf (file, "Trigger:\n");
	fprintf (file, "    Level: %g\n", GetHardwareTriggerLevel ());
	fprintf (file, "    Source: %s\n", GetHardwareTriggerSource ().c_str());
	//fprintf (stderr, "======================================================\n");
}

//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
