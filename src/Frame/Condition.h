#ifndef __CONDITION__
#define __CONDITION__

#include <iostream>

#include <exception>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#ifndef NANO
#define NANO 1000000000LL
#endif

namespace Threading
{
class ConditionException : public std::exception
{
private:
	int m_errno;
	std::string m_errmsg;
public:
	ConditionException(const std::string& errmsg, int error_no=0):m_errno(error_no),m_errmsg(errmsg){ }
	virtual ~ConditionException()throw(){}
	virtual const char* what() const throw(){return m_errmsg.c_str();}
	int no(){return m_errno;}
};


/// This class only for unix-like system.
class Condition
{
private:
	pthread_mutex_t m_hMutex;
	pthread_cond_t  m_hCond;
	inline void Exception(int rc);

public:
	Condition();
	~Condition();

	/// Return false if timeout.
	bool Wait(double waitSecond);
	void Wait();
	void Signal();
	void Broadcast();
};


Condition::Condition()
{
	int _rcCond	= pthread_cond_init(&m_hCond, NULL );
	Exception(_rcCond);

	int _rcMutex= pthread_mutex_init(&m_hMutex, NULL );
	Exception(_rcMutex);
}


Condition::~Condition()
{
	int _rcMutex=pthread_mutex_destroy(&m_hMutex );
	Exception(_rcMutex);

	int _rcCond = pthread_cond_destroy(&m_hCond );
	Exception(_rcCond);
}


void Condition::Wait()
{
	int _rcMutex1 = pthread_mutex_lock(&m_hMutex);
	Exception(_rcMutex1);

	int _rcCond = pthread_cond_wait(&m_hCond, &m_hMutex);
	Exception(_rcCond);

	int _rcMutex2 = pthread_mutex_unlock(&m_hMutex);
	Exception(_rcMutex2);
}


bool Condition::Wait(double waitSecond)
{
	int _rcMutex1 = pthread_mutex_lock(&m_hMutex);
	Exception(_rcMutex1);

	struct timespec _now;
	clock_gettime(CLOCK_REALTIME, &_now);
	double _deadLine = waitSecond + ((double)_now.tv_nsec / NANO);
	int _deadSec = floor(_deadLine);
	double _deadNsec = _deadLine - _deadSec;
	_now.tv_sec = _now.tv_sec + _deadSec;
	_now.tv_nsec = _deadNsec*NANO;

	int _rcCond = pthread_cond_timedwait(&m_hCond, &m_hMutex, &_now);

	int _rcMutex2 = pthread_mutex_unlock(&m_hMutex);
	Exception(_rcMutex2);

	if ( _rcCond == ETIMEDOUT )	return false;
	else if( _rcCond == 0 )		return true;
	else						Exception(_rcCond);
	return false;
}


void Condition::Signal()
{
	int _rcMutex1 = pthread_mutex_lock(&m_hMutex);
	Exception(_rcMutex1);

	int _rcCond = pthread_cond_signal(&m_hCond);
	Exception(_rcCond);

	int _rcMutex2 = pthread_mutex_unlock(&m_hMutex);
	Exception(_rcMutex2);
}


void Condition::Broadcast()
{
	int _rcMutex1 = pthread_mutex_lock(&m_hMutex);
	Exception(_rcMutex1);

	int _rcCond = pthread_cond_broadcast(&m_hCond);
	Exception(_rcCond);

	int _rcMutex2 = pthread_mutex_unlock(&m_hMutex);
	Exception(_rcMutex2);
}


void Condition::Exception(int rc)
{
	if(rc==0) 
		return;
	else if(rc==EAGAIN)
		throw ConditionException("The system lacked the necessary resources (other than memory) to initialise another mutex." ,rc);
	else if(rc==ENOMEM)
		throw ConditionException("Insufficient memory exists to initialise the mutex." ,rc);
	else if(rc==EPERM)
		throw ConditionException("The caller does not have the privilege to perform the operation." ,rc); 
	else if(rc==EBUSY)
		throw ConditionException("The implementation has detected an attempt to re-initialise the object referenced by mutex, a previously initialised, but not yet destroyed, mutex." ,rc);
	else if(rc==EINVAL)
		throw ConditionException("The value specified by attr is invalid." ,rc);
	else if(rc==EBUSY)
		throw ConditionException("The implementation has detected an attempt to destroy the object referenced by mutex while it is locked or referenced (for example, while being used in a pthread_cond_wait() or pthread_cond_timedwait()) by another thread." ,rc); 
	else if(rc==EINVAL)
		throw ConditionException("The value specified by mutex is invalid." ,rc);
	else
		throw ConditionException("Unknown returned value." ,rc);

}
}
#endif
