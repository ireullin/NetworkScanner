#ifndef __MUTEX__
#define __MUTEX__

#include <exception>
#include <string>
#include <math.h>

#ifdef _MSC_VER
#include <windows.h>
#define MutexHandle HANDLE
#else
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
 #define MutexHandle pthread_mutex_t
#endif

#ifndef NANO
#define NANO 1000000000LL
#endif

namespace Threading
{

class MutexException : public std::exception
{
private:
	int m_errno;
	std::string m_errmsg;
public:
	MutexException(const std::string& errmsg, int error_no=0)
		:m_errno(error_no),m_errmsg(errmsg){ }
	virtual ~MutexException()throw(){}
	virtual const char* what() const throw(){return m_errmsg.c_str();}
	int no(){return m_errno;}
};


class Mutex
{
private:
	Mutex(const Mutex&);
	Mutex& operator= (const Mutex&);
	
	MutexHandle m_mutexHandle;
	MutexException Exception(int err);
	//bool m_isLocking;
public:
	Mutex();
	virtual ~Mutex();
	void Lock();
	
	/// Timeout will return false. Unlock will return true; 
	//bool Lock(double waitSecond);
	void Unlock();
	//bool IsLocking(){return m_isLocking;}
	//operator bool(){return m_isLocking;}
	MutexHandle Handle(){return m_mutexHandle;}
};

class AutoLock
{
private:
	AutoLock(const AutoLock&);
	AutoLock& operator= (const AutoLock&);
	
	Mutex& m_mutex;
	
public:
	AutoLock(Mutex& mutex):m_mutex(mutex){m_mutex.Lock();}
	~AutoLock(){m_mutex.Unlock();}
	//operator Mutex&(){return m_mutex;}
	operator Mutex*(){return &m_mutex;}
};

#ifdef _MSC_VER
Mutex::Mutex()
{
	//m_isLocking = false;
	m_mutexHandle = ::CreateMutex(NULL, FALSE, "");
	if(m_mutexHandle==NULL) throw Exception(::GetLastError());
}

Mutex::~Mutex()
{
	if( ::CloseHandle(m_mutexHandle)==0)
		throw Exception(::GetLastError());

	m_mutexHandle = 0;
}


void Mutex::Lock()
{
	DWORD _rc = ::WaitForSingleObject(m_mutexHandle, INFINITE);
	if(_rc==WAIT_FAILED)	throw Exception( ::GetLastError() );
	if(_rc==WAIT_ABANDONED) throw MutexException("A lock is not released." ,WAIT_ABANDONED);
	//m_isLocking = true;
}


bool Mutex::Lock(double waitSecond)
{
	DWORD _rc = ::WaitForSingleObject(m_mutexHandle, waitSecond*1000);
	if(_rc==WAIT_TIMEOUT)	return false;
	if(_rc==WAIT_FAILED)	throw Exception( ::GetLastError() );
	if(_rc==WAIT_ABANDONED) throw MutexException("A lock is not released." ,WAIT_ABANDONED);
	//m_isLocking = true;
	return true;
}


void Mutex::Unlock()
{
	//if(!m_isLocking)
	//	return;

	if( ::ReleaseMutex(m_mutexHandle)==0)
		throw Exception(::GetLastError());

	//m_isLocking = false;
}

MutexException Mutex::Exception(int err)
{
	char _szError[256];
	memset(_szError, 0, sizeof(_szError));
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, _szError, sizeof(_szError), NULL);
	return MutexException(_szError, err);
}

#else
Mutex::Mutex()
{
	int _rc = pthread_mutex_init(&m_mutexHandle, NULL);
	if(_rc!=0)throw Exception(_rc);
	//m_isLocking = false;
}

Mutex::~Mutex()
{
	int _rc = pthread_mutex_destroy(&m_mutexHandle);
	if(_rc!=0)throw Exception(_rc);
}


void Mutex::Lock()
{
	int _rc = pthread_mutex_lock(&m_mutexHandle); 
	if(_rc!=0)throw Exception(_rc);
	//m_isLocking = true;
}


//bool Mutex::Lock(double waitSecond)
//{
//	struct timespec _now;
//	clock_gettime(CLOCK_REALTIME, &_now);
//	double _deadLine = waitSecond + ((double)_now.tv_nsec / NANO);
//	int _deadSec = floor(_deadLine);
//	double _deadNsec = _deadLine - _deadSec;
//	_now.tv_sec = _now.tv_sec + _deadSec;
//	_now.tv_nsec = _deadNsec*NANO;
//
//	int _rc = pthread_mutex_timedlock(&m_mutexHandle, &_now);
//	if(_rc==ETIMEDOUT)	return false;
//	if(_rc!=0)			throw Exception(_rc);
//	m_isLocking = true;
//	return true;
//}


void Mutex::Unlock()
{
	//if(!m_isLocking)
	//	return;

	int _rc = pthread_mutex_unlock(&m_mutexHandle); 
	if(_rc!=0)throw Exception(_rc);
	//m_isLocking = false;
}


MutexException Mutex::Exception(int err)
{
	if(err==EAGAIN)
		throw MutexException("The system lacked the necessary resources (other than memory) to initialise another mutex." ,err);
	else if(err==ENOMEM)
		throw MutexException("Insufficient memory exists to initialise the mutex." ,err);
	else if(err==EPERM)
		throw MutexException("The caller does not have the privilege to perform the operation." ,err); 
	else if(err==EBUSY)
		throw MutexException("The implementation has detected an attempt to re-initialise the object referenced by mutex, a previously initialised, but not yet destroyed, mutex." ,err);
	else if(err==EINVAL)
		throw MutexException("The value specified by attr is invalid." ,err);
	else if(err==EBUSY)
		throw MutexException("The implementation has detected an attempt to destroy the object referenced by mutex while it is locked or referenced (for example, while being used in a pthread_cond_wait() or pthread_cond_timedwait()) by another thread." ,err); 
	else if(err==EINVAL)
		throw MutexException("The value specified by mutex is invalid." ,err);

	return MutexException("Unknow exception", err);
}


#endif

}
#endif /*__MUTEX__*/
