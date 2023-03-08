/******************************************************************************\
|                             pulse_indices.cpp                                |
\******************************************************************************/
#include "TrpzInfo.h"
//-----------------------------------------------------------------------------
TTRapezInfo::TTRapezInfo ()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TTRapezInfo::TTRapezInfo (const TTRapezInfo &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
void TTRapezInfo::Clear ()
{
    SetRise (10);
    SetFall (10);
    SetOn (50);
    SetHeight (1);
}
//-----------------------------------------------------------------------------
double TTRapezInfo::GetRise() const
{
    return (m_dRise);
}
//-----------------------------------------------------------------------------
void TTRapezInfo::SetRise (Json::Value &jRise)
{
    SetRise(jRise.asFloat());
}

//-----------------------------------------------------------------------------
void TTRapezInfo::SetRise (double dRise)
{
    m_dRise = dRise;
}
//-----------------------------------------------------------------------------
double TTRapezInfo::GetFall () const
{
    return (m_dFall);
}
//-----------------------------------------------------------------------------
void TTRapezInfo::SetFall (double dFall)
{
    m_dFall = dFall;
}
//-----------------------------------------------------------------------------
double TTRapezInfo::GetOn () const
{
    return (m_dOn);
}
//-----------------------------------------------------------------------------
void TTRapezInfo::SetOn (double dOn)
{
    m_dOn = dOn;
}
//-----------------------------------------------------------------------------
double TTRapezInfo::GetHeight () const
{
    return (m_dHeight);
}
//-----------------------------------------------------------------------------
void TTRapezInfo::SetHeight (double dHeight)
{
    m_dHeight = dHeight;
}
//-----------------------------------------------------------------------------
void TTRapezInfo::AssignAll (const TTRapezInfo &other)
{
    SetRise (other.GetRise());
    SetFall (other.GetFall());
    SetOn (other.GetOn());
    SetHeight (other.GetHeight());
}
//-----------------------------------------------------------------------------
Json::Value TTRapezInfo::AsJson()
{
    Json::Value jTrapez;

    jTrapez["rise"] = GetRise();
    jTrapez["fall"] = GetFall();
    jTrapez["on"] = GetOn();
    jTrapez["height"] = GetHeight();
    return (jTrapez);
}
//-----------------------------------------------------------------------------
Json::Value TTRapezInfo::LoadFromJson(Json::Value jTrapez)
{
    try {
        SetRise(jTrapez["rise"]);
        SetFall (jTrapez["fall"]);
        SetOn (jTrapez["on"]);
        SetHeight(jTrapez["height"]);
    }
    catch (std::exception &exp) {
        jTrapez["error"] = exp.what();
    }
    return (jTrapez);
}
//-----------------------------------------------------------------------------
void TTRapezInfo::SetHeight (Json::Value &jHeight)
{
    SetHeight(jHeight.asFloat());
}
//-----------------------------------------------------------------------------
void TTRapezInfo::SetOn (Json::Value &jOn)
{
    SetOn(jOn.asFloat());
}
//-----------------------------------------------------------------------------
void TTRapezInfo::SetFall (Json::Value &jFall)
{
    SetFall(jFall.asFloat());
}
//-----------------------------------------------------------------------------
