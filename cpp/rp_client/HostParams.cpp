//---------------------------------------------------------------------------

#pragma hdrstop

#include "HostParams.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#include <IniFiles.hpp>
#include "misc.h"
static const string SectionHostParams = "Host Parameters";
static const string KeyAddress = "Host Address";
static const string KeyPort = "Host Port";
//---------------------------------------------------------------------------
THostParams::THostParams ()
{
	Clear ();
}
//---------------------------------------------------------------------------
THostParams::THostParams (const THostParams &other)
{
	AssignAll (other);
}
//---------------------------------------------------------------------------
THostParams THostParams::operator= (const THostParams &other)
{
	AssignAll (other);
	return (*this);
}
//---------------------------------------------------------------------------
bool THostParams::operator== (const THostParams &other) const
{
	if (GetAddress() != other.GetAddress())
		return (false);
	if (GetPort()    != other.GetPort())
		return (false);
	return (true);
}
//---------------------------------------------------------------------------
bool THostParams::operator!= (const THostParams &other) const
{
	return (!(*this == other));
}
//---------------------------------------------------------------------------
void THostParams::Clear ()
{
	SetAddress ("");
	SetHostPort    (0);
}
//---------------------------------------------------------------------------
void THostParams::AssignAll (const  THostParams &other)
{
	SetAddress (other.GetAddress());
	SetHostPort    (other.GetPort());
}
//---------------------------------------------------------------------------
string THostParams::GetAddress () const
{
	return (m_strAddress);
}
//---------------------------------------------------------------------------
void THostParams::SetAddress (const string &str)
{
	m_strAddress = str;
}
//---------------------------------------------------------------------------
uint THostParams::GetPort () const
{
	return (m_uiPort);
}
//---------------------------------------------------------------------------
void THostParams::SetHostPort (uint uiPort)
{
	m_uiPort = uiPort;
}
//---------------------------------------------------------------------------
bool THostParams::LoadFromIni (const string &strIni)
{
	TIniFile *ini=NULL;
	wstring w;
	UnicodeString us1, us2, us3;
	bool f;

	try {
		try {
			ini = new TIniFile (strIni.c_str());
			UnicodeString ustr = ini->ReadString (SectionHostParams.c_str(), KeyAddress.c_str(), GetAddress().c_str());
			SetAddress (UnicodeToString (ustr));
			SetHostPort    (ini->ReadInteger (SectionHostParams.c_str(), KeyPort.c_str(), GetPort()));
			f = true;
		}
		catch (exception &e) {
			m_strErr = e.what();
			f = false;
		}
	}
	__finally {
		if (ini != NULL) {
			delete ini;
		}
	}
	return (f);
}
//---------------------------------------------------------------------------
bool THostParams::SaveToIni (const string &strIni)
{
	TIniFile *ini=NULL;
	wstring w;
	UnicodeString us1, us2, us3;
	bool f;

	try {
		try {
			ini = new TIniFile (strIni.c_str());
			ini->WriteString (SectionHostParams.c_str(), KeyAddress.c_str(), GetAddress().c_str());
			ini->WriteInteger(SectionHostParams.c_str(), KeyPort.c_str(), GetPort());
			f = true;
		}
		catch (exception &e) {
			m_strErr = e.what();
			f = false;
		}
	}
	__finally {
		if (ini != NULL) {
			delete ini;
		}
	}
	return (f);
}
//---------------------------------------------------------------------------
//	m_host.LoadFromIni (GetAppIni (g_strAppIni));
//---------------------------------------------------------------------------
