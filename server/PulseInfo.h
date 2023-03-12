/******************************************************************************\
|                                 PulseInfo.h                                  |
\******************************************************************************/
#include "misc.h"
//---------------------------------------------------------------------------
class TPulseFilter {
public:
    TPulseFilter ();
    void Clear ();
    TPulseFilter (const TPulseFilter &other);
    void SetPulse (const TFloatVec &vPulse);
    void SetFilter (const TDoubleVec &vFilter);
    void Filter ();
    size_t GetFiltered (TDoubleVec &vFiltered);
    size_t GetPulse (TFloatVec &vPulse) const;
protected:
    void AssignAll (const TPulseFilter &other);
private:
    TFloatVec m_vPulse;
    TDoubleVec m_vFiltered;
    TDoubleVec m_vFilter;
};
//---------------------------------------------------------------------------
