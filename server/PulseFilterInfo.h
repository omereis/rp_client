/******************************************************************************\
|                              PulseFilterInfo.h                               |
\******************************************************************************/
#include "misc.h"
#include "vector"
#include "vector"
//---------------------------------------------------------------------------
class TPulseFilter;
typedef queue<TPulseFilter> TPulseQueue;
//---------------------------------------------------------------------------
class TPulseFilter {
public:
    TPulseFilter ();
    void Clear ();
    TPulseFilter (const TPulseFilter &other);
    void SetPulse (const TFloatVec &vPulse);
    void SetKernel (const TDoubleVec &vFilter);
    void SetFiltered (const TDoubleVec &vFilter);
    void Filter ();
    size_t GetFiltered (TDoubleVec &vFiltered) const;
    size_t GetKernel (TDoubleVec &vKernel) const;
    size_t GetFiltered (TFloatVec &vFiltered) const;
    size_t GetPulse (TFloatVec &vPulse) const;
protected:
    void AssignAll (const TPulseFilter &other);
private:
    TFloatVec m_vPulse;
    TDoubleVec m_vFiltered;
    TDoubleVec m_vKernel;
};
//---------------------------------------------------------------------------
