/******************************************************************************\
|                                 bd_types.h                                   |
\******************************************************************************/

#ifndef  BD_TYPES_H
#define  BD_TYPES_H

#include <vector>
#include <queue>
#include <string>

#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;

//---------------------------------------------------------------------------
#ifndef	TStringVec
typedef vector<string> TStringVec;
#endif
//---------------------------------------------------------------------------
#ifndef	TDoubleVec
typedef vector<double> TDoubleVec;
#endif
//---------------------------------------------------------------------------
#ifndef	TFloatVec
typedef vector<float> TFloatVec;
#endif
//---------------------------------------------------------------------------
#ifndef	TFloatVecVec
typedef vector<TFloatVec> TFloatVecVec;
#endif
//---------------------------------------------------------------------------
#ifndef	TFloatVecIterator
typedef TFloatVec::iterator	TFloatVecIterator;
#endif
//---------------------------------------------------------------------------
#ifndef	TFloatVecConstIterator
typedef TFloatVec::const_iterator	TFloatVecConstIterator;
#endif
//---------------------------------------------------------------------------
#ifndef	TDoubleVecQueue
typedef queue<TDoubleVec> TDoubleVecQueue;
#endif
//---------------------------------------------------------------------------
#ifndef	TFloatVecQueue
typedef queue<TFloatVec> TFloatVecQueue;
#endif
//---------------------------------------------------------------------------
#ifndef	TFloatQueue
typedef queue<float> TFloatQueue;
#endif
//---------------------------------------------------------------------------
#ifndef	uint
typedef	unsigned int	uint;
#endif
//---------------------------------------------------------------------------
#ifndef	chrono_clock
typedef	std::chrono::time_point<std::chrono::system_clock> chrono_clock;
#endif
#endif
