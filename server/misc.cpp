/******************************************************************************\
|                                  misc.cpp                                    |
\******************************************************************************/
//---------------------------------------------------------------------------

#include "misc.h"
#include "trim.h"

#include <string.h>
#include <cmath>

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
bool ReadVectorFromFile (const std::string &strFile, TDoubleVec &vData)
{
	bool fRead;

	try {
		vData.clear();
		char *szLine, szBuf[1024];
		FILE *file = fopen (strFile.c_str(), "r");
		if (file != NULL) {
			while ((szLine = fgets(szBuf, 1024, file)) != NULL) {
				float r = (float) atof (szLine);
				vData.push_back (r);
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

//-----------------------------------------------------------------------------
bool ReadVectorFromFile (const std::string &strFile, TFloatVec &vData)
{
	bool fRead;

	try {
		vData.clear();
		char *szLine, szBuf[1024];
		FILE *file = fopen (strFile.c_str(), "r");
		if (file != NULL) {
			while ((szLine = fgets(szBuf, 1024, file)) != NULL) {
				float r = (float) atof (szLine);
				vData.push_back (r);
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
double VectorAverage (const TDoubleVec &vec)
{
	return (VectorAverage (vec.begin(), vec.end()));
}

//-----------------------------------------------------------------------------
double VectorAverage (const TFloatVec &vec)
{
	return (VectorAverage (vec.begin(), vec.end()));
}

//-----------------------------------------------------------------------------
double VectorStdDev (const TFloatVec &vec, double dAvg)
{
	return (VectorStdDev (vec.begin(), vec.end(), dAvg));
/*
	TFloatVec::const_iterator i;
	double dStd = 0;

	if (vec.size() > 0) {
		for (i=vec.begin(), dStd=0 ; i != vec.end() ; i++)
			dStd += pow(*i - dAvg, 2.0);
			//dStd += (*i - dAvg, 2.0) * (*i - dAvg, 2.0);
		dStd = sqrt (dStd / vec.size());
	}
	return (dStd);
*/
}

//-----------------------------------------------------------------------------
double VectorAverage (TDoubleVec::const_iterator iBegin, TDoubleVec::const_iterator iEnd)
{
    TDoubleVec::const_iterator i;
    double dAverage=0;
	int n;

    for (i=iBegin, n=0 ; i != iEnd ; i++, n++)
		dAverage += (double) *i;
	if (n > 0)
    	dAverage /= (double) n;
    return (dAverage);
}

//-----------------------------------------------------------------------------
double VectorAverage (TFloatVec::const_iterator iBegin, TFloatVec::const_iterator iEnd)
{
    TFloatVec::const_iterator i;
    double dAverage=0;
	int n;

    for (i=iBegin, n=0 ; i != iEnd ; i++, n++)
		dAverage += (double) *i;
	if (n > 0)
    	dAverage /= (double) n;
    return (dAverage);
}

//-----------------------------------------------------------------------------
double VectorStdDev (TFloatVec::const_iterator iBegin, TFloatVec::const_iterator iEnd, double dAvg)
{
	TFloatVec::const_iterator i;
	double dStd = 0;
	int n=0;

	for (i=iBegin, dStd=0, n=0 ; i != iEnd ; i++, n++)
		dStd += pow(*i - dAvg, 2.0);
	if (n > 0)
		dStd = sqrt (dStd / (double) n);
	return (dStd);
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
void PrintVector (const TDoubleVec &vBuffer, const char szName[])
{
	TDoubleVec::const_iterator i;
	FILE *file = fopen (szName, "a+");
	fprintf (file, "------------------------------------\n");
	for (i=vBuffer.begin() ; i < vBuffer.end() ; i++)
		fprintf (file, "%g\n", *i);
	fprintf (file, "++++++++++++++++++++++++++++++++++++\n");
	fclose (file);
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

#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>

bool to_bool(const std::string &strSrc)
{
	std::string str(strSrc);
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::istringstream is(str);
	bool b;
	is >> std::boolalpha >> b;
	return b;
}

//---------------------------------------------------------------------------
size_t convolution(const TDoubleVec &vSource, const TDoubleVec &vKernel, TDoubleVec &vResult, size_t sz)
{
	int nconv;
	int i, j, i1;
	float tmp;
	//float *C;

	//allocated convolution array
	try {
		if (sz == 0)
			nconv = vSource.size() + vKernel.size() - 1;
		else
			nconv = sz;
    	vResult.resize (nconv, 0);
    	for (i=0 ; i < nconv ; i++) {
			i1 = i;
			tmp = 0.0;
			for (j=0 ; j < vKernel.size() ; j++) {
				if ((i1 >= 0) && (i1 < vSource.size()))
					tmp = tmp + (vSource[i1] * vKernel[j]);
				i1 = i1 - 1;
				vResult[i] = tmp;
			}
    	}
	}
	catch (std::exception &e) {
		fprintf (stderr, "Runtime in convolution:\n%s\n", e.what());
	}
    return (vResult.size());
}

//---------------------------------------------------------------------------
size_t consv(const TFloatVec &vSource, const TDoubleVec &vKernel, TDoubleVec &vResult)
{
	int nconv;
	int i, j, i1;
	float tmp;
	//float *C;

	//allocated convolution array	
	nconv = vSource.size() + vKernel.size() - 1;
    vResult.resize (nconv, 0);
    for (i=0 ; i < nconv ; i++) {
		i1 = i;
		tmp = 0.0;
		for (j=0 ; j < vKernel.size() ; j++) {
			if ((i1 >= 0) && (i1 < vSource.size()))
				tmp = tmp + (vSource[i1] * vKernel[j]);
			i1 = i1 - 1;
			vResult[i] = tmp;
		}
    }
    return (vResult.size());
}

//-----------------------------------------------------------------------------
size_t SubVector (const TDoubleVec &vSource, int nLength, TDoubleVec &vSub)
{
	TDoubleVec::const_iterator iSrc;
	TDoubleVec::iterator iDest;
	int n;

	vSub.resize (nLength);
	for (iSrc=vSource.begin(), n=0, iDest=vSub.begin() ; (iSrc != vSource.end()) && (n < nLength) ; iSrc++, n++, iDest++)
		*iDest = *iSrc;
	return (vSub.size());
}

//-----------------------------------------------------------------------------
size_t SubVector (const TFloatVec &vSource, int nLength, TFloatVec &vSub)
{
	TFloatVec::const_iterator iSrc;
	TFloatVec::iterator iDest;
	int n;

	vSub.resize (nLength);
	for (iSrc=vSource.begin(), n=0, iDest=vSub.begin() ; (iSrc != vSource.end()) && (n < nLength) ; iSrc++, n++, iDest++)
		*iDest = *iSrc;
	return (vSub.size());
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
