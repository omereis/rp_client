/******************************************************************************\
|                              mca_params.cpp                                  |
\******************************************************************************/
#include "mca_params.h"
//-----------------------------------------------------------------------------
TMcaParams::TMcaParams ()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TMcaParams::TMcaParams (const TMcaParams &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
TMcaParams TMcaParams::operator= (const TMcaParams &other)
{
    AssignAll (other);
    return(*this);
}
//-----------------------------------------------------------------------------
bool TMcaParams::operator== (const TMcaParams &other) const
{
    if (GetChannels() != other.GetChannels ())
        return (false);
    if (GetMinVoltage() != other.GetMinVoltage ())
        return (false);
    if (GetMaxVoltage() != other.GetMaxVoltage ())
        return (false);
    return (true);
}
//-----------------------------------------------------------------------------
bool TMcaParams::operator!= (const TMcaParams &other) const
{
    return (!(*this == other));
}
//-----------------------------------------------------------------------------
void TMcaParams::Clear ()
{
   SetChannels (1024);
   SetMinVoltage (0.0);
   SetMaxVoltage (5.0);
}
//-----------------------------------------------------------------------------
void TMcaParams::AssignAll (const TMcaParams &other)
{
    SetChannels (other.GetChannels ());
    SetMinVoltage (other.GetMinVoltage ());
    SetMaxVoltage (other.GetMaxVoltage ());
}
//-----------------------------------------------------------------------------
uint TMcaParams::GetChannels () const
{
    return (m_uiChannels);
}
//-----------------------------------------------------------------------------
void TMcaParams::SetChannels (uint uiChannels)
{
    m_uiChannels = uiChannels;
}
//-----------------------------------------------------------------------------
double TMcaParams::GetMinVoltage () const
{
    return (m_dMinVoltage);
}
//-----------------------------------------------------------------------------
void TMcaParams::SetMinVoltage (double dMinVoltage)
{
    m_dMinVoltage = dMinVoltage;
}
//-----------------------------------------------------------------------------
double TMcaParams::GetMaxVoltage () const
{
    return (m_dMaxVoltage);
}
//-----------------------------------------------------------------------------
void TMcaParams::SetMaxVoltage (double dMaxVoltage)
{
    m_dMaxVoltage = dMaxVoltage;
}
//-----------------------------------------------------------------------------
bool TMcaParams::LoadFromJson (Json::Value jMCA)
{
    Json::Value jChannels=jMCA["channels"], jMinVoltage=jMCA["min_voltage"], jMaxVoltage=jMCA["max_voltage"];
    bool f;

    try {
        if (!jChannels.isNull())
            SetChannels (jChannels.asInt());
        if (!jMinVoltage.isNull())
            SetMinVoltage (jMinVoltage.asDouble());
        if (!jMaxVoltage.isNull())
            SetMaxVoltage (jMaxVoltage.asDouble());
        f = true;
    }
    catch (...) {
        f = false;
    }
    return (f);
}
//-----------------------------------------------------------------------------
