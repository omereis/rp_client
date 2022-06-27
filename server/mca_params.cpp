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
Json::Value TMcaParams::LoadFromJson (Json::Value jMCA)
{
    Json::Value jNew;

    try {
        if (!jMCA.isNull()) {
			//std::string strMca = StringifyJson (jMCA);
			//fprintf (stderr, "Required MCA:\n%s\n", strMca.c_str());
            Json::Value jChannels=jMCA["channels"], jMinVoltage=jMCA["min_voltage"], jMaxVoltage=jMCA["max_voltage"];
            if (!jChannels.isNull())
                SetChannels ((uint)std::stoi(jChannels.asString()));
            if (!jMinVoltage.isNull())
                SetMinVoltage (std::stof (jMinVoltage.asString()));
            if (!jMaxVoltage.isNull())
                SetMaxVoltage (std::stof (jMaxVoltage.asString()));
            jNew = AsJson();
        }
    }
    catch (std::exception &exp) {
        jNew["error"] = exp.what();
    }
    return (jNew);
}
//-----------------------------------------------------------------------------
Json::Value TMcaParams::AsJson () const
{
    Json::Value jMCA;//jChannels=jMCA["channels"], jMinVoltage=jMCA["min_voltage"], jMaxVoltage=jMCA["max_voltage"];

    try {
        jMCA["channels"] = GetChannels();
        jMCA["min_voltage"] = GetMinVoltage();
        jMCA["max_voltage"] = GetMaxVoltage();
		//std::string strMca = StringifyJson (jMCA);
		//fprintf (stderr, "MCA setup:\n%s\n", strMca.c_str());
    }
    catch (std::exception &exp) {
        jMCA["error"] = exp.what();
    }
    return (jMCA);
}
//-----------------------------------------------------------------------------
