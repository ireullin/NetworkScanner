#ifndef __SINGLETON__
#define __SINGLETON__

#include "Mutex.h"
#include <memory>

namespace Frame
{

//template<typename T>
//class Sealed
//{
//protected:
//	Sealed(){}
//};
//
//
//template<typename T>
//class SealedSingleton : private virtual Sealed<T>
//{
//private:
//	typename friend T;
//
//public:
//	SealedSingleton(){}
//};


template<typename T>
class Singleton
{
private:
	static Threading::Mutex	m_mutex;
	static std::auto_ptr<T> m_ptr;
	Singleton(const Singleton&);
	Singleton& operator= (const Singleton&);

protected:
	Singleton(){}

public:
	static T* Instance()
	{
		m_mutex.Lock();
		if(m_ptr.get()==NULL)m_ptr.reset(new T);
		m_mutex.Unlock();
		return m_ptr.get() ;
	}
};

template<typename T>Threading::Mutex Singleton<T>::m_mutex;
template<typename T>std::auto_ptr<T> Singleton<T>::m_ptr;

}

#endif
