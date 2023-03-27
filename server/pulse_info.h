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

	string asString() const;

	void AddPulse (TDoubleVec::iterator iStart, TDoubleVec::iterator iEnd);
	//void AddPulse (TFloatVec::iterator iStart, TFloatVec::iterator iEnd);
    void SetArea (double dArea);
    double GetArea () const;
    void SetMaxVal (double dMaxVal);
    double GetMaxVal () const;
    void SetMinVal (double dMinVal);
    double GetMinVal () const;
    void SetLength (double dLength);
    double GetLength () const;
    void SetPrompt (double m_dPrompt);
    double GetPrompt () const;
    void SetDelayed (double dDelayed);
    double GetDelayed () const;
    void SetPulse (const TDoubleVec &vPulse);
    TDoubleVec GetPulse() const;
    void SetRawPulse (const TDoubleVec &vRawPulse);
    TDoubleVec GetRawPulse() const;
/*
    void SetPulse (const TFloatVec &vPulse);
    TFloatVec GetPulse() const;
    void SetRawPulse (const TFloatVec &vRawPulse);
    TFloatVec GetRawPulse() const;
*/

	void SetBackground (double dBkgnd);
	double GetBackground () const;
	void SetPulseID (int id);
	int GetPulseID () const;
    TDoubleVec::const_iterator GetPulseBegin() const;
    TDoubleVec::const_iterator GetPulseEnd() const;
    TDoubleVec::const_iterator GetRawPulseBegin() const;
    TDoubleVec::const_iterator GetRawPulseEnd() const;
/*
    TFloatVec::const_iterator GetPulseBegin() const;
    TFloatVec::const_iterator GetPulseEnd() const;
    TFloatVec::const_iterator GetRawPulseBegin() const;
    TFloatVec::const_iterator GetRawPulseEnd() const;
*/
protected:
    void AssignAll (const TPulseInfo &other);
private:
    TDoubleVec m_vPulse;
    TDoubleVec m_vRawPulse;
    //TFloatVec m_vPulse;
    //TFloatVec m_vRawPulse;
    double m_dArea;
    double m_dMaxVal;
	double m_dMinVal;
    double m_dLength;
    double m_dPrompt;
    double m_dDelayed;
    double m_dBackground;
	int m_id;
};
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
