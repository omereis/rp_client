/******************************************************************************\
|                              PulseFilterInfo.h                               |
\******************************************************************************/
#ifndef	_PULSE_FILTER_INFO_H
#define	_PULSE_FILTER_INFO_H

#include "misc.h"
#include "vector"
#include "list"
//---------------------------------------------------------------------------
#include "pulse_info.h"
//---------------------------------------------------------------------------
class TPulseFilter;
typedef queue<TPulseFilter> TPulseQueue;
typedef list<TPulseFilter> TPulseFilterQueue;
//---------------------------------------------------------------------------
class TPulseFilter {
public:
    TPulseFilter ();
    void Clear ();
    TPulseFilter (const TPulseFilter &other);
    void SetPulse (const TDoubleVec &vPulse);
    //void SetPulse (const TFloatVec &vPulse);
    void SetKernel (const TDoubleVec &vFilter);
    void SetFiltered (const TDoubleVec &vFilter);
	void SetDiff (const TDoubleVec &vDiff);
    void Filter ();
    size_t GetFiltered (TDoubleVec &vFiltered) const;
    size_t GetKernel (TDoubleVec &vKernel) const;
    size_t GetFiltered (TFloatVec &vFiltered) const;
    size_t GetPulse (TDoubleVec &vPulse) const;
    size_t GetDiff (TDoubleVec &vDiff) const;
    //size_t GetPulse (TFloatVec &vPulse) const;
    void SetData (const TDoubleVec &vPulse, const TDoubleVec &vFiltered, const TDoubleVec &vKernel, const TDoubleVec &vDiff);
    double GetValueAt (size_t n) const;

    size_t GetFilteredSize() const;
    size_t GetPulseSize() const;
    TDoubleVec::const_iterator GetFilteredBegin() const;
    TDoubleVec::const_iterator GetFilteredEnd() const;
    TDoubleVec::const_iterator GetPulseBegin() const;
    TDoubleVec::const_iterator GetPulseEnd() const;

	void SetPulsesInfo (const TPulseInfoVec &vPulsesInfo);
	size_t GetPulsesInfo (TPulseInfoVec &vPulsesInfo) const;
	size_t GetPulsesIndices (TPulseIndexVec &vIndices) const;
protected:
    void AssignAll (const TPulseFilter &other);
private:
    TDoubleVec m_vPulse;
    TDoubleVec m_vFiltered;
    TDoubleVec m_vKernel;
    TDoubleVec m_vDiff;
	TPulseInfoVec m_vPulsesInfo;
};
#endif
//---------------------------------------------------------------------------
