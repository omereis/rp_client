/******************************************************************************\
|                                TrpzInfo.h                                    |
\******************************************************************************/
#ifndef	_TRAPZE_INFO_H
#define	_TRAPZE_INFO_H
//-----------------------------------------------------------------------------
#include "jsoncpp/json/json.h"
//-----------------------------------------------------------------------------
class TTRapezInfo {
public:
    TTRapezInfo ();
    TTRapezInfo (const TTRapezInfo &other);
    void Clear ();

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
protected:
    void AssignAll (const TTRapezInfo &other);
private:
    double m_dRise;
    double m_dFall;
    double m_dOn;
    double m_dHeight;
};
//-----------------------------------------------------------------------------
#endif
