/******************************************************************************\
|                                rp_setup.cpp                                  |
\******************************************************************************/

#include "rp_setup.h"
#include "bd_types.h"
#include "misc.h"

#include "trim.h"

#include <mutex>

static const char *g_szPackageSize = "package_size";
static const int g_nDefaultPackageSize = 1024;
//-----------------------------------------------------------------------------
// TRedPitayaSetup
//-----------------------------------------------------------------------------
TRedPitayaSetup::TRedPitayaSetup ()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TRedPitayaSetup::TRedPitayaSetup (const TRedPitayaSetup &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
TRedPitayaSetup TRedPitayaSetup::operator= (const TRedPitayaSetup &other)
{
    AssignAll (other);
    return (*this);
}
//-----------------------------------------------------------------------------
bool TRedPitayaSetup::operator== (const TRedPitayaSetup &other) const
{
    return (true);
}
//-----------------------------------------------------------------------------
bool TRedPitayaSetup::operator!= (const TRedPitayaSetup &other) const
{
    return (!(*this == other));
}
//-----------------------------------------------------------------------------
void TRedPitayaSetup::Clear ()
{
/*
	rp_Init();
*/
    m_trigger.Clear();
    m_sampling.Clear ();
    m_mca_params.Clear();
	m_trapez.Clear();
    SetSamplingOnOff (false);
    SetMcaOnOff (false);
    SetPsdOnOff (false);
    SetBackground (0.1);
    SetPackageSize (g_nDefaultPackageSize);
    SetPreTriggerNs (100);
	SetMcaTimeLimit (0);
	//SetMcaStartTime (0);
	//SetMcaStopTime (0);
	SetMcaValid (false);
	m_remote_proc.Clear();
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::AssignAll (const TRedPitayaSetup &other)
{
    SetTrigger (other.GetTrigger());
    SetSampling (other.GetSampling());
    m_mca_params = other.m_mca_params;
	m_trapez = TTRapezInfo (other.m_trapez);
    SetSamplingOnOff (other.GetSamplingOnOff ());
    SetMcaOnOff (other.GetMcaOnOff ());
    SetPsdOnOff (other.GetPsdOnOff ());
    SetBackground (other.GetBackground());
    SetPackageSize (other.GetPackageSize());
    SetPreTriggerNs (other.GetPreTriggerNs());
	SetMcaTimeLimit (other.GetMcaTimeLimit ());

	SetMcaValid (other.IsMcaValid ());
	m_remote_proc = TRemoteProcessing (other.m_remote_proc);
}
//-----------------------------------------------------------------------------
TRedPitayaTrigger TRedPitayaSetup::GetTrigger() const
{
    return (m_trigger);
}
//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetTrigger (const TRedPitayaTrigger &trigger)
{
    m_trigger = trigger;
}
//-----------------------------------------------------------------------------
TRedPitayaSampling TRedPitayaSetup::GetSampling () const
{
    return (m_sampling);
}
//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetSampling (const TRedPitayaSampling &sampling)
{
    m_sampling = sampling;
}
//-----------------------------------------------------------------------------
TMcaParams TRedPitayaSetup::GetMcaParams () const
{
    return (m_mca_params);
}
//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaParams (const TMcaParams &mca_params)
{
    m_mca_params = mca_params;
}
//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::AppsAsJson()
{
    Json::Value jAppsMca;

    jAppsMca["mca"] = McaAsJson();
    jAppsMca["psd"] = "no psd yet";
    return (jAppsMca);
}
//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::McaAsJson()
{
    Json::Value jMCA;

    jMCA["mca"] = m_mca_params.AsJson();
    return (jMCA);
}

//-----------------------------------------------------------------------------
double TRedPitayaSetup::GetSamplingPeriod() const
{
	return (m_sampling.GetSamplingPeriod ());
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::LoadFromJson(const string &strFile)
{
	Json::Value root, jResult;//, jSampling, jTrigger;
	Json::Reader reader;
    string strJson = ReadFileAsString (strFile);

    Clear ();
    if (reader.parse (strJson, root)) {
        jResult = UpdateFromJson(root);
    }
	fprintf (stderr, "'TRedPitayaSetup::LoadFromJson' end:\n");
    return (true);
}

//-----------------------------------------------------------------------------
std::string DoubleAsString (double dValue)
{
    char sz[20];

    sprintf (sz, "%.3f", dValue);
    std::string str = std::string(sz);
    return (str);
}

//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::TriggerAsJson()
{
    Json::Value jSetup;

    jSetup["trigger"] = m_trigger.AsJson();
    return (jSetup);
}

//-----------------------------------------------------------------------------
std::string TRedPitayaSetup::PreTriggerAsString (double dPreTrigger)
{
	char *sz = new char[50];
	sprintf (sz, "%.2f", dPreTrigger);
	std::string s (sz);
	delete[] sz;
	return (s);
}

//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::AsJson()
{
    Json::Value jSetup;

    jSetup["sampling"] = m_sampling.AsJson();
    jSetup["trigger"] = m_trigger.AsJson();
    jSetup["mca"] = m_mca_params.AsJson();
    jSetup["background"] = DoubleAsString (GetBackground());
    jSetup["package_size"] = to_string (GetPackageSize());
	jSetup["pre_trigger_ns"] = PreTriggerAsString (GetPreTriggerNs());// to_string (GetPreTriggerNs());
	jSetup["trapez"] = m_trapez.AsJson();
	jSetup["remote_processing"] = m_remote_proc.AsJson();
    return (jSetup);
}

//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::UpdateFromJson(Json::Value &jSetup, bool fUpdateHardware)
{
    Json::Value jNewSetup;
	string str;

    try {
		std::string str = StringifyJson (jSetup);
        jNewSetup["sampling"] = m_sampling.UpdateFromJson(jSetup["sampling"]);
        jNewSetup["trigger"] = m_trigger.UpdateFromJson(jSetup["trigger"]);
        jNewSetup["mca"] = m_mca_params.LoadFromJson (jSetup["mca"]);
		jNewSetup["trapez"] = m_trapez.LoadFromJson (jSetup["trapez"]);

		str = StringifyJson (jSetup);
		str = StringifyJson (jSetup["remote_processing"]);
		jNewSetup["remote_processing"] = m_remote_proc.LoadFromJson(jSetup["remote_processing"]);
		jNewSetup["remote_processing"] = 
		str = StringifyJson (m_remote_proc.AsJson());

		if (!jSetup["background"].isNull()) {
			std::string strBkgnd = jSetup["background"].asString();
			double d = stod (strBkgnd);
			SetBackground (d);
		}
        if (!jSetup[g_szPackageSize].isNull()) {
			int nPkgSize=GetPackageSize();
            if (jSetup[g_szPackageSize].isInt())
                nPkgSize = jSetup[g_szPackageSize].asInt();
			else if (jSetup[g_szPackageSize].isString())
                nPkgSize = atoi (jSetup[g_szPackageSize].asString().c_str());
			SetPackageSize (nPkgSize);
		}
        SetPreTriggerNs (jSetup["pre_trigger_ns"]);
		if (fUpdateHardware) {
#ifdef	_RED_PITAYA_HW
			//fprintf (stderr, "=================++++++++++++++++++++==================\n");
			//fprintf (stderr, "rp_setup.cpp:178\n");
			//fprintf (stderr, "=================++++++++++++++++++++==================\n");
			m_trigger.SetHardwareTrigger();
#else
			;
#endif
        	//m_trigger.Print("Called from rp_setup.cpp (UpdateFromJson): 177");
		}
    }
    catch (std::exception &exp) {
		jNewSetup["error"] = exp.what();
    }
    return (jNewSetup);
}

//-----------------------------------------------------------------------------
size_t TRedPitayaSetup::GetTrapezSize() const
{
	return (m_trapez.GetTrapezSize());
}

//-----------------------------------------------------------------------------
TDoubleVec TRedPitayaSetup::GetTrapez() const
{
	TDoubleVec vTrapez;

	m_trapez.GetTrapez (vTrapez);
	if (vTrapez.size() == 0)
		m_trapez.GenerateTrapez (vTrapez);
	return (vTrapez);
}

//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::GetTrapezAsJson()
{
	return (m_trapez.AsJson());
}
//-----------------------------------------------------------------------------
bool TRedPitayaSetup::SaveToJson (const std::string &strFile)
{
    bool fLoad;

    try {
        string strJson = StringifyJson (AsJson());
        FILE *file = fopen (strFile.c_str(), "w+");
        fprintf (file, "%s\n", strJson.c_str());
        fclose (file);
        fLoad = true;
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error in 'SaveToJson':\n%s\n", exp.what());
        fLoad = false;
    }
    return (fLoad);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetSamplingOnOff (bool fSampling)
{
    mutex mtx;

	//fprintf (stderr, "TRedPitayaSetup::SetSamplingOnOff, fSampling:%d\n", (int) fSampling);
    mtx.lock();
    m_fSamplingOnOff = fSampling;
    mtx.unlock();
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::GetSamplingOnOff () const
{
    mutex mtx;

    mtx.lock();
    bool f = m_fSamplingOnOff;
    mtx.unlock();
    return (f);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaOnOff (Json::Value &jMcaCmd)
{
	if (!jMcaCmd.isNull()) {
		string strMcaTime, str = StringifyJson (jMcaCmd);
		if (jMcaCmd["mca_time"].isNull() == false) {
			string s = StringifyJson (jMcaCmd["mca_time"]);
			strMcaTime = jMcaCmd["mca_time"].asString();
		}
		SetMcaTimeLimit (strMcaTime);
		if (jMcaCmd["mca"].isString())
			SetMcaOnOff (jMcaCmd["mca"].asString());
		else if (jMcaCmd["mca"].isBool())
			SetMcaOnOff (jMcaCmd["mca"].asBool());
	}
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaOnOff (const string &strOnOff)
{
	try {
		bool f = to_bool (strOnOff);
		SetMcaOnOff (f);
	}
	catch (std::exception &exp) {
		fprintf (stderr, "Runtime error on 'SetMcaOnOff:\n%s\n", exp.what());
	}
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaOnOff (const string &strOnOff, const string &strTime)
{
	try {
		SetMcaOnOff (strOnOff);
		SetMcaTimeLimit (strTime);
	}
	catch (std::exception &exp) {
		fprintf (stderr, "Runtime error on 'SetMcaOnOff:\n%s\n", exp.what());
	}
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaOnOff (bool fMca)
{
    mutex mtx;

    mtx.lock();
	if ((fMca == true) && (m_fMcaOnOff == false)) // starting
		SetMcaStartTime ();
	else if ((fMca == false) && (m_fMcaOnOff == true)) // ending
		SetMcaStopTime();
    m_fMcaOnOff = fMca;
    mtx.unlock();
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::GetMcaOnOff () const
{
    mutex mtx;

    mtx.lock();
    bool f = m_fMcaOnOff;
    mtx.unlock();
    return (f);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPsdOnOff (bool fPsd)
{
    mutex mtx;

    mtx.lock();
    m_fPsdOnOff = fPsd;
    mtx.unlock();
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::GetPsdOnOff () const
{
    mutex mtx;

    mtx.lock();
    bool f = m_fPsdOnOff;
    mtx.unlock();
    return (f);
}

//-----------------------------------------------------------------------------
float TRedPitayaSetup::GetTriggerLevel ()
{
    std::string strTrigger = GetTrigger().GetLevel();
    float fLevel;

    try {
        fLevel = (float) stod (strTrigger);
    }
    catch (std::exception &exp) {
        fLevel = -1;
        fprintf (stderr, "Runtime error in 'GetTriggerLevel':\n%s\n", exp.what());
    }
    return (fLevel);
}

//-----------------------------------------------------------------------------
double TRedPitayaSetup::GetBackground() const
{
    return (m_dBackground);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetBackground (const string &strBackground)
{
	try {
		double dBackground = stod (strBackground);
		SetBackground (dBackground);
	}
    catch (std::exception &exp) {
		PrintRuntimeError (exp, "'TRedPitayaSetup::SetBackground'");
    }
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetBackground (double dBackground)
{
    m_dBackground = dBackground;
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetBackgroundFromJson (Json::Value jBkgnd)
{
    try {
        string str = jBkgnd.asString();
        SetBackground (stod(str));
    }
    catch (std::exception &exp) {
		fprintf (stderr, "Runtime error on 'TRedPitayaSetup::SetBackgroundFromJson'\n%s\n", exp.what());
    }
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::TriggerNow ()
{
	m_trigger.TriggerNow ();
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::GetMcaSpectrum (TFloatVec &vSpectrum)
{
    m_mca_params.GetSpectrum (vSpectrum);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::ResetMcaSpectrum ()
{
	m_mca_params.ResetSpectrum ();
}

#include "rp_server.h"
//-----------------------------------------------------------------------------
Json::Value TRedPitayaSetup::HandleBackground (Json::Value &jBkgnd, const TDoubleVec &vSignal)
//Json::Value TRedPitayaSetup::HandleBackground (Json::Value &jBkgnd, const TFloatVec &vSignal)
{
	Json::Value jSetup;
	Json::Value jSet = jBkgnd["set"];


	if (!jBkgnd.isNull()) {
		if (!jSet.isNull()) {
			SetBackground (jSet.asString());
		}
		if (jBkgnd.isString()) {
			string strCommand = ToLower (jBkgnd.asString());
			if (strCommand == "measure") {
                TFloatVec vPulse;
        		//if (GetNextPulse (vPulse)) {
                //if (ReadHardwareSamples (*this, vPulse)) {
                    SetBackground (VectorAverage (vPulse));
                //}
            }
#ifdef	_RED_PITAYA_HW
#endif
		}
	}
    jSetup["background"] = DoubleAsString (GetBackground());
	return (jSetup);
}

//-----------------------------------------------------------------------------
int TRedPitayaSetup::GetPackageSize() const
{
    return (m_nPackageSize);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPackageSize (int nPackageSize)
{
    m_nPackageSize = nPackageSize;
}

//-----------------------------------------------------------------------------
float TRedPitayaSetup::GetPreTriggerNs() const
{
    return (m_fPreTriggerNs);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPreTriggerNs (float fDelayNs)
{
    m_fPreTriggerNs = fDelayNs;
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPreTriggerNs (const std::string &str)
{
    try {
        double dPre = std::stod (str);
        SetPreTriggerNs (dPre);
    }
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error setting pre trigger:\n%s\n", exp.what());
    }
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetPreTriggerNs (Json::Value jPreTrigger)
{
    if (!jPreTrigger.isNull()) {
        if (jPreTrigger.isString())
            SetPreTriggerNs (jPreTrigger.asString());
        else if (jPreTrigger.isDouble())
            SetPreTriggerNs (jPreTrigger.asDouble());
    }
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::NewPulse (const TPulseInfoVec &vPulsesInfo)
{
	if (GetMcaOnOff()) {
    	m_mca_params.NewPulse (vPulsesInfo);
		double dMcaMeasureTime, dTimeLimit = GetMcaTimeLimit ();
		if (dTimeLimit > 0) {
			dMcaMeasureTime = GetMcaMeasureTime ();
			if (dMcaMeasureTime >= dTimeLimit) {
				SetMcaOnOff (false);
				printf ("MCA Stopped\n");
			}
		}
	}
}

//-----------------------------------------------------------------------------
int TRedPitayaSetup::GetMcaPulses() const
{
	return (m_mca_params.GetMcaPulses());
}

/*
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
*/

//-----------------------------------------------------------------------------
double TRedPitayaSetup::CalculateBackground (const TDoubleVec &vSource)
//double TRedPitayaSetup::CalculateBackground (const TFloatVec &vSource)
{
	TDoubleVec vPulse;

	SubVector (vSource, GetPreTriggerNs(), vPulse);
	double dAvg = VectorAverage (vPulse);
	//double dStd = VectorStdDev (vPulse, dAvg);
	//SetBackground (dAvg - 3 * dStd);
	SetBackground (dAvg);
	return (GetBackground());
}

//-----------------------------------------------------------------------------
TFloatVec::const_iterator TRedPitayaSetup::FindFirstTrigger(const TFloatVec &vSignal)
{
	TFloatVec::const_iterator i, iFound = vSignal.end();
	int idx=0;
	double dTrigger = m_trigger.GetTriggerLevel ();

	for (i=vSignal.begin() ; (i != vSignal.end()) && (iFound == vSignal.end()) ; i++, idx++)
		if (*i < dTrigger)
			iFound = i;
	return (iFound);
}

//-----------------------------------------------------------------------------
double TRedPitayaSetup::GetSignalBackground(const TFloatVec &vSignal)
{
	double dSum = 0;
	int n;
	TFloatVec::const_iterator i, iTrigger = FindFirstTrigger(vSignal);

	if (iTrigger != vSignal.end()) {
		for (n=0 ; (n < 5) && (iTrigger != vSignal.begin()) ; n++)
			iTrigger--;
		if (iTrigger != vSignal.begin()) {
			double dAvg = VectorAverage (vSignal.begin(), iTrigger);
			double dStdDev = VectorStdDev (vSignal.begin(), iTrigger, dAvg);
			SetBackground (dAvg - 3 * dStdDev);
		}
	}
	return (GetBackground());
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaTimeLimit (const string &strTime)
{
	try {
		string str (strTime);
		double d;
		try {
			str = trimString (str);
			d = atof  (str.c_str());
		}
		catch (...) {
			d = 0;
		}
		SetMcaTimeLimit (d);
	}
    catch (std::exception &exp) {
        fprintf (stderr, "Runtime error in 'SetMcaTimeLimit':\n%s\n", exp.what());
    }
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaTimeLimit (double dMcaSeconds)
{
	m_dMcaTimeLimit = dMcaSeconds;
}

//-----------------------------------------------------------------------------
double TRedPitayaSetup::GetMcaTimeLimit () const
{
	return (m_dMcaTimeLimit);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaStartTime (const chrono_clock &crnStart)
{
	m_crnMcaStart = crnStart;
	 SetMcaValid (true);
}

//-----------------------------------------------------------------------------
double TRedPitayaSetup::GetMcaMeasureTime () const
{
	double dTime = 0;
	chrono_clock end;

	if (IsMcaValid ()) {
		if (GetMcaOnOff ())
			end = std::chrono::system_clock::now();
		else
			end = GetMcaStopTime();
		std::chrono::duration<double> elapsed_seconds = end - GetMcaStartTime();
		dTime = elapsed_seconds.count();
	}
	else {
		//printf ("MCA not valid\n");
		dTime = 0;
	}
	return (dTime);
}

//-----------------------------------------------------------------------------
chrono_clock TRedPitayaSetup::GetMcaStopTime() const
{
	return (m_crnMcaStop);
}

//-----------------------------------------------------------------------------
chrono_clock TRedPitayaSetup::SetMcaStopTime (chrono_clock clk)
{
	m_crnMcaStop = clk;
	return (m_crnMcaStop);
}
//-----------------------------------------------------------------------------
chrono_clock TRedPitayaSetup::SetMcaStopTime ()
{
	SetMcaStopTime (std::chrono::system_clock::now());
	return (GetMcaStartTime());
}

//-----------------------------------------------------------------------------
chrono_clock TRedPitayaSetup::GetMcaStartTime() const
{
	return (m_crnMcaStart);
}

//-----------------------------------------------------------------------------
chrono_clock TRedPitayaSetup::SetMcaStartTime ()
{
	SetMcaStartTime (std::chrono::system_clock::now());
	return (GetMcaStartTime());
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::IsFilterOn() const
{
	return (m_trapez.GetOnOff());
}

//-----------------------------------------------------------------------------
TRemoteProcessing TRedPitayaSetup::GetRemoteProc() const
{
	return (m_remote_proc);
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::GetRemoteProc(const TRemoteProcessing &remote_proc)
{
	m_remote_proc.AssignAll(remote_proc);
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::IsRemoteProcessingOn() const
{
	return (m_remote_proc.GetOnOff ());
}

//-----------------------------------------------------------------------------
size_t TRedPitayaSetup::GetMcaCount() const
{
    return (m_mca_params.GetCount());
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::ClearMca()
{
	m_mca_params.ClearMca ();
}

//-----------------------------------------------------------------------------
double TRedPitayaSetup::GetMcaMin() const
{
	return (m_mca_params.GetMin());
}

//-----------------------------------------------------------------------------
double TRedPitayaSetup::GetMcaMax() const
{
	return (m_mca_params.GetMax());
}

//-----------------------------------------------------------------------------
double TRedPitayaSetup::GetMcaMinVoltage() const
{
	return (m_mca_params.GetMinVoltage());
}

//-----------------------------------------------------------------------------
double TRedPitayaSetup::GetMcaMaxVoltage() const
{
	return (m_mca_params.GetMaxVoltage());
}

//-----------------------------------------------------------------------------
void TRedPitayaSetup::SetMcaValid (bool fValid)
{
	m_fMcaValid = fValid;
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::IsMcaValid () const
{
	return (m_fMcaValid);
}

//-----------------------------------------------------------------------------
#ifdef	_RED_PITAYA_HW
bool TRedPitayaSetup::LoadFromHardware (bool fInit)
{
	bool fLoad;
	rp_acq_trig_src_t dir;

	try {
		rp_AcqGetTriggerSrc(&dir);
    	m_trigger.LoadFromHardware();
		fLoad = true;
	}
	catch (std::exception &exp) {
		fprintf (stderr, "Runtime error in 'TRedPitayaSetup::LoadFromHardware':\n%s", exp.what());
		fLoad = false;
	}
	return (fLoad);
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::SetHardwareTrigger()
{
	return (SetHardwareTrigger(m_trigger));
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::SetHardwareTrigger(const TRedPitayaTrigger &trigger)
{
	bool fSet;

	try {
		m_trigger = trigger;
		m_trigger.SetHardwareTrigger ();
		fSet = true;
	}
	catch (std::exception exp) {
		fprintf (stderr, "Runtime error in 'TRedPitayaSetup::SetHardwareTrigger':\n%s", exp.what());
		fSet = false;
	}
	return (fSet);
}

//-----------------------------------------------------------------------------
bool TRedPitayaSetup::PrintHardwareSetup (FILE *file)
{
	return (m_trigger.Print ("PrintHardwareSetup", file));
	//m_trigger.PrintHardwareSetup (file);
}

//-----------------------------------------------------------------------------
rp_acq_decimation_t TRedPitayaSetup::GetHardwareDecimation() const
{
	return (m_sampling.GetHardwareDecimation());
}

//-----------------------------------------------------------------------------
rp_channel_trigger_t TRedPitayaSetup::GetHardwareTriggerChannel() const
{
	return (m_trigger.GetHardwareTriggerChannel());
}

//-----------------------------------------------------------------------------
float TRedPitayaSetup::GetHardwareTriggerLevel() const
{
	return (m_trigger.GetHardwareTriggerLevel());
}

//-----------------------------------------------------------------------------
rp_acq_sampling_rate_t TRedPitayaSetup::GetHardwareSamplingRate() const
{
	return (m_sampling.GetHardwareSamplingRate());
}

//-----------------------------------------------------------------------------
rp_acq_trig_src_t TRedPitayaSetup::GetHardwareTriggerSource() const
{
	return (m_trigger.GetHardwareTriggerSource());
}
#endif
//-----------------------------------------------------------------------------
