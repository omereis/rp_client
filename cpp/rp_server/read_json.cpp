/******************************************************************************\
|                               read_json.cpp                                  |
\******************************************************************************/

#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "bd_types.h"

#include "jsoncpp/json/json.h"

#include "trim.h"
#include "misc.h"
#include "rp_setup.h"

using namespace std;


void ReadJson (char *szFile);
string ExtractDir (const string &strPath, string &strFile);
string compose_json_filename (char *szFile);
string ReadFileAsString (string &strFile);
void read_setup (TRedPitayaSetup &rp_setup);
void write_setup(TRedPitayaSetup &rp_setup);
string GetJsonFile ();

string strMenu[] =   {"1. Read Setup",
                    "2. Write Setup",
                    "3. Quit"};
//-----------------------------------------------------------------------------
int main (void) {
    bool fQuit=false;
    char *szFile, szBuf[1024];
    TRedPitayaSetup rp_setup;

    rp_setup.LoadFromJson("rp_setup.json");
    while (!fQuit) {
        for (int n=0 ; n < 3 ; n++)
            printf ("%s\n", strMenu[n].c_str());
        printf ("Enter Selection ");
        szFile = fgets(szBuf, 1024, stdin);
        int nOption;
        if (szFile == NULL)
            fQuit = true;
		else {
        	string strFile (szFile);
        	strFile = trimString(strFile);
        	if (strFile.length() == 0)
            	fQuit = true;
            else {
                try {
                    nOption = stoi (strFile);
                    if (nOption == 1)
                        read_setup (rp_setup);
                    else if (nOption == 2)
                        write_setup(rp_setup);
                    else if (nOption == 3)
                        fQuit = true;
                }
                catch (std::exception &exp) {
                    fprintf (stderr, "Runtime error:\n%s\n", exp.what());
                    fQuit = true;
                }
            }
		}
    }
/*
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
*/
}
//-----------------------------------------------------------------------------
void ReadJson (char *szFile)
{
/*
	Json::Value root, sampling, rate;
	Json::Reader reader;
	string strFile = compose_json_filename (szFile);
    string strJson = ReadFileAsString (strFile);

    if (reader.parse (strJson, root))
        if (!root["read_setup"].isNull()) {

  
      }
*/
/*
    if (reader.parse (strJson, root)) {
        sampling = root["sampling"];
        if (!root["sampling"].isNull()) {
            rate = sampling["rate"];
            if (!rate.isNull()) {
                printf ("rate sampling: %s\n", rate.asString().c_str());
            }
        }
    }
*/
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

void read_setup (TRedPitayaSetup &rp_setup)
{
    Json::Value jSetup = rp_setup.AsJson();
    printf ("Setup: %s\n", StringifyJson(jSetup).c_str());
}
//-----------------------------------------------------------------------------

void write_setup(TRedPitayaSetup &rp_setup)
{
    string strFile = GetJsonFile ();

    if (strFile.length() > 0) {
    	Json::Value root, sampling, rate;
	    Json::Reader reader;
        string strJson = read_file_as_line (strFile);

        if (reader.parse (strJson, root)) {
            if (!root.isNull()) {
                rp_setup.UpdateFromJson(root);
				printf ("\nWritten setup:\n'%s'\n", StringifyJson (rp_setup.AsJson()).c_str());
            }
        }
    }
}
//-----------------------------------------------------------------------------

string GetJsonFile ()
{
    string strFile;
    char *szFile, szBuf[1024];

    printf ("Enter JSON file name [.json]");
    szFile = fgets(szBuf, 1024, stdin);
    if (szFile != NULL) {
        	strFile = string (szFile);
        	strFile = trimString(strFile);
        	if (strFile.length() > 0) {
                size_t s = strFile.find_last_of (".");
                if (s == string::npos)
                    strFile += ".json";
            }
    }
    return (strFile);
}
//-----------------------------------------------------------------------------
