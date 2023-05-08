/******************************************************************************\
|                            PulseFilterInfo.cpp                               |
\******************************************************************************/
//---------------------------------------------------------------------------

#include <mutex>
#include <thread>

#include "PulseFilterInfo.h"

//---------------------------------------------------------------------------
TPulseFilter::TPulseFilter ()
{
    Clear ();
}

//---------------------------------------------------------------------------
void TPulseFilter::Clear ()
{
    m_vPulse.clear();
    m_vFiltered.clear();
    m_vKernel.clear();
	m_vDiff.clear();
	m_vPulsesInfo.clear();
}

//---------------------------------------------------------------------------
TPulseFilter::TPulseFilter (const TPulseFilter &other)
{
    AssignAll (other);
}

//---------------------------------------------------------------------------
void TPulseFilter::SetPulse (const TDoubleVec &vPulse)
//void TPulseFilter::SetPulse (const TFloatVec &vPulse)
{
    mutex mtx;

    mtx.lock ();
    m_vPulse = vPulse;
    mtx.unlock ();
}

//---------------------------------------------------------------------------
//convolution algorithm
// SOURCE: https://toto-share.com/2011/11/cc-convolution-source-code/
float *conv(float *A, float *B, int lenA, int lenB, int *lenC)
{
	int nconv;
	int i, j, i1;
	float tmp;
	float *C;

	//allocated convolution array	
	nconv = lenA+lenB-1;
	C = (float*) calloc(nconv, sizeof(float));

	//convolution process
	for (i=0; i<nconv; i++) {
		i1 = i;
		tmp = 0.0;
		for (j=0; j<lenB; j++) {
			if((i1 >= 0) && (i1 < lenA))
				tmp = tmp + (A[i1]*B[j]);
			i1 = i1-1;
			C[i] = tmp;
		}
	}

	//get length of convolution array	
	(*lenC) = nconv;

	//return convolution array
	return(C);
}

//---------------------------------------------------------------------------
void TPulseFilter::SetKernel (const TDoubleVec &vKernel)
{
    m_vKernel = vKernel;
}

//---------------------------------------------------------------------------
void TPulseFilter::SetFiltered (const TDoubleVec &vFiltered)
{
    m_vFiltered = vFiltered;
}

//---------------------------------------------------------------------------
void TPulseFilter::Filter ()
{
    convolution(m_vPulse, m_vKernel, m_vFiltered);
    //consv(m_vPulse, m_vKernel, m_vFiltered);
// perform convolution
}

//---------------------------------------------------------------------------
void TPulseFilter::SetData (const TDoubleVec &vPulse, const TDoubleVec &vFiltered, const TDoubleVec &vKernel, const TDoubleVec &vDiff)
{
    SetPulse (vPulse);
    SetKernel (vKernel);
    SetFiltered (vFiltered);
	SetDiff (vDiff);
}

//---------------------------------------------------------------------------
void TPulseFilter::SetDiff (const TDoubleVec &vDiff)
{
	m_vDiff = vDiff;
}

//---------------------------------------------------------------------------
size_t TPulseFilter::GetDiff (TDoubleVec &vDiff) const
{
	vDiff = m_vDiff;
	return (vDiff.size());
}

//---------------------------------------------------------------------------
size_t TPulseFilter::GetFiltered (TFloatVec &vFiltered) const
{
	TFloatVec::iterator i;
	TDoubleVec::iterator iv;
	TDoubleVec vResult;

    //if (m_vFilter.size() == 0)
        //Filter ();
    //consv(m_vPulse, m_vKernel, vResult);//vFiltered);
    convolution (m_vPulse, m_vKernel, vResult);//vFiltered);
	vFiltered.resize (vResult.size());
	i = vFiltered.begin();
	for (iv=vResult.begin() ; iv != vResult.end() ; iv++, i++)
		*i = *iv;
    return (vResult.size());
/*
    vFiltered.resize(m_vFiltered.size());
	i = vFiltered.begin();
	for (iv=m_vFiltered.begin() ; iv != m_vFiltered.end() ; iv++, i++)
		*i = *iv;
    return (m_vFiltered.size());
	return (17);
*/
}

