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
