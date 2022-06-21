/******************************************************************************\
|                               pulse_info.h                                   |
\******************************************************************************/
#ifndef  __RP_DATA_H
#define  __RP_DATA_H
//-----------------------------------------------------------------------------
#include "bd_types.h"
class TPulseInfo;
#ifndef TPulseInfoVec
typedef vector<TPulseInfo> TPulseInfoVec;
#endif
//-----------------------------------------------------------------------------
class TPulseInfo {
public:
    TPulseInfo ();
    TPulseInfo (const TPulseInfo &other);
    void Clear ();

    void SetArea (double dArea);
    double GetArea () const;
    void SetMaxVal (double dMaxVal);
    double GetMaxVal () const;
    void SetLength (double dLength);
    double GetLength () const;
    void SetPrompt (double m_dPrompt);
    double GetPrompt () const;
    void SetDelayed (double dDelayed);
    double GetDelayed () const;
    void SetPulse (const TFloatVec &vPulse);
    TFloatVec GetPulse() const;
    void SetRawPulse (const TFloatVec &vRawPulse);
    TFloatVec GetRawPulse() const;
protected:
    void AssignAll (const TPulseInfo &other);
private:
    TFloatVec m_vPulse;
    TFloatVec m_vRawPulse;
    double m_dArea;
    double m_dMaxVal;
    double m_dLength;
    double m_dPrompt;
    double m_dDelayed;
};
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
