/******************************************************************************\
|                                TrpzInfo.h                                    |
\******************************************************************************/
#ifndef	_TRAPZE_INFO_H
#define	_TRAPZE_INFO_H
//-----------------------------------------------------------------------------
#include "jsoncpp/json/json.h"
#include "bd_types.h"
//-----------------------------------------------------------------------------
class TTRapezInfo {
public:
    TTRapezInfo ();
    TTRapezInfo (const TTRapezInfo &other);
    void Clear ();

    size_t GetTrapez (TDoubleVec &vTrapez) const;
	void GenerateTrapez (TDoubleVec &vTrapez) const;
    //void GenerateTrapez ();

    Json::Value AsJson();
    Json::Value LoadFromJson(Json::Value jTrapez);
    double GetRise() const;
    void SetRise (Json::Value &jRise);
    void SetRise (double dRise);
    double GetFall () const;
    void SetFall (double dFall);
    void SetFall (Json::Value &jFall);

    double GetOn () const;
    void SetOn (double dOn);
    void SetOn (Json::Value &jOn);

    double GetHeight () const;
    void SetHeight (double dHeight);
    void SetHeight (Json::Value &jHeight);

    double GetFactor() const;
    void SetFactor (double dFactor);
    void SetFactor (Json::Value &jFactor);

    bool GetOnOff () const;
    void SetOnOff (bool fOnOff);
    void SetOnOff (Json::Value &jOnOff);
protected:
    void AssignAll (const TTRapezInfo &other);
private:
    TDoubleVec m_vTrapez;
    double m_dRise;
    double m_dFall;
    double m_dOn;
    double m_dHeight;
    double m_dFactor;
    bool m_fOnOff;
};
//-----------------------------------------------------------------------------
#endif
