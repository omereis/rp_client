/******************************************************************************\
|                           RedPitayaTrigger.cpp                               |
\******************************************************************************/

#include "RedPitayaTrigger.h"
#include "bd_types.h"
#include "misc.h"

#include <mutex>
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
	try {
		std::string strLevel = to_string (fLevel);
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
	}
	catch (std::exception &e) {
		fprintf (stderr, "Runtime error in 'TRedPitayaTrigger::LoadFromHardware':\n%s\n", e.what());
		fLoad = false;
	}
}
#endif
//-----------------------------------------------------------------------------
