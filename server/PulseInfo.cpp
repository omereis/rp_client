/******************************************************************************\
|                                PulseInfo.cpp                                 |
\******************************************************************************/
//---------------------------------------------------------------------------

#include <mutex>
#include <thread>

#include "PulseInfo.h"

//---------------------------------------------------------------------------
TPulseFilter::TPulseFilter ()
{
    Clear ();
}

//---------------------------------------------------------------------------
void TPulseFilter::Clear ()
{

}

//---------------------------------------------------------------------------
TPulseFilter::TPulseFilter (const TPulseFilter &other)
{
    AssignAll (other);
}

//---------------------------------------------------------------------------
void TPulseFilter::SetPulse (const TFloatVec &vPulse)
{
    mutex mtx;

    mtx.lock ();
    m_vPulse = vPulse;
    mtx.unlock ();
}

//---------------------------------------------------------------------------
void TPulseFilter::SetFilter (const TDoubleVec &vFilter)
{
    m_vFiltered = vFilter;
}

//---------------------------------------------------------------------------
void TPulseFilter::Filter ()
{
 // perform convolution
}

//---------------------------------------------------------------------------
size_t TPulseFilter::GetFiltered (TDoubleVec &vFiltered)
{
    if (m_vFilter.size() == 0)
        Filter ();
    vFiltered = m_vFiltered;
    return (m_vFiltered.size());
}

//---------------------------------------------------------------------------
size_t TPulseFilter::GetPulse (TFloatVec &vPulse) const
{
    mutex mtx;

    mtx.lock ();
    vPulse = m_vPulse;
    mtx.unlock ();
    return (vPulse.size());
}

//---------------------------------------------------------------------------
void TPulseFilter::AssignAll (const TPulseFilter &other)
{
    TFloatVec vPulse;
    TDoubleVec vFilter;
    
    other.GetPulse(vPulse);
    SetPulse (vPulse);
    GetFiltered (vFilter);
    SetFilter (vFilter);
/*
*/
}
