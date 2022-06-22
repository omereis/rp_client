/******************************************************************************\
|                              pulse_info.cpp                                  |
\******************************************************************************/
#include "pulse_info.h"
//-----------------------------------------------------------------------------
TPulseInfo::TPulseInfo ()
{
	Clear ();
}

//-----------------------------------------------------------------------------
TPulseInfo::TPulseInfo (const TPulseInfo &other)
{
	AssignAll (other);
}

//-----------------------------------------------------------------------------
void TPulseInfo::Clear ()
{
	m_vPulse.clear ();
    m_vRawPulse.clear ();
    SetArea (0);
    SetMaxVal (0);
    SetLength (0);
    SetPrompt (0);
    SetDelayed (0);
}

//-----------------------------------------------------------------------------
void TPulseInfo::AssignAll (const TPulseInfo &other)
{
	SetPulse (other.GetPulse ());
    SetRawPulse (other.GetRawPulse());
    SetArea (other.GetArea ());
    SetMaxVal (other.GetMaxVal ());
    SetLength (other.GetLength ());
    SetPrompt (other.GetPrompt ());
    SetDelayed (other.GetDelayed ());
}

//-----------------------------------------------------------------------------
double TPulseInfo::GetArea () const
{
    return (m_dArea);
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetArea (double dArea)
{
	m_dArea = dArea;
}

//-----------------------------------------------------------------------------
double TPulseInfo::GetMaxVal () const
{
	return (m_dMaxVal);
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetMaxVal (double dMaxVal)
{
    m_dMaxVal = dMaxVal;
}

//-----------------------------------------------------------------------------
double TPulseInfo::GetLength () const
{
	return (m_dLength);
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetLength (double dLength)
{
    m_dLength = m_dLength;
}

//-----------------------------------------------------------------------------
double TPulseInfo::GetPrompt () const
{
	return (m_dPrompt);
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetPrompt (double dPrompt)
{
    m_dPrompt = dPrompt;
}

//-----------------------------------------------------------------------------
double TPulseInfo::GetDelayed () const
{
	return (m_dDelayed);
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetDelayed (double dDelayed)
{
    m_dDelayed = dDelayed;
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetRawPulse (const TFloatVec &vPulse)
{
    m_vRawPulse = vPulse;
}

//-----------------------------------------------------------------------------
TFloatVec TPulseInfo::GetRawPulse() const
{
    return (m_vRawPulse);
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetPulse (const TFloatVec &vPulse)
{
    m_vPulse = vPulse;
}

//-----------------------------------------------------------------------------
TFloatVec TPulseInfo::GetPulse() const
{
    return (m_vPulse);
}

//-----------------------------------------------------------------------------
TFloatVec::const_iterator TPulseInfo::GetPulseBegin() const
{
    return (m_vPulse.begin());
}

//-----------------------------------------------------------------------------
TFloatVec::const_iterator TPulseInfo::GetPulseEnd() const
{
    return (m_vPulse.end());
}

//-----------------------------------------------------------------------------
TFloatVec::const_iterator TPulseInfo::GetRawPulseBegin() const
{
    return (m_vRawPulse.begin());
}

//-----------------------------------------------------------------------------
TFloatVec::const_iterator TPulseInfo::GetRawPulseEnd() const
{
    return (m_vRawPulse.end());
}
//-----------------------------------------------------------------------------
