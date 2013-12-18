#ifndef __THREAD__
#define __THREAD__

#include <string>
#include <exception>
#include <assert.h>

#ifdef _MSC_VER
	#include <windows.h>
#else
	#include <pthread.h>
	#include <unistd.h>
	#include <errno.h>
#endif


#ifdef _MSC_VER
	#define ThreadHandle HANDLE
#else
	#define ThreadHandle pthread_t
#endif


typedef void* (THREAD_FUNPTR)(void *);



namespace Threading
{

extern void			Sleep(double second);
extern unsigned int CurrentThreadID();
extern ThreadHandle	Begin(THREAD_FUNPTR pfn, void* params);


class ThreadException : public std::exception
{
private:
	int m_errno;
	std::string m_errmsg;
public:
	ThreadException(const std::string& errmsg, int error_no=0)
		:m_errno(error_no),m_errmsg(errmsg){ }
	virtual ~ThreadException()throw(){}
	virtual const char* what() const throw(){return m_errmsg.c_str();}
	int no(){return m_errno;}
};

/// A thread object is only implemented by inherited.
class Thread
{
private:
	Thread(const Thread&);
	Thread& operator= (const Thread&);

	ThreadHandle	m_handle;
	unsigned int	m_id;
	void*			m_rhs;
	void*			m_lhs;

	void			Exception(int err);
	static void*	ThreadCallback(void* param);
	static void		Signal();


protected:
	Thread();
	virtual void Execute(void* lhs, void* rhs)=0;
	void ReleaseHandle();
	void Sleep(double second);

public:
	virtual ~Thread();
	void Start(void* lhs=NULL, void* rhs=NULL);
	void Join();
	
	ThreadHandle Handle()
	{return m_handle;}
	
	//operator int()
	//{return (int)m_handle;}

	//operator ThreadHandle()
	//{return m_handle;}

	bool IsRunning()
	{return m_id!=0;}

	unsigned int ID()
	{return m_id;}
};

Thread::Thread()
	:m_handle(0),m_id(0)
{}


Thread::~Thread()
{
	if(IsRunning())
		throw ThreadException("Memory was be deleted before the thread finished.", 0);
	
	ReleaseHandle();
}


void Thread::ReleaseHandle()
{
	if(m_handle!=0)
	{
#ifdef _MSC_VER
		int _rc = ::CloseHandle(m_handle);
		if(_rc==0) Exception(::GetLastError());
#else
		int _rc = pthread_detach(m_handle);
		if(_rc!=0 && _rc!=ESRCH) Exception(_rc);
#endif
		m_handle = 0;
	}
}


void Thread::Join()
{
	if(m_id==CurrentThreadID())
		throw ThreadException("Can not be joined by self.", 0);

#ifdef _MSC_VER
	DWORD _rc = ::WaitForSingleObject(m_handle, INFINITE);
	if(_rc==WAIT_FAILED)	Exception( ::GetLastError() );
	if(_rc==WAIT_ABANDONED) throw ThreadException("A lock is not released." ,WAIT_ABANDONED);
#else
	int _rc = pthread_join(m_handle, NULL );
	if(_rc!=0 && _rc!=ESRCH) Exception(_rc);
#endif

	ReleaseHandle();
}


void* Thread::ThreadCallback(void* param)
{
	Thread* _thread = reinterpret_cast<Thread*>(param);
	
	try
	{
		_thread->m_id = CurrentThreadID();
		_thread->Execute(_thread->m_lhs, _thread->m_rhs);
	}
	catch(std::exception& exp)
	{
		std::cout << "Thread " << CurrentThreadID() << " threw an exception \"" << exp.what() << "\"" << std::endl;
		Signal();
	}
	catch(const char* errMsg)
	{
		std::cout << "Thread " << CurrentThreadID() << " threw an exception \"" << errMsg << "\"" << std::endl;
		Signal();
	}
	catch(const std::string& errMsg)
	{
		std::cout << "Thread " << CurrentThreadID() << " threw an exception \"" << errMsg << "\"" << std::endl;
		Signal();
	}
	catch(...)
	{
		std::cout << "Thread " << CurrentThreadID() << " threw an unknown exception." << std::endl;
		Signal();
	}
	
	_thread->m_id = 0;
	return NULL;
}


void Thread::Signal()
{
	struct sigaction _act;  
	_act.sa_handler = signal_event;
	_act.sa_flags = 0;  
	//sigfillset(&_act.sa_mask);
	sigemptyset(&_act.sa_mask);
	sigaction(SIGUSR1, &_act, NULL);

	raise(SIGUSR1);
}


void Thread::Start(void* lhs, void* rhs)
{
	m_rhs = rhs;
	m_lhs = lhs;

#ifdef _MSC_VER
	DWORD _threadID = 0;
	m_handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&Thread::ThreadCallback, this, 0, &_threadID);
	if(m_handle<=0) Exception( ::GetLastError() );
#else
	int _rc = pthread_create( &m_handle, NULL, Thread::ThreadCallback, this );
	if(_rc!=0) Exception(_rc);
#endif
}


void Thread::Exception(int err)
{
#ifdef _MSC_VER
	char _szError[256];
	memset(_szError, 0, sizeof(_szError));
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, _szError, sizeof(_szError), NULL);
	throw ThreadException(_szError, err);
#else
	if(err==EAGAIN)
		throw ThreadException("The system lacked the necessary resources to create another thread, or the system-imposed limit on the total number of threads in a process PTHREAD_THREADS_MAX would be exceeded.",err);
	else if(err==EINVAL)
		throw ThreadException("The value specified by attr is invalid.",err);
	else if(err==EPERM)
		throw ThreadException("The caller does not have appropriate permission to set the required scheduling parameters or scheduling policy.",err);
	else
		throw ThreadException("Unknown thread error.",err);
#endif	
}


void Thread::Sleep(double second)
{
	Threading::Sleep(second);
}


void Sleep(double second)
{
#ifdef _MSC_VER
	::Sleep(second*1000);
#else
	usleep(second*1000*1000);
#endif
}


unsigned int CurrentThreadID()
{
#ifdef _MSC_VER
	return (unsigned int)GetCurrentThreadId();
#else
	return (unsigned int)pthread_self();
#endif
}


ThreadHandle Begin(THREAD_FUNPTR pfn, void* params)
{
	ThreadHandle _handle = 0;

#ifdef _MSC_VER
	DWORD _threadID = 0;
	_handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)pfn, params, 0, &_threadID);
	if(_handle<=0)throw ThreadException("Unknown thread error.",0);
#else
	int _rc = pthread_create( &_handle, NULL, pfn, params );
	if(_rc!=0) throw ThreadException("Unknown thread error.",0);
#endif

	return _handle;
}


}
#endif /*THREAD*/
