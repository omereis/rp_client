/******************************************************************************\
|                              mca_info.cpp                                    |
\******************************************************************************/
#include "mca_info.h"
#include "trim.h"
#include <math.h>
#include <unistd.h>

//-----------------------------------------------------------------------------
extern const char *g_szSampling;
extern const char *g_szStatus;

//-----------------------------------------------------------------------------
TMcaInfo::TMcaInfo (TMcaParams *pParams)
{
    Clear (pParams);
}

//-----------------------------------------------------------------------------
TMcaInfo::TMcaInfo (const TMcaInfo &other)
{
    AssignAll (other);
}

//-----------------------------------------------------------------------------
void TMcaInfo::Clear (TMcaParams *pParams)
{
    m_pParams = pParams;
    if (m_pParams != NULL)
        m_pParams->ClearParams ();
    SetStartTime ();
	SetCount (0);
	SetValid (false);
}

//-----------------------------------------------------------------------------
void TMcaInfo::AssignAll (const TMcaInfo &other)
{
    m_pParams = other.m_pParams;
}

//-----------------------------------------------------------------------------
void TMcaInfo::SetStartTime (chrono_clock clkStart)
{
    m_crnMcaStart = clkStart;
}

//-----------------------------------------------------------------------------
void TMcaInfo::SetStartTime ()
{
    SetStartTime (std::chrono::system_clock::now());
}

//-----------------------------------------------------------------------------
chrono_clock TMcaInfo::GetStartTime () const
{
    return (m_crnMcaStart);
}

//-----------------------------------------------------------------------------
void TMcaInfo::NewPulses (TPulseInfoVec &piVec)
{
    TPulseInfoVec::iterator iPulse;

    for (iPulse=piVec.begin() ; iPulse != piVec.end() ; iPulse++)
        NewPulse (*iPulse);
}

//-----------------------------------------------------------------------------
void TMcaInfo::NewPulse (TPulseInfo &pulse_info)
{
	int idx;
	double dIndex;

    if (m_vSpectrum.size() == 0)
        SetSpectrum (m_pParams->GetChannels());
	//if (pulse_info.GetMaxVal () > m_pParams->GetMinVoltage())
		//dIndex = 0;
	//else
	dIndex = (double) m_pParams->GetChannels() * fabs(pulse_info.GetMaxVal ());
    dIndex /= fabs((m_pParams->GetMaxVoltage() - m_pParams->GetMinVoltage()));
	idx = (int) (dIndex + 0.5);
    if ((idx >= 0) && (idx < (int) m_vSpectrum.size())) {
		//if (idx < m_vSpectrum.size())  {
			//idx = m_vSpectrum.size() - 1;
        	//int n = m_vSpectrum[idx];
        	m_vSpectrum[idx] = (m_vSpectrum[idx] + 1);
		//}
	}
    IncreaseCount ();
    if (GetCount() == 0) {
        SetMin(pulse_info.GetMinVal ());
        SetMax(pulse_info.GetMaxVal ());
    }
}

//-----------------------------------------------------------------------------
void  TMcaInfo::IncreaseCount ()
{
    m_nCount++;
}

//-----------------------------------------------------------------------------
size_t TMcaInfo::GetCount() const
{
	return (m_nCount);
}

//-----------------------------------------------------------------------------
void TMcaInfo::SetCount (size_t nCount)
{
	m_nCount = nCount;
}

//-----------------------------------------------------------------------------
void TMcaInfo::SetMin (double dMin)
{
    m_dMin = dMin;
}

//-----------------------------------------------------------------------------
void TMcaInfo::SetMax (double dMax)
{
    m_dMax = dMax;
}

//-----------------------------------------------------------------------------
double TMcaInfo::GetMin () const
{
    return (m_dMin);
}

//-----------------------------------------------------------------------------
double TMcaInfo::GetMax () const
{
    return (m_dMax);
}

