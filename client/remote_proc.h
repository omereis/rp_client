/******************************************************************************\
|                              remote_proc.h                                   |
\******************************************************************************/
#ifndef  __REMOTE_PROC_H
#define	__REMOTE_PROC_H
//-----------------------------------------------------------------------------
using namespace std;
#include "jsoncpp/json/json.h"
#include <string>
//-----------------------------------------------------------------------------
class TRemoteProcessing {
public:
    TRemoteProcessing ();
    TRemoteProcessing (const TRemoteProcessing &other);

    void Clear ();

    void SetOnOff (bool fOnOff);
    void SetOnOff (Json::Value &jOnOff);
    bool GetOnOff () const;
    void SetHost (const string &strHost);
    void SetHost (Json::Value &jHost);
    string GetHost() const;
    void SetPort (int nPort);
    void SetPort (Json::Value &jPort);
    int GetPort () const;

    Json::Value LoadFromJson(Json::Value &jSetup);
    Json::Value AsJson();
    void AssignAll (const TRemoteProcessing &other);
protected:
private:
    bool m_fOnOff;
    string m_strHost;
    int m_nPort;
};
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
