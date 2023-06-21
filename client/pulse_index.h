/******************************************************************************\
|                               pulse_index.h                                  |
\******************************************************************************/
#ifndef	_PULSE_INDEX_H
#define	_PULSE_INDEX_H
//-----------------------------------------------------------------------------

#include "jsoncpp/json/json.h"

using namespace std;
#include <string>
#include "bd_types.h"

//-----------------------------------------------------------------------------
class TPulseIndex;
typedef vector<TPulseIndex> TPulseIndexVec;

class TPulseIndex
{
public:
	TPulseIndex ();
	TPulseIndex (int nStart, int nEnd);
	TPulseIndex (const TPulseIndex &other);

	static Json::Value ToJson (const TPulseIndexVec &vIndices);
	Json::Value AsJson() const;
	void Clear ();

	void SetPulse (int nStart, int nEnd, int nSteps);
	int GetStart () const;
	void SetStart (int nStart);
	int GetEnd () const;
	void SetEnd (int nEnd);
	int GetSteps () const;
	void SetSteps (int nSteps);
protected:
	void AssignAll (const TPulseIndex &other);
private:
	int m_nStart;
	int m_nEnd;
	int m_nSteps;
};
#endif
//---------------------------------------------------------------------------