//-----------------------------------------------------------------------------
void TMcaInfo::HandleMcaCommands (Json::Value &jMCA)
{
    try {
	    if (!jMCA["mca_time"].isNull()) {
			string sTime = jMCA["mca_time"].asString();
			double d = atof (sTime.c_str());
			printf ("\nMCA Required Time: %s, %g seconds\n\n", sTime.c_str(), d);
		}
		string strMca = jMCA["mca_time"].asString();
		printf ("MCA Time: %s\n", strMca.c_str());
		SetMcaOnOff (ToLower (jMCA["mca"].asString()), jMCA["mca_time"].asString());
    }
    catch (std::exception &exp) {
        string strJson = StringifyJson (jMCA);
        fprintf (stderr, "Runtime error in 'TMcaInfo::HandleMcaCommands':\n%s\n", exp.what());
        fprintf (stderr, "JSON argument::\n%s\n", strJson.c_str());
    }
}

//-----------------------------------------------------------------------------
void TMcaInfo::SetMcaOnOff (const string &strOnOff, const string &strTime)
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
void TMcaInfo::SetMcaOnOff (bool fMca)
{
	if ((fMca == true) && (GetMcaOnOff () == false)) { // starting
		SetStartTime ();
		SetValid (true);
		SetCount (0);
	}
	else if ((fMca == false) && (GetMcaOnOff () == true)) // ending
		SetStopTime();
    m_fMcaOnOff = fMca;
}

//-----------------------------------------------------------------------------
void TMcaInfo::SetMcaOnOff (const string &strOnOff)
{
	SetMcaOnOff (to_bool (strOnOff));
}

//-----------------------------------------------------------------------------
bool TMcaInfo::GetMcaOnOff () const
{
    return (m_fMcaOnOff);
}

/*
//-----------------------------------------------------------------------------
chrono_clock TMcaInfo::SetMcaStopTime (chrono_clock clk)
{
	m_crnMcaStop = clk;
	return (m_crnMcaStop);
}

//-----------------------------------------------------------------------------
chrono_clock TMcaInfo::SetMcaStopTime ()
{
	SetMcaStopTime (std::chrono::system_clock::now());
	return (GetMcaStartTime());
}
*/

//-----------------------------------------------------------------------------
void TMcaInfo::SetSpectrum (uint uiChannels)
{
	m_pParams->SetChannels (uiChannels);
	m_vSpectrum.resize(uiChannels, 0);
}

//-----------------------------------------------------------------------------
void TMcaInfo::ResetSpectrum()
{
	m_vSpectrum.resize (m_pParams->GetChannels(), 0);
}

//-----------------------------------------------------------------------------
void TMcaInfo::SetMcaTimeLimit (const string &strTime)
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
double TMcaInfo::GetMcaTimeLimit () const
{
	return (m_dMcaTimeLimit);
}

//-----------------------------------------------------------------------------
void TMcaInfo::SetMcaTimeLimit (double dMcaSeconds)
{
	m_dMcaTimeLimit = dMcaSeconds;
}

//-----------------------------------------------------------------------------
chrono_clock TMcaInfo::SetStopTime (chrono_clock clk)
{
	m_crnMcaStop = clk;
	return (m_crnMcaStop);
}

//-----------------------------------------------------------------------------
chrono_clock TMcaInfo::GetStopTime() const
{
	return (m_crnMcaStop);
}

//-----------------------------------------------------------------------------
chrono_clock TMcaInfo::SetStopTime ()
{
	SetStopTime (std::chrono::system_clock::now());
	return (GetStartTime());
}

