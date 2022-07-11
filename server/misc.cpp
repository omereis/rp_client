/******************************************************************************\
|                                  misc.cpp                                    |
\******************************************************************************/
//---------------------------------------------------------------------------

#include "misc.h"
#include "trim.h"

#include <string.h>

using namespace std;

//-----------------------------------------------------------------------------

std::string StringifyJson (const Json::Value &val)
{
	std::string strJson;
	Json::FastWriter fastWriter;
	
	strJson = fastWriter.write(val);
	return (strJson);
}
//-----------------------------------------------------------------------------

std::string read_file_as_line (const std::string &strFile)
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

std::string ReadFileAsString (const std::string &strFile)
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

string ToLower (const std::string &str)
{
	string strLower;

	for (int n=0 ; n < str.size() ; n++)
		strLower += tolower(str[n]);
	return (strLower);
}
//-----------------------------------------------------------------------------

bool ReadVectorFromFile (const std::string &strFile, TFloatVec &vDate)
{
	bool fRead;

	try {
		vDate.clear();
		char *szLine, szBuf[1024];
		FILE *file = fopen (strFile.c_str(), "r");
		if (file != NULL) {
			while ((szLine = fgets(szBuf, 1024, file)) != NULL) {
				float r = (float) atof (szLine);
				vDate.push_back (r);
			}
			fRead = true;
		}
		else {
			fprintf (stderr, "Could not open file %s\n", strFile.c_str());
			fRead = false;
		}
	}
	catch (std::exception &exp) {
		fprintf (stderr, "Runtime error in 'ReadVectorFromFile':\n%s\n", exp.what());
		fRead = false;
	}
	return (fRead);
}
//---------------------------------------------------------------------------

std::string ReplaceAll(const std::string &strSrc, const std::string& from, const std::string& to)
{
	size_t start_pos = 0;
	std::string str (strSrc);

	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return (str);
}

//-----------------------------------------------------------------------------
float VectorAverage (const TFloatVec &vec)
{
    TFloatVec::const_iterator i;
    double dAverage=0;

	if (vec.size() > 0) {
    	for (i=vec.begin() ; i != vec.end() ; i++)
       		dAverage += (double) *i;
    	dAverage /= (double) vec.size();
	}
    return (dAverage);
}

//-----------------------------------------------------------------------------
void PrintRuntimeError (std::exception &exp, const std::string &strProcedure, FILE *file)
{
	PrintRuntimeError (exp, strProcedure.c_str(), file);
}

//-----------------------------------------------------------------------------
void PrintRuntimeError (const char szMessage[], FILE *file)
{
	fprintf (file, "Runtime error %s:\n", szMessage);
}

//-----------------------------------------------------------------------------
void PrintRuntimeError (std::exception &exp, const char szProcedure[], FILE *file)
{
	fprintf (file, "Runtime error in %s:\n%s", szProcedure, exp.what());
}

#ifdef	_RED_PITAYA_HW

//-----------------------------------------------------------------------------
std::string GetHardwareTriggerName (rp_acq_trig_src_t trigger_src)
{
	std::string strSource;

    if (trigger_src == RP_TRIG_SRC_DISABLED) {
		//fprintf (stderr, "RP_TRIG_SRC_DISABLED\n");
		strSource = "RP_TRIG_SRC_DISABLED";
	}
    else if (trigger_src == RP_TRIG_SRC_NOW)
		strSource = "RP_TRIG_SRC_NOW";
    else if (trigger_src == RP_TRIG_SRC_CHA_PE)
		strSource = "RP_TRIG_SRC_CHA_PE";
    else if (trigger_src == RP_TRIG_SRC_CHA_NE)
		strSource = "RP_TRIG_SRC_CHA_NE";
    else if (trigger_src == RP_TRIG_SRC_CHB_PE)
		strSource = "RP_TRIG_SRC_CHB_PE";
    else if (trigger_src == RP_TRIG_SRC_CHB_NE)
		strSource = "RP_TRIG_SRC_CHB_NE";
    else if (trigger_src == RP_TRIG_SRC_EXT_PE)
		strSource = "RP_TRIG_SRC_EXT_PE";
    else if (trigger_src == RP_TRIG_SRC_EXT_NE)
		strSource = "RP_TRIG_SRC_EXT_NE";
    else if (trigger_src == RP_TRIG_SRC_AWG_PE)
		strSource = "RP_TRIG_SRC_AWG_PE";
    else if (trigger_src == RP_TRIG_SRC_AWG_NE)
		strSource = "RP_TRIG_SRC_AWG_NE";
	else
		strSource = "";
	//fprintf (stderr, "GetHardwareTriggerName, returning '%s'\n", strSource.c_str());
	return (strSource);
}

//-----------------------------------------------------------------------------
std::string GetHardwareTriggerName ()
{
	rp_acq_trig_src_t trigger_src;

	rp_AcqGetTriggerSrc(&trigger_src);
	std::string strSource = GetHardwareTriggerName (trigger_src);
	return (strSource);
}

//-----------------------------------------------------------------------------
void PrintTriggerSource (const char sz[]/*=NULL*/)
{
	rp_acq_trig_src_t trigger_src = RP_TRIG_SRC_AWG_NE;

	if (strlen(sz) > 0)
		fprintf (stderr, "%s\n", sz);
	std::string strSource = GetHardwareTriggerName ();

	//fprintf (stderr, "trigger source: %s\n", strSource.c_str());
}
#endif
//---------------------------------------------------------------------------
