/******************************************************************************\
|                                mca_params.h                                  |
\******************************************************************************/
//-----------------------------------------------------------------------------
#ifndef  __MCA_PARAMS_H
#define  __MCA_PARAMS_H
//-----------------------------------------------------------------------------
#include "bd_types.h"
#include "misc.h"
#include "jsoncpp/json/json.h"
//-----------------------------------------------------------------------------
class TMcaParams {
public:
    TMcaParams ();
    TMcaParams (const TMcaParams &other);
    TMcaParams operator= (const TMcaParams &other);
    bool operator== (const TMcaParams &other) const;
    bool operator!= (const TMcaParams &other) const;
    void Clear ();
//-----------------------------------------------------------------------------
    Json::Value LoadFromJson (Json::Value jMCA);
    Json::Value AsJson () const;
//-----------------------------------------------------------------------------
    uint GetChannels () const;
    void SetChannels (uint uiChannels);

    double GetMinVoltage () const;
    void SetMinVoltage (double dMinVoltage);

    double GetMaxVoltage () const;
    void SetMaxVoltage (double dMaxVoltage);
protected:
    void AssignAll (const TMcaParams &other);
private:
    uint m_uiChannels;
    double m_dMinVoltage;
    double m_dMaxVoltage;
};
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
