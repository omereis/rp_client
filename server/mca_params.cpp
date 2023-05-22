/******************************************************************************\
|                              mca_params.cpp                                  |
\******************************************************************************/
#include <mutex>
#include <math.h>
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
	m_vPulses.clear();
    SetMax (0);
    SetMin (0);
	SetCount (0);
}
//-----------------------------------------------------------------------------
void TMcaParams::AssignAll (const TMcaParams &other)
{
    SetChannels (other.GetChannels ());
    SetMinVoltage (other.GetMinVoltage ());
    SetMaxVoltage (other.GetMaxVoltage ());
	m_vPulses = other.m_vPulses;
    SetMax (other.GetMax());
    SetMin (other.GetMin());
	SetCount (other.GetCount());
}
//-----------------------------------------------------------------------------
uint TMcaParams::GetChannels () const
{
    return (m_uiChannels);
}
//-----------------------------------------------------------------------------
void TMcaParams::SetChannels (uint uiChannels)
{
	TFloatVec::iterator i;

    m_uiChannels = uiChannels;
	m_vSpectrum.resize (uiChannels, 0);
	m_vPulses.clear();
	for (i=m_vSpectrum.begin() ; i != m_vSpectrum.end() ; i++)
		*i = 0;
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
void TMcaParams::ResetSpectrum ()
{
	m_vSpectrum.resize (m_vSpectrum.size(), 0);
	SetCount (0);
	m_vPulses.clear();
}

//-----------------------------------------------------------------------------
Json::Value TMcaParams::LoadFromJson (Json::Value jMCA)
{
    Json::Value jNew;

    try {
        if (!jMCA.isNull()) {
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
    }
    catch (std::exception &exp) {
        jMCA["error"] = exp.what();
    }
    return (jMCA);
}

//-----------------------------------------------------------------------------
void TMcaParams::SetSpectrum (const TFloatVec &vSpectrum)
{
	if (vSpectrum.size() != m_vSpectrum.size())
		m_vSpectrum.resize (vSpectrum.size());
	TFloatVec::const_iterator iSrc=vSpectrum.begin();
	TFloatVec::iterator iDest=m_vSpectrum.end();
	for ( ; iSrc != vSpectrum.end() ; iSrc++, iDest++)
		*iDest = *iSrc;
}

//-----------------------------------------------------------------------------
size_t TMcaParams::GetSpectrum (TFloatVec &vSpectrum)
{
	vSpectrum = m_vSpectrum;
	return (vSpectrum.size());
}

//-----------------------------------------------------------------------------
int TMcaParams::HeightIndex (float fSignalMin, float fSignalMax)
{
    int idx;
	double dMaxAmp = fabs (GetMaxVoltage() - GetMinVoltage());
	double dAmplitude = fabs (fSignalMax - fSignalMin);

	if (dMaxAmp > 0)
		idx = (int) ((double) GetChannels()) * (dAmplitude /dMaxAmp);
	else
		idx = -1;

    return (idx);
}

//-----------------------------------------------------------------------------
int TMcaParams::GetMcaPulses() const
{
	mutex mtx;
	int nCount;

    mtx.lock();
	nCount = (int) m_vPulses.size();
    mtx.unlock();
    return (nCount);
}

//-----------------------------------------------------------------------------
void TMcaParams::SetSpectrum (uint uiChannels)
{
	SetChannels (uiChannels);
	m_vSpectrum.resize(uiChannels, 0);
}

//-----------------------------------------------------------------------------
void TMcaParams::NewPulse (const TPulseInfo &pulse_info)
{
	TFloatVec vPulse;
	TFloatVec::const_iterator i;
	int idx;
	
	m_vPulses.push_back (pulse_info);
    if (m_vSpectrum.size() == 0)
        SetSpectrum (GetChannels());
	double dIndex;
	if (pulse_info.GetMaxVal () > GetMinVoltage())
		dIndex = 0;
	else
		dIndex = ((double) GetChannels()) * fabs(pulse_info.GetMaxVal () - GetMinVoltage()) / fabs((GetMaxVoltage() - GetMinVoltage()));
	idx = (int) (dIndex + 0.5);
    if (idx >= 0) {
		if (idx < m_vSpectrum.size())  {
			//idx = m_vSpectrum.size() - 1;
        	int n = m_vSpectrum[idx];
        	m_vSpectrum[idx] = (n + 1);
		}
	}
    if (GetCount() == 0) {
        SetMin(pulse_info.GetMaxVal ());
        SetMax(pulse_info.GetMaxVal ());
    }
    else{
        if (pulse_info.GetMaxVal () < GetMin())
            SetMin(pulse_info.GetMaxVal ());
        if (pulse_info.GetMaxVal () > GetMax())
            SetMax(pulse_info.GetMaxVal ());
    }
    IncreaseCount ();
	static int nCount;
	FILE *file = fopen ("mca_res.csv", "a+");
	fprintf (file, "%g,%g, %g, %d,%d\n", GetMinVoltage(), GetMaxVoltage(), pulse_info.GetMaxVal(), idx, GetChannels());
	fclose (file);
}

//-----------------------------------------------------------------------------
void TMcaParams::NewPulse (const TPulseInfoVec &vPulsesInfo)
{
	TPulseInfoVec ::const_iterator i;

	for (i=vPulsesInfo.begin() ; i != vPulsesInfo.end() ; i++)
		NewPulse (*i);
}

//-----------------------------------------------------------------------------
size_t TMcaParams::GetCount() const
{
	return (m_nCount);
}

//-----------------------------------------------------------------------------
void TMcaParams::SetCount (size_t n)
{
	m_nCount = n;
}


//-----------------------------------------------------------------------------
void TMcaParams::ClearMca ()
{
	TFloatVec::iterator i;

	SetCount (0);
	for (i=m_vSpectrum.begin() ; i != m_vSpectrum.end() ; i++)
		*i = 0;
}

//-----------------------------------------------------------------------------
void TMcaParams::SetMax (double dMax)
{
    m_dMax = dMax;
}

//-----------------------------------------------------------------------------
double TMcaParams::GetMax() const
{
    return (m_dMax);
}

//-----------------------------------------------------------------------------
void TMcaParams::SetMin (double dMin)
{
    m_dMin = dMin;
}

//-----------------------------------------------------------------------------
double TMcaParams::GetMin() const
{
    return (m_dMin);
}

//-----------------------------------------------------------------------------
void TMcaParams::IncreaseCount ()
{
    m_nCount++;
}


