/******************************************************************************\
|                                MutexQueue.h                                  |
\******************************************************************************/
#ifndef  MUTEX_QUEUE_INC
#define  MUTEX_QUEUE_INC
//-----------------------------------------------------------------------------
#include <mutex>
#include <thread>
#include "bd_types.h"
#include "pulse_info.h"
#include <queue>
#include <unistd.h>
//-----------------------------------------------------------------------------
template <class T>
class TMutexQueue {
public:
    TMutexQueue () {
		m_fBusy = false;
		m_nMax = 100;
	}
//-----------------------------------------------------------------------------
    size_t GetSize() {
    	size_t n = 0;

        while (m_fBusy)
            usleep(1);
		m_fBusy = true;
        n = m_queue.size();
		m_fBusy = false;
    	return (n);
	}
//-----------------------------------------------------------------------------
    void Clear() {
    	size_t n = 0;

        while (m_fBusy)
            usleep(1);
		m_fBusy = true;
		m_queue.clear();
		m_fBusy = false;
	}
//-----------------------------------------------------------------------------
    void AddItem(const T &t) {
        while (m_fBusy)
            usleep(1);
		m_fBusy = true;
		m_queue.push_back (t);
		while (m_queue.size() > m_nMax)
			m_queue.erase (m_queue.begin());
		m_fBusy = false;
	}
//-----------------------------------------------------------------------------
    //void AddItem(const TDoubleVec &vPulse);
	void GetLastItem (T &t);
//-----------------------------------------------------------------------------
	void PopLastItem (T &t) {
        while (m_fBusy)
            usleep(1);
		m_fBusy = true;
		if (m_queue.size() > 0) {
			t = (T) *m_queue.begin();
			m_queue.erase (m_queue.begin());
		}
		m_fBusy = false;
	}
protected:
private:
    vector<T> m_queue;
	bool m_fBusy;
	size_t m_nMax;
};
//-----------------------------------------------------------------------------
//typedef TMutexQueue<TDoubleVecQueue>    TMutexVecQueue;
//typedef TMutexQueue<TPulseFilter>       TMutexPulseFilter;
//-----------------------------------------------------------------------------
#endif
