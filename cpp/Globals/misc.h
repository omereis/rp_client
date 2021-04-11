//---------------------------------------------------------------------------

#ifndef miscH
#define miscH
//---------------------------------------------------------------------------
#include <string>
#include <ustring.h>
//---------------------------------------------------------------------------
std::string GetAppIni (const std::string &strIni);
std::string WStringToString (const std::wstring &wstr);
std::string UnicodeToString (const UnicodeString ustr);
#endif
