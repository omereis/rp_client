/******************************************************************************\
|                                 bd_types.h                                   |
\******************************************************************************/

#ifndef  BD_TYPES_H
#define  BD_TYPES_H

#include <vector>
#include <queue>
#include <string>

using namespace std;

//---------------------------------------------------------------------------
#ifndef	TStringVec
typedef vector<string> TStringVec;
#endif
//---------------------------------------------------------------------------
#ifndef	TFloatVec
typedef vector<float> TFloatVec;
#endif
//---------------------------------------------------------------------------
#ifndef	TFloatVecQueue
typedef queue<TFloatVec> TFloatVecQueue;
#endif
//---------------------------------------------------------------------------
#ifndef	uint
typedef	unsigned int	uint;
#endif
//---------------------------------------------------------------------------
#endif
