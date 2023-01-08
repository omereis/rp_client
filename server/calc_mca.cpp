/******************************************************************************\
|                                calc_mca.cpp                                  |
\******************************************************************************/
//-----------------------------------------------------------------------------
#include <math.h>
#include "calc_mca.h"
//-----------------------------------------------------------------------------
TCalcMca::TCalcMca ()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TCalcMca::TCalcMca (const TCalcMca &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
void TCalcMca::Clear ()
{
    m_params.Clear();
    ResetSpectrum (m_params.GetChannels());
}
//-----------------------------------------------------------------------------
void TCalcMca::ResetSpectrum (size_t sz)
{
    m_vSpectrum = TFloatVec ();
    if (sz > 0)
        m_vSpectrum.resize(sz);
}
//-----------------------------------------------------------------------------
void TCalcMca::SetParams (const TMcaParams &params)
{
    if (params != m_params) {
        m_params = params;
        ResetSpectrum (m_params.GetChannels());
    }
}
//-----------------------------------------------------------------------------
TMcaParams TCalcMca::GetParams () const
{
    return (m_params);
}

//-----------------------------------------------------------------------------
int TCalcMca::HeightIndex (float fSignalMin, float fSignalMax)
{
    int idx;
	double dMaxAmp = fabs (m_params.GetMaxVoltage() - m_params.GetMinVoltage());
	double dAmplitude = fabs (fSignalMax - fSignalMin);

	if (dMaxAmp > 0)
		idx = (int) ((double) m_params.GetChannels()) * (dAmplitude /dMaxAmp);
	else
		idx = -1;

/*
    if ((m_params.GetMaxVoltage() > m_params.GetMinVoltage()) && (fSignalMax > fSignalMin)) {
        float fIndex = (fSignalMax - fSignalMin) / (m_params.GetMaxVoltage() - m_params.GetMinVoltage());
        idx = (int) (fIndex * m_params.GetChannels());
		if (idx >= m_params.GetChannels())
			idx >= m_params.GetChannels();
    }
    else
        idx = -1;
*/
    return (idx);
}

/*
//-----------------------------------------------------------------------------
void GetVectorMinMax (const TFloatVec &vPulse, float &fMin, float &fMax)
{
    TFloatVec::const_iterator i;

    i = vPulse.begin();
    fMin = fMax = *i;
    for (i++ ; i != vPulse.end() ; i++) {
        fMin = min (fMin, *i);
        fMax = max (fMax, *i);
    }
}
*/

//-----------------------------------------------------------------------------
void TCalcMca::NewPulse (const TFloatVec &vPulse)
{
    float fMin, fMax, fHeight;

    GetVectorMinMax (vPulse, fMin, fMax);
    int idx = HeightIndex (fMin, fMax);
    if (idx >= 0) {
		if (idx >= m_vSpectrum.size())
			idx = m_vSpectrum.size() - 1;
        int n = m_vSpectrum[idx];
        m_vSpectrum[idx] = (n + 1);
	}
}

//-----------------------------------------------------------------------------
void TCalcMca::NewPulse (const TPulseInfo &pulse_info)
{
	TFloatVec vPulse;
	TFloatVec::const_iterator i;

    if (m_vSpectrum.size() == 0)
        ResetSpectrum (m_params.GetChannels());
    int idx = HeightIndex (pulse_info.GetBackground(), pulse_info.GetMinVal ());
    //int idx = HeightIndex (0, pulse_info.GetMaxVal ());
    if (idx >= 0) {
		if (idx >= m_vSpectrum.size())  {
			idx = m_vSpectrum.size() - 1;
		}
        int n = m_vSpectrum[idx];
        m_vSpectrum[idx] = (n + 1);
	}
	//fclose (file);
	//fclose (filePulse);
}

//-----------------------------------------------------------------------------
void TCalcMca::NewPulse (const TPulseInfoVec &piVec)
{
	TPulseInfoVec::const_iterator i;

	for (i=piVec.begin() ; i != piVec.end() ; i++)
		NewPulse (*i);
}

//-----------------------------------------------------------------------------
void TCalcMca::ResetSpectrum ()
{
    ResetSpectrum (m_params.GetChannels());
}
//-----------------------------------------------------------------------------
void TCalcMca::GetSpectrum (TFloatVec &vSpectrum)
{
    vSpectrum = m_vSpectrum;
}
//-----------------------------------------------------------------------------
void TCalcMca::AssignAll (const TCalcMca &other)
{
    m_params = other.m_params;
    m_vSpectrum = other.m_vSpectrum;
}
//-----------------------------------------------------------------------------
uint TCalcMca::CountItemsInSpectrum()
{
    uint ui;
    TFloatVec::iterator i;

    for (i=m_vSpectrum.begin(), ui=0 ; i != m_vSpectrum.end() ; i++)
        if (*i != 0)
            ui++;
    return (ui);
}
//-----------------------------------------------------------------------------
