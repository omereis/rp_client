/******************************************************************************\
|                            remote_proc.cpp                                   |
\******************************************************************************/
#include "remote_proc.h"

//-----------------------------------------------------------------------------
TRemoteProcessing::TRemoteProcessing ()
{
    Clear ();
}

//-----------------------------------------------------------------------------
TRemoteProcessing::TRemoteProcessing (const TRemoteProcessing &other)
{
    AssignAll (other);
}

//-----------------------------------------------------------------------------
void TRemoteProcessing::Clear ()
{
    SetOnOff (false);
    SetHost ("");
    SetPort (0);
}

//-----------------------------------------------------------------------------
void TRemoteProcessing::SetOnOff (bool fOnOff)
{
    m_fOnOff = fOnOff;
}

//-----------------------------------------------------------------------------
bool TRemoteProcessing::GetOnOff () const
{
    return (m_fOnOff);
}

//-----------------------------------------------------------------------------
void TRemoteProcessing::SetHost (const string &strHost)
{
    m_strHost = strHost;
}

//-----------------------------------------------------------------------------
string TRemoteProcessing::GetHost() const
{
    return (m_strHost);
}

//-----------------------------------------------------------------------------
void TRemoteProcessing::SetPort (int nPort)
{
    m_nPort = nPort;
}

//-----------------------------------------------------------------------------
int TRemoteProcessing::GetPort () const
{
    return (m_nPort);
}

#include "misc.h"
//-----------------------------------------------------------------------------
Json::Value TRemoteProcessing::LoadFromJson(Json::Value &jSetup)
{
	Json::Value j;

    try {
		string str;
		str = StringifyJson (jSetup);
        if (!jSetup.isNull()) {
            SetOnOff (jSetup["remote_on_off"]);
            SetHost (jSetup["remote_host"]);
            SetPort (jSetup["remote_port"]);
			j = AsJson();
        }
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error in 'TRemoteProcessing::LoadFromJson':%s\n", exp.what());
		j["error"] = exp.what();
    }
    return (j);
}

#include "misc.h"
//-----------------------------------------------------------------------------
Json::Value TRemoteProcessing::AsJson()
{
    Json::Value jRemote;
	string s;

    try {
        jRemote["remote_on_off"] = GetOnOff();
        jRemote["remote_host"] = GetHost();
        jRemote["remote_port"] = GetPort ();
		s = StringifyJson (jRemote);
    }
    catch (std::exception &exp) {
        jRemote["error"] = exp.what ();
    }
	s = StringifyJson (jRemote);
    return (jRemote);
}

//-----------------------------------------------------------------------------
void TRemoteProcessing::AssignAll (const TRemoteProcessing &other)
{
    SetOnOff (other.GetOnOff ());
    SetHost (other.GetHost());
    SetPort (other.GetPort ());
}

//-----------------------------------------------------------------------------
void TRemoteProcessing::SetOnOff (Json::Value &jOnOff)
{
    try {
        if (jOnOff.isBool())
            SetOnOff (jOnOff.asBool());
        else
            SetOnOff ((bool )jOnOff.asInt());
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error in 'TRemoteProcessing::SetOnOff'%s\n", exp.what());
        SetOnOff (false);
    }
}

//-----------------------------------------------------------------------------
void TRemoteProcessing::SetHost (Json::Value &jHost)
{
    try {
        SetHost (jHost.asString());
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error in 'TRemoteProcessing::SetOnOff'%s\n", exp.what());
        SetHost ("");
    }
}

//-----------------------------------------------------------------------------
void TRemoteProcessing::SetPort (Json::Value &jPort)
{
    try {
		string str;
		str = StringifyJson (jPort);
		str = jPort.asString();
        SetPort (atoi(str.c_str()));
        //SetPort (jPort.asInt());
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error in 'TRemoteProcessing::SetPort'%s\n", exp.what());
        SetPort (0);
    }
}