//-----------------------------------------------------------------------------
Json::Value TMcaInfo::ReadMca ()
{
    Json::Value jMCA, jMcaData;
    TFloatVec vSpectrum;
    TDoubleVec::iterator i;

	try {
    	for (i=m_vSpectrum.begin() ; i < m_vSpectrum.end() ; i++) {
        	jMcaData.append (*i);
    	}
		jMCA["mca_count"] = (double) GetCount();//rp_setup.GetMcaCount();
		jMCA["mca_min"] = GetMin();//rp_setup.GetMcaMin();
		jMCA["mca_max"] = GetMax();//rp_setup.GetMcaMax();
		jMCA["mca_min_v"] = m_pParams->GetMinVoltage();//.rp_setup.GetMcaMinVoltage();
		jMCA["mca_max_v"] = m_pParams->GetMaxVoltage();//rp_setup.GetMcaMaxVoltage();
		jMCA["mca_time"] = GetMcaMeasureTime ();
		jMCA["mca_data"] = jMcaData;
		jMCA["channels"] = m_pParams->GetChannels ();
		string s;
		s = StringifyJson (jMCA);
		s += "";
	}
    catch (std::exception exp) {
		fprintf (stderr, "Runtime error in 'ReadMca':\n%s\n", exp.what());
		jMCA["error"] = exp.what();
	}
    return (jMCA);
}


//-----------------------------------------------------------------------------
Json::Value TMcaInfo::HandleMCA(Json::Value &jMCA)
{
	return (ReadMca());
/*
    std::string strResult;
    Json::Value jResult;

    try {
        string str = jMCA.asString();
        if (str == g_szStatus)
            jResult[g_szStatus] = SafeGetMca();
        else if (str == "true") {
            SafeSetMca(true, rp_setup);
            jResult[g_szStatus] = SafeGetMca();
        }
        else if (str == "false") {
            SafeSetMca(false, rp_setup);
            jResult[g_szStatus] = SafeGetMca();
        }
        else if (str == g_szReset) {
            SafeResetMca(rp_setup);
            jResult[g_szStatus] = g_szReset;
			jResult["pulse_count"] = to_string(g_qDebug.size());
        }
        else if (str == g_szReadMca)
            SafeReadMca (rp_setup, jResult);
        else if (str == g_szSaveMCA)
            jResult = SaveMCA (rp_setup);
        else
            jResult[g_szMCA] = "Command Unknown";
    }
    catch (std::exception &exp) {
        jResult[g_szError] = exp.what();
    }
    return (jResult);
*/
}

//-----------------------------------------------------------------------------
double TMcaInfo::GetMcaMeasureTime () const
{
	double dTime = 0;
	chrono_clock end;

	if (GetValid ()) {
		if (GetMcaOnOff ())
			end = std::chrono::system_clock::now();
		else
			end = GetStopTime();
		std::chrono::duration<double> elapsed_seconds = end - GetStartTime();
		dTime = elapsed_seconds.count();
	}
	else {
		dTime = 0;
	}
	return (dTime);
}

//-----------------------------------------------------------------------------
void TMcaInfo::SetValid (bool fValid)
{
	m_fMcaValid = fValid;
}
	
