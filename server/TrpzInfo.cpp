/******************************************************************************\
|                             pulse_indices.cpp                                |
\******************************************************************************/
#include "TrpzInfo.h"
#include "misc.h"
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
    SetRise (10e-9);
    SetFall (10e-9);
    SetOn (50e-9);
    SetHeight (1);
    m_vTrapez.clear ();
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
    m_vTrapez = other.m_vTrapez;
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
        GenerateTrapez ();
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
void TTRapezInfo::GenerateTrapez ()
{
    m_vTrapez.clear ();
    double d=0, delta, delta_t = 1e-9, dValue=0;
	int N = (int) (GetRise() / delta_t + 0.5);
	delta = GetHeight() / N;
    while (d < GetRise()) {
        m_vTrapez.push_back (dValue);
        dValue += delta;
        d += delta_t;
    }
	for (d=0 ; d < GetOn() ; d += delta_t)
        m_vTrapez.push_back (dValue);
	for (d=0 ; d < GetFall() ; d += delta_t) {
        m_vTrapez.push_back (dValue);
		dValue -= delta;
	}
    m_vTrapez.push_back (0);
    PrintVector (m_vTrapez, "trpz.csv");
}
//-----------------------------------------------------------------------------
