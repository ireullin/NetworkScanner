#ifndef __THREADPOOL__
#define __THREADPOOL__

#include <vector>
#include <algorithm>
#include "Runable.h"
#include "Singleton.h"
#include "Condition.h"
#include "ThreadBind.h"


namespace Frame
{

class SubThread;
class ThreadPool;


enum SUBTHREAD_STATUS
{
	eINIT=0,
	eIDLE,
	eWORKING,
	eRELEASE
};


/// Only be created by ThreadPool.
class SubThread : public Threading::Thread
{
private:
	SubThread(const SubThread&);
	SubThread& operator= (const SubThread&);

	int						m_status;
	bool					m_isContinue;
	IRunable*				m_pIRunable;
	Threading::Condition	m_con;
	
public:
	SubThread():m_status(eINIT),m_isContinue(false),m_pIRunable(NULL){}
	~SubThread();

	int		Status();
	void	Stop();
	void	WakeUp(IRunable* pIRunable);
	virtual void Execute(void* lhs, void* rhs);
};


class ThreadPool : public Singleton<ThreadPool>
{
private:
	friend class Singleton<ThreadPool>;
	typedef std::vector<SubThread*> ThreadArray;

	ThreadArray			m_arrThread;
	Threading::Mutex	m_mutex;

	void CreateThreads(int count);

	ThreadPool();

	static bool PfnTidyThread(SubThread* subThread);

public:
	~ThreadPool();

	/// How many threads in this pool.
	int Total();

	/// How many threads is busy in this pool
	int OnBusy();

	/// Invoke an idle thread to handle the object.
	void Invoke(IRunable* pIRunable);

	/// Assign the max count of threads in this pool.
	/// If parameter count <= current total count, it will not do everything.
	void MaxThreads(int count);
	
	/// Remove unused threads.
	void TidyThreads();
};



//////////////////////////////////////////////// ThreadPool's methods ///////////////////////////////////////////////
void ThreadPool::CreateThreads(int count)
{	
	for(int i=0; i<count; i++)
	{
		SubThread* _pSubThread = new SubThread();
		_pSubThread->Start();
		m_arrThread.push_back(_pSubThread);

		while(_pSubThread->Status()==eINIT)
		{Threading::Sleep(0.01);}
	}
}


ThreadPool::ThreadPool()
{
	Threading::AutoLock _auto(m_mutex);
	CreateThreads(5);
}

bool ThreadPool::PfnTidyThread(SubThread* subThread)
{
	if(subThread->Status()!=eIDLE)
		return false;
		
	subThread->Stop();
	subThread->Join();
	delete subThread;
	return true;
}

	
ThreadPool::~ThreadPool()
{
	Threading::AutoLock _auto(m_mutex);
	int _count = m_arrThread.size();
	for(int i=0; i<_count; i++)
		m_arrThread[i]->Stop();

	for(int i=0; i<_count; i++)
	{
		m_arrThread[i]->Join();
		delete m_arrThread[i];
		m_arrThread[i] = NULL;
	}
}


int ThreadPool::Total()
{return m_arrThread.size();}


int ThreadPool::OnBusy()
{
	int _rc = 0;
	int _count = m_arrThread.size();
			
	for(int i=0; i<_count; i++)
	{
		if( m_arrThread[i]->Status()==eWORKING)
			_rc++;
	}

	return _rc;
}


void ThreadPool::Invoke(IRunable* pIRunable)
{
	Threading::AutoLock _auto(m_mutex);

	while(true)
	{
		int _count = m_arrThread.size();

		for(int i=0; i<_count; i++)
		{
			if( m_arrThread[i]->Status()==eIDLE)
			{
				m_arrThread[i]->WakeUp(pIRunable);
				return;
			}
		}
		
		CreateThreads(5);
	}	
}


void ThreadPool::MaxThreads(int count)
{
	int _diffCount = count - Total();
	if(_diffCount<=0)
		return;
	
	CreateThreads(_diffCount);
}
	
	
void ThreadPool::TidyThreads()
{
	Threading::AutoLock _auto(m_mutex);
	m_arrThread.erase(std::remove_if(m_arrThread.begin(), m_arrThread.end(), &ThreadPool::PfnTidyThread), m_arrThread.end());
}


///////////////////////////////////////// SubThread's methods ///////////////////////////////////////////////
SubThread::~SubThread()
{
	if(m_pIRunable!=NULL)
	{
		delete m_pIRunable;
		m_pIRunable = NULL;
	}
}

int SubThread::Status()
{return m_status;}


void SubThread::Stop()
{
	m_isContinue = false;
	m_con.Signal();
}


void SubThread::Execute(void* lhs, void* rhs)
{
	m_isContinue = true;
	m_status = eIDLE;

	while(m_isContinue)
	{
		m_con.Wait(5);
		
		if(m_status==eIDLE)
			continue;
		
		if(m_status==eWORKING)
		{
			m_pIRunable->Run();
			delete m_pIRunable;
			m_pIRunable = NULL;
			m_status = eIDLE;
			continue;
		}
	}

	m_status = eRELEASE;
}

void SubThread::WakeUp(IRunable* pIRunable)
{
	m_pIRunable = pIRunable;
	m_status = eWORKING;
	m_con.Signal();
}

}
#endif
