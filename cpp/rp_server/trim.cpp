/******************************************************************************\
|                                 trim.cpp                                     |
source:
https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
\******************************************************************************/
#include <iostream>
#include <string>

using std::cout; using std::cin;
using std::endl; using std::string;

string trimLeftRight(string& str, string& chars);

string leftTrim(string &str, string &chars)
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

string rightTrim(string &str, string &chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

string trimString(string& str)
{
	string s (" ");

	string strToTrim = trimLeftRight(str, s);
	s = string("\t");
	strToTrim = trimLeftRight(strToTrim, s);
	s = string("\n");
	strToTrim = trimLeftRight(strToTrim, s);
	return (strToTrim);
}

string trimLeftRight(string& str, string& chars)
{
    string strTemp = rightTrim(str, chars);
    strTemp = leftTrim(strTemp, chars);
	return (strTemp);
}
