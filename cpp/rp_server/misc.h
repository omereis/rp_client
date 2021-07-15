//---------------------------------------------------------------------------

#ifndef miscH
#define miscH
//---------------------------------------------------------------------------
#include <string>
#include "jsoncpp/json/json.h"

//---------------------------------------------------------------------------
std::string StringifyJson (const Json::Value &val);
std::string ReadFileAsString (const std::string &strFile);
std::string ToLower (const std::string &str);
#endif
