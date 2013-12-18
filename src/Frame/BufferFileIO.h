#ifndef __BUFFERFILEIO__
#define __BUFFERFILEIO__

#include "FileIO.h"

class BufferFileIO : public FileIO
{
private:
	friend class FileController;
	typedef FileIO super;

	std::string	m_szBuff;
	int			m_waitSecond;
	DateTime	m_dtLastWrite;

	BufferFileIO(const BufferFileIO&);
	BufferFileIO& operator= (const BufferFileIO&);

	void NoLockFlush();

protected:
	BufferFileIO(const std::string& szPath, bool append, int bufferWaitSecond, int bufferSize);

	virtual void CheckFlush();

public:
	
	virtual ~BufferFileIO();
	
	virtual void Flush();
		    
	virtual void Write(const char* data, int size);
	virtual void Write(const std::string& szData);
	virtual void WriteLine(const char* data, int size);
	virtual void WriteLine(const std::string& szData);
	virtual void WriteBlock(int pos, const char* data, int size);
	virtual void WriteBlock(int pos, const std::string& szData);
	virtual void WriteFormat(const char* format, ...);
};


BufferFileIO::BufferFileIO(const std::string& szPath, bool append, int bufferSize, int bufferWaitSecond)
	:super(szPath, append),m_waitSecond(bufferWaitSecond)
{
	m_szBuff.reserve(bufferSize);
}


BufferFileIO::~BufferFileIO()
{
	Flush();
}


void BufferFileIO::NoLockFlush()
{
	if(m_szBuff.empty())
		return;

	super::NoLockWrite(m_szBuff.c_str(), m_szBuff.size());
	m_szBuff.clear();

	m_dtLastWrite = DateTime::Now();
}


void BufferFileIO::Flush()
{
	Threading::AutoLock _auto(super::m_mutex);
	NoLockFlush();
}


void BufferFileIO::CheckFlush()
{
	DateTimeDiff _df = DateTime::Now() - m_dtLastWrite;
	if(_df.TatolSecond() > m_waitSecond)
		NoLockFlush();
}


void BufferFileIO::Write(const char* data, int size)
{
	Threading::AutoLock _auto(super::m_mutex);
	if(m_szBuff.capacity() < (size+m_szBuff.size()))
	{
		NoLockFlush();
	}

	m_szBuff.append(data, size);
}


void BufferFileIO::Write(const std::string& szData)
{
	Threading::AutoLock _auto(super::m_mutex);
	if(m_szBuff.capacity() < (szData.size()+m_szBuff.size()))
		NoLockFlush();

	m_szBuff.append(szData);
}


void BufferFileIO::WriteLine(const char* data, int size)
{
	Threading::AutoLock _auto(super::m_mutex);
	if(m_szBuff.capacity() < (size+m_szBuff.size()))
		NoLockFlush();

	m_szBuff.append(data, size);
	m_szBuff.append("\n");
}


void BufferFileIO::WriteLine(const std::string& szData)
{
	Threading::AutoLock _auto(super::m_mutex);
	if(m_szBuff.capacity() < (szData.size()+m_szBuff.size()))
		NoLockFlush();

	m_szBuff.append(szData);
	m_szBuff.append("\n");
}


void BufferFileIO::WriteFormat(const char* format, ...)
{
	char _acFormat[1024];
	//memset(_acFormat, '0', sizeof(_acFormat));

	va_list args;
	va_start(args, format);
	int _size = vsprintf(_acFormat, format, args);
	va_end(args);

	this->Write(_acFormat, _size);
}


void BufferFileIO::WriteBlock(int pos, const char* data, int size)
{
	Threading::AutoLock _auto(super::m_mutex);
	NoLockFlush();
	super::NoLockSeek(pos, SEEK_SET);
	super::NoLockWrite(data, size);
}

void BufferFileIO::WriteBlock(int pos, const std::string& szData)
{
	Threading::AutoLock _auto(super::m_mutex);
	NoLockFlush();
	super::NoLockSeek(pos, SEEK_SET);
	NoLockWrite(szData.c_str(), szData.size());
}

#endif
