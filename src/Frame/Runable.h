#ifndef __RUNABLE__
#define __RUNABLE__

namespace Frame
{

class IRunable
{
public:
	IRunable(){}
	virtual ~IRunable(){}
	virtual void Run()=0;
};


template<typename T>
class Runable : public IRunable
{
private:
	typedef void (T::*MethodName)();
	MethodName	m_pMethod;
	T*			m_instance;

public:
	Runable(MethodName pfn , T* instance)
	{
		m_pMethod = pfn;
		m_instance = instance;
	}

	Runable(const Runable& runable)
	{
		m_pMethod = runable.m_pMethod;
		m_instance = runable.m_instance;
	}
	
	Runable& operator=(const Runable& runable)
	{
		m_pMethod = runable.m_pMethod;
		m_instance = runable.m_instance;
	}

	virtual ~Runable()
	{}

	virtual void Run()
	{(m_instance->*m_pMethod)();}

	static IRunable* Wrap(MethodName pfn , T* instance)
	{return new Runable(pfn, instance);}
};
}

#endif
