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
    SetOnOff (true);
    SetHeight (1);
    SetFactor (0);
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
    SetFactor (other.GetFactor());
    SetOnOff (other.GetOnOff());
    m_vTrapez = other.m_vTrapez;
}
//-----------------------------------------------------------------------------
Json::Value TTRapezInfo::AsJson()
{
    Json::Value jTrapez;

    jTrapez["rise"] = GetRise();
    jTrapez["fall"] = GetFall();
    jTrapez["on"] = GetOn();
    jTrapez["on_off"] = GetOnOff();
    jTrapez["height"] = GetHeight();
    jTrapez["factor"] = GetFactor();
    return (jTrapez);
}

//-----------------------------------------------------------------------------
Json::Value TTRapezInfo::LoadFromJson(Json::Value jTrapez)
{
    try {
        SetRise(jTrapez["rise"]);
        SetFall (jTrapez["fall"]);
        SetOn (jTrapez["on"]);
        SetOnOff (jTrapez["on_off"]);
        SetHeight(jTrapez["height"]);
        SetFactor (jTrapez["factor"]);
        GenerateTrapez (m_vTrapez);
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
size_t TTRapezInfo::GetTrapez (TDoubleVec &vTrapez) const
{
    vTrapez = m_vTrapez;
    return (vTrapez.size());
}

//-----------------------------------------------------------------------------
double TTRapezInfo::GetFactor() const
{
    return (m_dFactor);
}

//-----------------------------------------------------------------------------
void TTRapezInfo::SetFactor (Json::Value &jFactor)
{
    SetFactor (jFactor.asFloat());
}
//-----------------------------------------------------------------------------
void TTRapezInfo::SetFactor (double dFactor)
{
    m_dFactor = dFactor;
}

//-----------------------------------------------------------------------------
void TTRapezInfo::GenerateTrapez (TDoubleVec &vTrapez) const
{
    double d=0, delta, delta_t = 8e-9, dValue=0;
	double dA1=0, dArea = 0;

    vTrapez.clear ();
    if (GetOnOff ()) {
	    int N = (int) (GetRise() / delta_t + 0.5);
	    delta = GetHeight() / N;
        while (d < GetRise()) {
            vTrapez.push_back (dValue);
            dValue += delta;
            d += delta_t;
        }
	    for (d=0 ; d < GetOn() ; d += delta_t) {
            vTrapez.push_back (dValue);
    	}
	    for (d=0 ; d < GetFall() ; d += delta_t) {
            vTrapez.push_back (dValue);
		    dValue -= delta;
	    }
        vTrapez.push_back (0);
	    TDoubleVec::iterator i;
	    size_t n;
	    for (i=vTrapez.begin() ; i != vTrapez.end() ; i++)
		    *i = *i + GetFactor();
    }
	printf ("\n\nArea: %g\nA1=%g\n%d items\n\n", dArea, dA1, (int) m_vTrapez.size());
    PrintVector (m_vTrapez, "trpz.csv");
}

//-----------------------------------------------------------------------------
bool TTRapezInfo::GetOnOff () const
{
    return (m_fOnOff);
}

//-----------------------------------------------------------------------------
void TTRapezInfo::SetOnOff (bool fOnOff)
{
    m_fOnOff = fOnOff;
}

//-----------------------------------------------------------------------------
void TTRapezInfo::SetOnOff (Json::Value &jOnOff)
{
    bool fOnOff;

    try {
        fOnOff = jOnOff.asBool();
    }
    catch (...) {
        fOnOff = false;
    }
    SetOnOff (fOnOff);
}
//-----------------------------------------------------------------------------
