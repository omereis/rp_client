/******************************************************************************\
|                               read_json.cpp                                  |
\******************************************************************************/

#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include <vector>

#include "jsoncpp/json/json.h"

#include "trim.h"

using namespace std;

#ifndef	TStringVec
typedef vector<string> TStringVec;
#endif

void ReadJson (char *szFile);
string ExtractDir (const string &strPath, string &strFile);
string compose_json_filename (char *szFile);
string ReadFileAsString (string &strFile);

int main (void) {
    bool fQuit=false;
    char *szFile, szBuf[1024];

    while (!fQuit) {
        printf ("Enter JSON file name [.json]");
        szFile = fgets(szBuf, 1024, stdin);
        if (szFile == NULL)
            fQuit = true;
		else {
        	string strFile (szFile);
        	strFile = trimString(strFile);
        	if (strFile.length() == 0)
            	fQuit = true;
        	else
            	ReadJson (szFile);
		}
		szFile = NULL;
    }
}
//-----------------------------------------------------------------------------
void ReadJson (char *szFile)
{
	Json::Value root, sampling, rate;
	Json::Reader reader;
	string strFile = compose_json_filename (szFile);
    string strJson = ReadFileAsString (strFile);
    //string strJson = ReadFileAsString (compose_json_filename (szFile));

    if (reader.parse (strJson, root)) {
        sampling = root["sampling"];
        if (!root["sampling"].isNull()) {
            rate = sampling["rate"];
            if (!rate.isNull()) {
                printf ("rate sampling: %s\n", rate.asString().c_str());
            }
        }
            //if ()
    }
}
//-----------------------------------------------------------------------------

string compose_json_filename (char *szFile)
{
    string strFile, strPath (szFile);
    string strDir = ExtractDir (strPath, strFile);
    size_t sPos = strFile.find_last_of(".");
    if (sPos == string::npos)
        strFile += ".json";
	strPath = strDir + "/" + strFile;
	return (strPath);
    //printf ("Path to file is '%s'\n", strPath.c_str());
}
//-----------------------------------------------------------------------------

string ExtractDir (const string &strPath, string &strFile)
{
    size_t sPos;
    string strDir;

    sPos = strPath.find_last_of  ("/");
    if (sPos == string::npos) {
        strDir = ".";
        strFile = strPath;
    }
    else {
        strDir = strPath.substr (0, sPos - 1);
        strFile = strPath.substr(sPos+1, strPath.length() - sPos);
    }
	strFile = trimString(strFile);
    return (strDir);
}
//-----------------------------------------------------------------------------

string ReadFileAsString (string &strFile)
{
	FILE *file=NULL;
	char *szLine, szBuf[1024];
	TStringVec vstr;
	TStringVec::iterator i;
	string strLine;

	try {
		file = fopen (strFile.c_str(), "r");
		while ((szLine = fgets (szBuf, 1024, file)) != NULL) {
			strLine = string(szLine);
			strLine = trimString(strLine);
			if (strLine.length() > 0)
				vstr.push_back (strLine);
		}
        strLine = "";
        for (i=vstr.begin() ; i != vstr.end() ; i++)
            strLine += trimString (*i);
		fclose (file);
	}
	catch (std::exception &exp) {
		fprintf (stderr, "Runtime error in ReadJson:\n%s\n", exp.what());
        if (file != NULL)
		    fclose (file);
        strLine = "";
	}
    return (strLine);
}
//-----------------------------------------------------------------------------
