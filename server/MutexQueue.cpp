/******************************************************************************\
|                              MutexQueue.cpp                                  |
\******************************************************************************/
#include "MutexQueue.h"

#include <unistd.h>

/*
//-----------------------------------------------------------------------------
template <class T>
TMutexQueue<T>::TMutexQueue ()
{
    m_pQueue = NULL;
    m_pmtx   = NULL;
}
*/

/*
//-----------------------------------------------------------------------------
template <class T>
TMutexQueue<T>::TMutexQueue (mutex *pmtx)
{
    //m_pQueue = new queue<T>;//pQueue;
    m_pmtx   = pmtx;
	m_nMax = 100;
}
*/

/*
//-----------------------------------------------------------------------------
template <class T>
TMutexQueue<T>::~TMutexQueue ()
{
	//delete m_pQueue;
}
*/

//-----------------------------------------------------------------------------
template <class T>
size_t TMutexQueue<T>::GetSize() const
{
    size_t n = 0;

    if (m_pmtx != NULL) {
        while (m_pmtx->try_lock())
            usleep(1);
        m_pmtx->lock();
        n = m_queue.size();
        //n = m_pQueue->size();
        m_pmtx->unlock();
    }
    return (n);
}

//-----------------------------------------------------------------------------
template <class T>
void TMutexQueue<T>::Clear()
{
    size_t n = 0;

    if (m_pmtx != NULL) {
        while (m_pmtx->try_lock())
            usleep(1);
		m_queue.clear();
        //m_pmtx->lock();
		//while (m_pQueue->size() > 0)
			//m_pQueue->pop();
        //m_pQueue->clear();
        m_pmtx->unlock();
    }
}

//-----------------------------------------------------------------------------
template <class T>
void TMutexQueue<T>::AddItem(const T &t)
{
    size_t n = 0;

    if (m_pmtx != NULL) {
        while (m_pmtx->try_lock())
            usleep(1);
        m_pmtx->lock();
		m_queue.push_back (t);
		while (m_queue.size() > m_nMax)
			m_queue.erase (m_queue.begin());
        //m_pQueue->push (t);
		//while (m_pQueue->size() > m_nMax)
			//m_pQueue->pop();
        m_pmtx->unlock();
    }
}

/*
//-----------------------------------------------------------------------------
template <class T>
void TMutexQueue<T>::AddItem(int nItem)
{
    size_t n = 0;

    if (m_pmtx != NULL) {
        while (m_pmtx->try_lock())
            usleep(1);
        m_pmtx->lock();
        m_pQueue->push (nItem);
		while (m_pQueue->size() > m_nMax)
			m_pQueue->pop();
        m_pmtx->unlock();
    }
}
*/

/*
//-----------------------------------------------------------------------------
template <class T>
void TMutexQueue<T>::AddItem (const TDoubleVec &vPulse)
{
    size_t n = 0;

    if (m_pmtx != NULL) {
        while (m_pmtx->try_lock())
            usleep(1);
        m_pmtx->lock();
        m_pQueue->push (vPulse);
		while (m_pQueue->size() > m_nMax)
			m_pQueue->pop();
        m_pmtx->unlock();
	}
}
*/

//-----------------------------------------------------------------------------
template <class T>
void TMutexQueue<T>::GetLastItem (T &t)
{
    if (m_pmtx != NULL) {
        while (m_pmtx->try_lock())
            usleep(1);
        m_pmtx->lock();
		if (m_queue.size() > 0) {
			t = (T) *m_queue.begin();
		}
        //m_pQueue->push (vPulse);
		//while (m_pQueue->size() > m_nMax)
			//m_pQueue->pop();
        m_pmtx->unlock();
	}
}

//-----------------------------------------------------------------------------
template <class T>
void TMutexQueue<T>::PopLastItem (T &t)
{
    if (m_pmtx != NULL) {
        while (m_pmtx->try_lock())
            usleep(1);
        m_pmtx->lock();
		if (m_queue.size() > 0) {
			t = (T) *m_queue.begin();
			m_queue.erase (m_queue.begin());
		}
        //m_pQueue->push (vPulse);
		//while (m_pQueue->size() > m_nMax)
			//m_pQueue->pop();
        m_pmtx->unlock();
	}
}

/*
//-----------------------------------------------------------------------------
void foo (TMutexQueue<TDoubleVecQueue> *pqRaw)
{
	printf ("%zu\n", pqRaw->GetSize());
}

void SamplingThread (void *p)
{
}

//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
	mutex mtxRaw;

	TMutexQueue<TDoubleVecQueue> qRaw (&mtxRaw);
	foo (&qRaw);
	std::thread threadTick (SamplingThread, (void *) &qRaw);//&mtxRaw);//mtxPulsesQueue);
	return (0);
}
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