//---------------------------------------------------------------------------
size_t TPulseFilter::GetFiltered (TDoubleVec &vFiltered) const
{
    //TDoubleVec vFiltered;

    //consv(m_vPulse, m_vKernel, vFiltered);
    //if (m_vFilter.size() == 0)
        //Filter ();
    vFiltered = m_vFiltered;
    return (vFiltered.size());
}

//---------------------------------------------------------------------------
size_t TPulseFilter::GetPulse (TDoubleVec &vPulse) const
//size_t TPulseFilter::GetPulse (TFloatVec &vPulse) const
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
    //TFloatVec vPulse;
    TDoubleVec v;
	TPulseInfoVec vPulsesInfo;

    other.GetPulse(v);
    SetPulse (v);
    other.GetKernel (v);
    SetKernel (v);
    other.GetFiltered (v);
    SetFiltered (v);
	GetDiff (v);
	SetDiff (v);

	other.GetPulsesInfo (vPulsesInfo);
	SetPulsesInfo (vPulsesInfo);
}

//-------------------------------------------
size_t TPulseFilter::GetKernel (TDoubleVec &vKernel) const
{
    vKernel = m_vKernel;
    return (vKernel.size());
}

//-------------------------------------------
double TPulseFilter::GetValueAt (size_t n) const
{
    double dValue = 0;

    if (n < m_vPulse.size())
        dValue = m_vPulse[n];
    return (dValue);
}

//-------------------------------------------
void TPulseFilter::SetPulsesInfo (const TPulseInfoVec &vPulsesInfo)
{
	m_vPulsesInfo = vPulsesInfo;
}

//-------------------------------------------
size_t TPulseFilter::GetPulsesInfo (TPulseInfoVec &vPulsesInfo) const
{
	vPulsesInfo = m_vPulsesInfo;
	return (m_vPulsesInfo.size());
}

//-------------------------------------------
size_t TPulseFilter::GetPulsesIndices (TPulseIndexVec &vIndices) const
{
	TPulseInfoVec vPulsesInfo;
	TPulseIndex pi;
	TPulseInfoVec::iterator iPulseInfo;

	vIndices.clear();
	GetPulsesInfo (vPulsesInfo);
	for (iPulseInfo=vPulsesInfo.begin() ; iPulseInfo != vPulsesInfo.end() ; iPulseInfo++) {
		pi = iPulseInfo->GetIndices ();
		vIndices.push_back (pi);
		pi.Clear();
	}
	return (vIndices.size());
}

//-------------------------------------------
TDoubleVec::const_iterator TPulseFilter::GetFilteredBegin() const
{
	TDoubleVec::const_iterator iBegin;

	if (m_vFiltered.size() > 0)
		iBegin = m_vFiltered.begin();
	else
		iBegin = m_vPulse.begin();
	return (iBegin);
	//return (m_vFiltered.begin());
}

//-------------------------------------------
TDoubleVec::const_iterator TPulseFilter::GetFilteredEnd() const
{
	TDoubleVec::const_iterator iEnd;

	if (m_vFiltered.size() > 0)
		iEnd = m_vFiltered.end();
	else
		iEnd = m_vPulse.end();
	return (iEnd);
	//return (m_vFiltered.end());
}

//-------------------------------------------
TDoubleVec::const_iterator TPulseFilter::GetPulseBegin() const
{
	return (m_vPulse.begin());
}

//-------------------------------------------
TDoubleVec::const_iterator TPulseFilter::GetPulseEnd() const
{
	return (m_vPulse.end());
}

//-------------------------------------------
size_t TPulseFilter::GetFilteredSize() const
{
	return (m_vFiltered.size());
}
