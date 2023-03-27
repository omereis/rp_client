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
void TPulseFilter::SetData (const TDoubleVec &vPulse, const TDoubleVec &vFiltered, const TDoubleVec &vKernel)
{
    SetPulse (vPulse);
    SetKernel (vKernel);
    SetFiltered (vFiltered);
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
    TDoubleVec vKernel, vFiltered, vPulse;
    
    other.GetPulse(vPulse);
    SetPulse (vPulse);
    other.GetKernel (vKernel);
    SetKernel (vKernel);
    other.GetFiltered (vFiltered);
    SetFiltered (vFiltered);

}

//-------------------------------------------
size_t TPulseFilter::GetKernel (TDoubleVec &vKernel) const
{
    vKernel = m_vKernel;
    return (vKernel.size());
}
