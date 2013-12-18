#ifndef __AUTOBUFFER__
#define __AUTOBUFFER__

namespace Frame
{

class AutoBuffer
{
private:
	AutoBuffer(const AutoBuffer&);
	AutoBuffer& operator= (const AutoBuffer&);
	char* m_ptr;
public:
	AutoBuffer(int size){m_ptr = new char[size];}
	virtual ~AutoBuffer(){delete[] m_ptr;}
	operator char*(){return m_ptr;}
	char& operator[](int i){return m_ptr[i];}
};


/// Create automtic collecting buffer in the heap.
template<int T>
class HeapBuffer
{
private:
	HeapBuffer(const HeapBuffer&);
	HeapBuffer& operator= (const HeapBuffer&);
	char* m_ptr;
public:
	HeapBuffer(){m_ptr = new char[T];}
	virtual ~HeapBuffer(){delete[] m_ptr;}
	operator char*(){return m_ptr;}
	char& operator[](int i){return m_ptr[i];}
	int Size(){return T;}
};

};

#endif
