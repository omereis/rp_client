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
    SetMinVal (0);
    SetLength (0);
    SetPrompt (0);
    SetDelayed (0);
	SetPulseID (0);
	SetBackground (0);
    m_indices.Clear();
}

//-----------------------------------------------------------------------------
void TPulseInfo::AssignAll (const TPulseInfo &other)
{
	SetPulse (other.GetPulse ());
    SetRawPulse (other.GetRawPulse());
    SetArea (other.GetArea ());
    SetMaxVal (other.GetMaxVal ());
    SetMinVal (other.GetMinVal ());
    SetLength (other.GetLength ());
    SetPrompt (other.GetPrompt ());
    SetDelayed (other.GetDelayed ());
	SetPulseID (other.GetPulseID() );
	SetBackground (other.GetBackground ());
    SetIndices (other.GetIndices());
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
void TPulseInfo::SetMinVal (double dMinVal)
{
	m_dMinVal = dMinVal;
}

//-----------------------------------------------------------------------------
double TPulseInfo::GetMinVal () const
{
	return (m_dMinVal);
}

//-----------------------------------------------------------------------------
double TPulseInfo::GetLength () const
{
	return (m_dLength);
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetLength (double dLength)
{
    m_dLength = dLength;
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
void TPulseInfo::SetRawPulse (const TDoubleVec &vPulse)
//void TPulseInfo::SetRawPulse (const TFloatVec &vPulse)
{
    m_vRawPulse = vPulse;
}

//-----------------------------------------------------------------------------
TDoubleVec TPulseInfo::GetRawPulse() const
{
    return (m_vRawPulse);
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetBackground (double dBkgnd)
{
	m_dBackground = dBkgnd;
}

//-----------------------------------------------------------------------------
double TPulseInfo::GetBackground () const
{

	return (m_dBackground);
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetPulse (const TDoubleVec &vPulse)
//void TPulseInfo::SetPulse (const TFloatVec &vPulse)
{
    m_vPulse = vPulse;
}

//-----------------------------------------------------------------------------
TDoubleVec TPulseInfo::GetPulse() const
//TFloatVec TPulseInfo::GetPulse() const
{
    return (m_vPulse);
}

//-----------------------------------------------------------------------------
TDoubleVec::const_iterator TPulseInfo::GetPulseBegin() const
//TFloatVec::const_iterator TPulseInfo::GetPulseBegin() const
{
    return (m_vPulse.begin());
}

//-----------------------------------------------------------------------------
TDoubleVec::const_iterator TPulseInfo::GetPulseEnd() const
//TFloatVec::const_iterator TPulseInfo::GetPulseEnd() const
{
    return (m_vPulse.end());
}

//-----------------------------------------------------------------------------
TDoubleVec::const_iterator TPulseInfo::GetRawPulseBegin() const
//TFloatVec::const_iterator TPulseInfo::GetRawPulseBegin() const
{
    return (m_vRawPulse.begin());
}

//-----------------------------------------------------------------------------
TDoubleVec::const_iterator TPulseInfo::GetRawPulseEnd() const
//TFloatVec::const_iterator TPulseInfo::GetRawPulseEnd() const
{
    return (m_vRawPulse.end());
}

//-----------------------------------------------------------------------------
string TPulseInfo::asString() const
{
	string str;

	str = to_string (GetPulseID() ) + "," + to_string (GetArea ()) + "," + to_string (GetMaxVal ()) + "," + to_string (GetLength ()) + "," + to_string (GetPrompt ()) + "," + to_string (GetDelayed ());
	return (str);
}

//-----------------------------------------------------------------------------
void TPulseInfo::SetPulseID (int id)
{
	m_id = id;
}

//-----------------------------------------------------------------------------
int TPulseInfo::GetPulseID () const
{
	return (m_id);
}

//----------------------------------------------------------------------------p
void TPulseInfo::AddPulse (TDoubleVec::iterator iStart, TDoubleVec::iterator iEnd)
{
    m_vRawPulse.clear();
	m_vRawPulse.insert (m_vRawPulse.begin(), iStart, iEnd);
}

//----------------------------------------------------------------------------p
void TPulseInfo::SetIndices (const TPulseIndex &indices)
{
    m_indices = indices;
}

//----------------------------------------------------------------------------p
TPulseIndex TPulseInfo::GetIndices() const
{
    return (m_indices);
}
//----------------------------------------------------------------------------p
