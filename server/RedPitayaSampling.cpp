/******************************************************************************\
|                           RedPitayaSampling.cpp                              |
\******************************************************************************/

#include "RedPitayaSampling.h"
#include "bd_types.h"
#include "misc.h"

#include <mutex>
//
// TRedPitayaSampling
//-----------------------------------------------------------------------------
TRedPitayaSampling::TRedPitayaSampling ()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TRedPitayaSampling::TRedPitayaSampling (const TRedPitayaSampling &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
TRedPitayaSampling TRedPitayaSampling::operator= (const TRedPitayaSampling &other)
{
    AssignAll (other);
    return (*this);
}
//-----------------------------------------------------------------------------
bool TRedPitayaSampling::operator== (const TRedPitayaSampling &other) const
{
    if (GetRate()       != other.GetRate())
        return (false);
    if (GetDecimation() != other.GetDecimation())
        return (false);
    if (GetBufferSize() != other.GetBufferSize())
        return (false);
    if (GetSignalPoints() != other.GetSignalPoints())
        return (false);
    return (true);
}
//-----------------------------------------------------------------------------
bool TRedPitayaSampling::operator!= (const TRedPitayaSampling &other) const
{
    return (!(*this == other));
}
//-----------------------------------------------------------------------------
void TRedPitayaSampling::Clear ()
{
    SetRate ("125000000");
    SetDecimation ("1");
    SetBufferSize (1000);
    SetSignalPoints (200);
}
//-----------------------------------------------------------------------------
void TRedPitayaSampling::AssignAll (const TRedPitayaSampling &other)
{
    SetRate (other.GetRate());
    SetDecimation (other.GetDecimation());
    SetBufferSize (other.GetBufferSize());
    SetSignalPoints (other.GetSignalPoints());
}
//-----------------------------------------------------------------------------
string TRedPitayaSampling::GetRate () const
{
    return (m_strRate);
}
//-----------------------------------------------------------------------------
void TRedPitayaSampling::SetRate (const string &strRate)
{
    m_strRate = strRate;
}
//-----------------------------------------------------------------------------
string TRedPitayaSampling::GetDecimation () const
{
    return (m_strDecimation);
}
//-----------------------------------------------------------------------------
void  TRedPitayaSampling::SetDecimation (const string &str)
{
    m_strDecimation = str;
}
//-----------------------------------------------------------------------------
Json::Value TRedPitayaSampling::LoadFromJson (Json::Value jSampling)
{
    Json::Value jNew;

    try {
        Json::Value jRate = jSampling["rate"], jDecimation = jSampling["decimation"], jBufferSize = jSampling["buffer_size"];

        if (!jRate.isNull())
            SetRate (jRate.asString());
        if (!jDecimation.isNull())
            SetDecimation (jDecimation.asString());
        if (!jBufferSize.isNull())
            SetBufferSize (jBufferSize.asString());
        jNew = AsJson();
    }
    catch (std::exception &exp) {
        jNew["error"] = exp.what();
    }
    return (jNew);
}
//-----------------------------------------------------------------------------

Json::Value TRedPitayaSampling::AsJson()
{
    Json::Value jSampling;

    jSampling["rate"] = GetRate();
    jSampling["decimation"] = GetDecimation();
    jSampling["buffer_length"] = std::to_string (GetBufferSize ());
    jSampling["signal_points"] = std::to_string (GetSignalPoints());
    return (jSampling);
}
//-----------------------------------------------------------------------------

Json::Value TRedPitayaSampling::UpdateFromJson(Json::Value &jSetup)
{
    Json::Value jNew;
    
    try {
        SetRate (jSetup["rate"].asString());
        SetDecimation (jSetup["decimation"].asString());
        if (!jSetup["buffer_length"].isNull())
            SetBufferSize(jSetup["buffer_length"].asString());
        if (!jSetup["signal_points"].isNull())
            SetBufferSize(jSetup["signal_points"].asString());
        jNew = AsJson();
    }
    catch (std::exception exp) {
        jNew["error"] = exp.what();
    }
    return (jNew);
}

//-----------------------------------------------------------------------------
int TRedPitayaSampling::GetBufferSize () const
{
    return (m_nBufferSize);
}

//-----------------------------------------------------------------------------
void TRedPitayaSampling::SetBufferSize (const std::string &strSize) {
    SetBufferSize(std::stoi(strSize));
}

//-----------------------------------------------------------------------------
void TRedPitayaSampling::SetBufferSize (int nSize)
{
    m_nBufferSize = nSize;
}

//-----------------------------------------------------------------------------
int TRedPitayaSampling::GetSignalPoints () const
{
    return (m_nSignalPoints);
}

//-----------------------------------------------------------------------------
void TRedPitayaSampling::SetSignalPoints (int nPoints)
{
    m_nSignalPoints = max (1, nPoints);
}

//-----------------------------------------------------------------------------
void TRedPitayaSampling::SetSignalPoints (const std::string &strPoints)
{
    int nPoints;

    try {
        nPoints = std::stoi (strPoints);
    }
    catch (...) {
        nPoints = 200;
    }
    SetSignalPoints (nPoints);
}

#ifdef	_RED_PITAYA_HW

#include "misc.h"
#include "trim.h"
//-----------------------------------------------------------------------------
rp_acq_decimation_t TRedPitayaSampling::GetHardwareDecimation() const
{
	//fprintf (stderr, "Decimation: %s\n", GetDecimation ().c_str());
	return (GetHardwareDecimationFromName(GetDecimation ()));
}

//-----------------------------------------------------------------------------
rp_acq_sampling_rate_t GetSamplingRate (const std::string &strRate)
{
	rp_acq_sampling_rate_t rate;

	try {
		//fprintf (stderr, "GetSamplingRate, strRate: %s\n", strRate.c_str());
		if (strRate.length() > 0) {
			double d = stod (strRate);

			if (d == 125e6) {
				rate = RP_SMP_125M;
		//fprintf (stderr, "Sampling Rate: 125M\n");
			}
			else if (d == 15.625e6) {
				rate = RP_SMP_15_625M;
		//fprintf (stderr, "Sampling Rate: 15.625M\n");
			}
			if (d == 1.953e6) {
				rate = RP_SMP_1_953M;
		//fprintf (stderr, "Sampling Rate: 1.953M\n");
			}
			if (d == 122070) {
				rate = RP_SMP_122_070K;
		//fprintf (stderr, "Sampling Rate: 122.070K\n");
			}
			if (d == 15258) {
				rate = RP_SMP_15_258K;
		//fprintf (stderr, "Sampling Rate: 15.258K\n");
			}
			if (d == 1907) {
				rate = RP_SMP_1_907K;
		//fprintf (stderr, "Sampling Rate: 1.907K\n");
			}
		}
		else
			rate = RP_SMP_125M;
	}
	catch (std::exception exp) {
		PrintRuntimeError (exp, "'GetSamplingRate'");
		rate = RP_SMP_125M;
	}
	//fprintf (stderr, "Sampling Rate: %s\n", strRate.c_str());
	return (RP_SMP_125M);
}

//-----------------------------------------------------------------------------
rp_acq_sampling_rate_t TRedPitayaSampling::GetHardwareSamplingRate() const
{
	rp_acq_sampling_rate_t rate;// = GetSamplingRate (GetRate());
	rate = GetSamplingRate (GetRate());
	return (rate);
}
#endif
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

