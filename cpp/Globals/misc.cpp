//---------------------------------------------------------------------------

#pragma hdrstop

#include "misc.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#include <vcl.h>
using namespace std;
std::string GetAppIni (const std::string &strIni)
{
	wstring wstr, wstrDir;
	std::string strApp;

	wstr = std::wstring (Application->ExeName.c_str(), Application->ExeName.Length());
	size_t sFound = wstr.find_last_of(L"/\\");
	if (sFound > 0)
		wsDir = wstr.substr (0, sFound);
	else
		wsDir = "C:\\Temp";
	strApp = std::string (wsDir.begin(), wsDir.end()) + "\\" + strIni;
	return (strApp);
}
//---------------------------------------------------------------------------
std::string WStringToString (const wstring &wstr)
{
	string str;

	str = std::string (wstr.begin(), wstr.end());
	return (str);
}
//---------------------------------------------------------------------------
std::string UnicodeToString (const UnicodeString ustr)
{
	wstring w = wstring (ustr.c_str(), ustr.Length());
	string str = WStringToString (w);
	return (str);
}
//---------------------------------------------------------------------------