//-----------------------------------------------------------------------------
bool TMcaInfo::GetValid () const
{
	return (m_fMcaValid);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Json::Value TMcaInfo::UpdateFromJson (Json::Value jMCA)
{
	return (m_pParams->UpdateFromJson (jMCA));
}

//-----------------------------------------------------------------------------
Json::Value TMcaInfo::AsJson()
{
	return (m_pParams->AsJson());
}

//-----------------------------------------------------------------------------
void TMcaInfo::StopMca()
{
	SetMcaOnOff (false);
}

//-----------------------------------------------------------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
TMutexMcaInfo::TMutexMcaInfo (TMcaInfo *pMcaInfo, mutex *pMutex)
 {
    m_pMcaInfo = pMcaInfo;
    m_pMutex = pMutex;
}

//-----------------------------------------------------------------------------
void TMutexMcaInfo::NewPulses (TPulseInfoVec &piVec)
{
    while (!m_pMutex->try_lock())
        usleep (0.1);
    m_pMcaInfo->NewPulses (piVec);
    m_pMutex->unlock();
}

//-----------------------------------------------------------------------------
void TMutexMcaInfo::HandleMcaCommands (Json::Value &jSampling)
{
    while (!m_pMutex->try_lock())
        usleep (0.1);
    m_pMcaInfo->HandleMcaCommands (jSampling);
    m_pMutex->unlock();
}

//-----------------------------------------------------------------------------
void TMutexMcaInfo::ResetSpectrum()
{
    while (!m_pMutex->try_lock())
        usleep (0.1);
    m_pMcaInfo->ResetSpectrum();
    m_pMutex->unlock();
}

//-----------------------------------------------------------------------------
Json::Value TMutexMcaInfo::ReadMca ()
{
	Json::Value jMca;

    while (!m_pMutex->try_lock())
        usleep (0.1);
    jMca = m_pMcaInfo->ReadMca();
    m_pMutex->unlock();
	return (jMca);
}

//-----------------------------------------------------------------------------
void TMutexMcaInfo::SetMcaOnOff (const string &strOnOff, const string &strTime)
{
    while (!m_pMutex->try_lock())
        usleep (0.1);
	m_pMcaInfo->SetMcaOnOff (strOnOff, strTime);
    m_pMutex->unlock();
}

//-----------------------------------------------------------------------------
bool TMutexMcaInfo::GetMcaOnOff ()
{
	bool fOnOff;

    while (!m_pMutex->try_lock())
        usleep (0.1);
    fOnOff = m_pMcaInfo->GetMcaOnOff();
    m_pMutex->unlock();
	return (fOnOff);
}

//-----------------------------------------------------------------------------
size_t TMutexMcaInfo::GetCount() const
{
	size_t nCount;

    while (!m_pMutex->try_lock())
        usleep (0.1);
    nCount = m_pMcaInfo->GetCount();
    m_pMutex->unlock();
	return (nCount);
}

//-----------------------------------------------------------------------------
double TMutexMcaInfo::GetMcaMeasureTime () const
{
	double dTime;

    while (!m_pMutex->try_lock())
        usleep (0.1);
    dTime = m_pMcaInfo->GetMcaMeasureTime ();
    m_pMutex->unlock();
	return (dTime);
}

//-----------------------------------------------------------------------------
Json::Value TMutexMcaInfo::HandleMCA (Json::Value &jMCA)
{
	Json::Value jReply;

    while (!m_pMutex->try_lock())
        usleep (0.1);
    jReply = m_pMcaInfo->HandleMCA (jMCA);
    m_pMutex->unlock();
	return (jReply);
}

//-----------------------------------------------------------------------------
Json::Value TMutexMcaInfo::UpdateFromJson (Json::Value jMCA)
{
	Json::Value jReply;

    while (!m_pMutex->try_lock())
        usleep (0.1);
    jReply = m_pMcaInfo->UpdateFromJson (jMCA);
    m_pMutex->unlock();
	return (jReply);
}

//-----------------------------------------------------------------------------
Json::Value TMutexMcaInfo::AsJson()
{
	Json::Value jReply;

    while (!m_pMutex->try_lock())
        usleep (0.1);
    jReply = m_pMcaInfo->AsJson();
    m_pMutex->unlock();
	return (jReply);
}

//-----------------------------------------------------------------------------
double TMutexMcaInfo::GetMcaTimeLimit () const
{
	double dTime;

    while (!m_pMutex->try_lock())
        usleep (0.1);
    dTime = m_pMcaInfo->GetMcaTimeLimit ();
    m_pMutex->unlock();
	return (dTime);
}

//-----------------------------------------------------------------------------
void TMutexMcaInfo::StopMca()
{
    while (!m_pMutex->try_lock())
        usleep (0.1);
    m_pMcaInfo->StopMca();
    m_pMutex->unlock();
}
//-----------------------------------------------------------------------------
