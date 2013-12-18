#ifndef __THREADBIND__
#define __THREADBIND__

#include "Thread.h"

namespace Threading
{

 //You can run a member function on other thread without inherited Thread class.
template<typename T>
class ThreadBind : public Thread
{
private:
	ThreadBind(const ThreadBind&);
	ThreadBind& operator= (const ThreadBind&);

public:
	typedef void (T::*MethodName)(void* lhs, void* rhs);
	MethodName	m_pMethod;
	T*			m_instance;

	ThreadBind(MethodName pfn , T* instance)
	{
		m_pMethod = pfn;
		m_instance = instance;
	}

protected:
	virtual void Execute(void* lhs, void* rhs)
	{
		(m_instance->*m_pMethod)(lhs, rhs);
	}
};
}

#endif /*__RUNTHREAD__*/
