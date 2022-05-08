/******************************************************************************\
|                                 calc_mca.h                                   |
\******************************************************************************/
//-----------------------------------------------------------------------------
#include "mca_params.h"
//-----------------------------------------------------------------------------
class TCalcMca {
public:
    TCalcMca ();
    TCalcMca (const TCalcMca &other);
    void Clear ();
    void SetParams (const TMcaParams &params);
    TMcaParams GetParams () const;
    void NewPulse (const TFloatVec &vPulse);
    void ResetSpectrum ();
    void GetSpectrum (TFloatVec &vSpectrum);
    int HeightIndex (float fSignalMin, float fSignafMax);
    uint CountItemsInSpectrum();
protected:
    void AssignAll (const TCalcMca &other);
    void ResetSpectrum (size_t sz);
private:
    TMcaParams m_params;
    TFloatVec m_vSpectrum;
};
//-----------------------------------------------------------------------------

