//---------------------------------------------------------------------------

#ifndef HostParamsH
#define HostParamsH
//---------------------------------------------------------------------------
#include <string>
#include "GlobalTypes.h"
//---------------------------------------------------------------------------
using namespace std;
class THostParams
{
public:
	THostParams ();
	THostParams (const THostParams &other);
	THostParams operator= (const THostParams &other);
	bool operator== (const THostParams &other) const;
	bool operator!= (const THostParams &other) const;
	void Clear ();

	bool LoadFromIni (const string &strIni);
	bool SaveToIni (const string &strIni);
protected:
	void AssignAll (const  THostParams &other);
public:
	string GetAddress () const;
	void SetAddress (const string &str);
	uint GetPort () const;
	void SetHostPort (uint uiPort);
private:
	string m_strAddress;
	string m_strErr;
	uint m_uiPort;
};
#endif
