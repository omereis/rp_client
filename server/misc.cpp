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

//-----------------------------------------------------------------------------
std::string BoolToString (bool f)
{
	std::string str = f ? "true" : "false";
	return (str);
}

//-----------------------------------------------------------------------------
void PrintBool (const char *szTitle, bool fValue)
{
	fprintf (stderr, "%s: %s\n", szTitle, fValue ? "true" : "false");
}

/*
//-----------------------------------------------------------------------------
float VectorAverage (const TFloatVec &vPulse)
{
	TFloatVec::const_iterator i;
	double dSum = 0;
	float fAverage = 0;

	if (vPulse.size() > 0) {
		i = vPulse.begin();
		dAverage = *i;
		for (++i, i != vPulse.end() ; i++)
			dAverage += *i;
		fAverage = (float) dSum / vPulse.size();
	}
	return (fAverage);
}
*/

//-----------------------------------------------------------------------------
EPulseDir PulseDirFromString (const std::string &strSource)
{
	std::string s (strSource);
	s = ToLower (s);
	s = trimString (s);
	std::string strPulseDir = s;
	EPulseDir pulse_dir;

	if ((strPulseDir == "positive") || (strPulseDir == "up"))
		pulse_dir = EPS_UP;
	else if ((strPulseDir == "negative") || (strPulseDir == "down"))
		pulse_dir = EPS_DOWN;
	else
		pulse_dir = EPS_Undecided;
	return (pulse_dir);
}

//-----------------------------------------------------------------------------
std::string PulseDirName (EPulseDir pulse_dir)
{
	std::string strPulseDir;

	if (pulse_dir == EPS_UP)
		strPulseDir = "positive";
	else if (pulse_dir == EPS_DOWN)
		strPulseDir = "negative";
	else
		strPulseDir = "mixed";
	return (strPulseDir);
}

//-----------------------------------------------------------------------------
void PrintVector (const TFloatVec &vBuffer, const char szName[])
{
	TFloatVec::const_iterator i;
	FILE *file = fopen (szName, "a+");
	fprintf (file, "------------------------------------\n");
	for (i=vBuffer.begin() ; i < vBuffer.end() ; i++)
		fprintf (file, "%g\n", *i);
	fprintf (file, "++++++++++++++++++++++++++++++++++++\n");
	fclose (file);
}

//-----------------------------------------------------------------------------
void GetVectorMinMax (const TFloatVec &vPulse, float &fMin, float &fMax)
{
    TFloatVec::const_iterator i;

    i = vPulse.begin();
    fMin = fMax = *i;
    for (i++ ; i != vPulse.end() ; i++) {
        fMin = min (fMin, *i);
        fMax = max (fMax, *i);
    }
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

//-----------------------------------------------------------------------------
rp_acq_decimation_t GetHardwareDecimationFromName(const std::string &strDecimationSrc)
{
	std::string strDecimation = ToLower (strDecimationSrc);
	rp_acq_decimation_t decimation;

	if (strDecimation == "1")
		decimation = RP_DEC_1;
	if (strDecimation == "8")
		decimation = RP_DEC_8;
	else if (strDecimation == "8")
		decimation = RP_DEC_64;
	else if (strDecimation == "8")
		decimation = RP_DEC_1024;
	else if (strDecimation == "8")
		decimation = RP_DEC_8192;
	else if (strDecimation == "8")
		decimation = RP_DEC_65536;
	else
		decimation = RP_DEC_1;
	return (decimation);
}

/*
//---------------------------------------------------------------------------
bool ReadHardwareSamples (const TRedPitayaSetup &rp_setup, TFloatVec &vPulse)
{
    uint32_t buff_size = 16384;
    float *buff = (float *)malloc(buff_size * sizeof(float));
	int16_t* auiBuffer = (int16_t*) calloc (buff_size, sizeof (auiBuffer[0]));
	rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;
	bool fTrigger=false, fTimeout=false;
	clock_t cStart;
	double dDiff;

    //rp_AcqReset();
	//rp_AcqSetDecimation(RP_DEC_1);
	rp_AcqSetDecimation(rp_setup.GetHardwareDecimation());

	float fLevel = rp_setup.GetHardwareTriggerLevel();
	//fprintf (stderr, "Trigger level (#642): %g\n", fLevel);
	rp_AcqSetTriggerLevel(rp_setup.GetHardwareTriggerChannel(), rp_setup.GetHardwareTriggerLevel());
	//rp_AcqSetTriggerLevel(RP_T_CH_1, -15e-3);
	rp_AcqSetSamplingRate (rp_setup.GetHardwareSamplingRate());
	//rp_AcqSetSamplingRate (RP_SMP_125M);
    rp_AcqSetTriggerDelay(0);
    rp_AcqStart();
	usleep(1);
	rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
	cStart = clock();
	while((fTrigger == false) && (fTimeout == false)){
		rp_AcqGetTriggerState(&state);
		if (state == RP_TRIG_STATE_TRIGGERED)//{
			fTrigger =true;
		else
			fprintf (stderr, "No Trigger\r");
		dDiff = (clock() - cStart);
		dDiff /= (double) CLOCKS_PER_SEC;
		if (dDiff >= 3)
			fTimeout = true;
	}
    if (fTrigger) {
	    uint32_t uiTriggerPos, uiLen=buff_size, n;
        TFloatVec::iterator i;
	    rp_AcqGetWritePointerAtTrig (&uiTriggerPos);
	    rp_AcqGetDataV (RP_CH_1, uiTriggerPos, &uiLen, buff);
	    //rp_AcqGetDataRaw (RP_CH_1, uiTriggerPos, &uiLen, auiBuffer);
        vPulse.resize (buff_size, 0);
        for (n=0, i=vPulse.begin() ; n < (int) buff_size ; n++, i++) //{
			*i = buff[n];
			//*i = auiBuffer[n];
    }
    else
    	printf ("Timeout: %g\n", dDiff);
    rp_AcqStop();
    return (fTrigger);
}
*/
#endif
//---------------------------------------------------------------------------
