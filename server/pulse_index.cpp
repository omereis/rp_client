/******************************************************************************\
|                             pulse_indices.cpp                                |
\******************************************************************************/
#include "pulse_index.h"

//---------------------------------------------------------------------------
TPulseIndex::TPulseIndex ()
{
	Clear ();
}

//---------------------------------------------------------------------------
TPulseIndex::TPulseIndex (int nStart, int nEnd)
{
	SetStart (nStart);
	SetEnd (nEnd);
}

//---------------------------------------------------------------------------
TPulseIndex::TPulseIndex (const TPulseIndex &other)
{
	AssignAll (other);
}

//---------------------------------------------------------------------------
void TPulseIndex::Clear ()
{
	SetStart (0);
	SetEnd (0);
	SetSteps (0);
}

//---------------------------------------------------------------------------
int TPulseIndex::GetStart () const
{
	return (m_nStart);
}

//---------------------------------------------------------------------------
void TPulseIndex::SetStart (int nStart)
{
	m_nStart = nStart;
}

//---------------------------------------------------------------------------
int TPulseIndex::GetEnd () const
{
	return (m_nEnd);
}

//---------------------------------------------------------------------------
void TPulseIndex::SetEnd (int nEnd)
{
	m_nEnd = nEnd;
}

//---------------------------------------------------------------------------
void TPulseIndex::AssignAll (const TPulseIndex &other)
{
	SetStart (other.GetStart());
	SetEnd (other.GetEnd());
	SetSteps (other.GetSteps());
}

//---------------------------------------------------------------------------
int TPulseIndex::GetSteps () const
{
	return (m_nSteps);
}

//---------------------------------------------------------------------------
void TPulseIndex::SetSteps (int nSteps)
{
	m_nSteps = nSteps;
}
//---------------------------------------------------------------------------


