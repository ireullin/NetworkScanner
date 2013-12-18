#ifndef __FILEIO__
#define __FILEIO__

#include "TextBufferReader.h"

#include <aio.h>

class FileIO
{
private:
	friend class FileController;

	std::string				m_path;
	int						m_fd;
	fd_set					m_set;

	struct stat				m_stat;
	time_t					m_lastModify;
	off_t					m_lastSize;

	FileIO(const FileIO&);
	FileIO& operator= (const FileIO&);

protected:
	Threading::Mutex		m_mutex;

	FileIO(const std::string& szPath, bool append);
	
	virtual void CheckFlush(){/*only for inheritance*/}
	
	/// 為了避免dead lock給內部函式叫用的
	inline void NoLockRead(int pos, int size, char* buff);
	inline void NoLockWrite(const char* buff, int size);
	inline int  NoLockSeek(int pos, int type);

public:
	
	virtual ~FileIO();
	
	/// 檔案的路徑
	const std::string& Path()
	{return m_path;}

	/// 檔案大小
	int Size();

	/// 將緩衝寫入檔案,沒有Buffer的話則沒效果
	virtual void Flush(){/*only for inheritance*/}
		    
	/// 寫入資料在檔案最後
	virtual void Write(const char* data, int size);
	virtual void Write(const std::string& szData);
	
	/// 寫入含換行符號的資料在檔案最後
	virtual void WriteLine(const char* data, int size);
	virtual void WriteLine(const std::string& szData);
	
	/// 寫入固定區塊,如果檔案大小小於區塊,則寫在最後
	virtual void WriteBlock(int pos, const char* data, int size);
	virtual void WriteBlock(int pos, const std::string& szData);
	
	/// 依照格式寫在檔案最後
	virtual void WriteFormat(const char* format, ...);

	/// 將檔案一次讀出
	std::string ReadAll();
	
	/// 將檔案讀到Buffer
	void ReadToBuffer(ITextBufferReader& buffReader);
	
	/// 讀一個固定區塊出來
	std::string ReadBlock(int pos, int size);
	
	/// 檔案是否有被更動過
	/// 只代表此次與上一次呼叫此函式中間是否有被更動過
	bool IsModified();

	//bool SyncRead(std::string& szBuffer, int offset, int length, double waitsecond);
};

/*
bool FileIO::SyncRead(std::string& szBuffer, int offset, int length, double waitsecond)
{
	Frame::AutoBuffer _buff(length);
	memset((char*)_buff, 0, length);

	struct aiocb _aiocb;
	memset(&_aiocb, 0, sizeof(struct aiocb));
	_aiocb.aio_buf = (char*)_buff;
	_aiocb.aio_fildes = m_fd;
	_aiocb.aio_nbytes = length;
	_aiocb.aio_offset = offset;

	if(aio_read(&_aiocb)!=0)
		throw FRAME_EXPCEPTION_1(errno);

	
	struct timespec _now;
	clock_gettime(CLOCK_REALTIME, &_now);
	double _deadLine = waitsecond + ((double)_now.tv_nsec / NANO);
	int _deadSec = floor(_deadLine);
	double _deadNsec = _deadLine - _deadSec;
	_now.tv_sec = _now.tv_sec + _deadSec;
	_now.tv_nsec = _deadNsec*NANO;


	struct aiocb* _pAiocb[1];
	_pAiocb[0] = &_aiocb;

	int _rc = aio_suspend( _pAiocb, 1, &_now );
	if(_rc==0)
	{
	szBuffer.clear();
	szBuffer.append((char*)_buff,length);
	return true;
	}
	else
		return false;
}
*/

FileIO::FileIO(const std::string& szPath, bool append)
	:m_fd(0),m_lastModify(0),m_lastSize(0)
{
	if(append)	m_fd = open(szPath.c_str(), O_RDWR|O_CREAT);//O_APPEND會讓lseek無效
	else		m_fd = open(szPath.c_str(), O_RDWR|O_CREAT|O_TRUNC);
	if(m_fd==0)	throw FRAME_EXPCEPTION_1("open file fialed");

	//IsModified();
}


FileIO::~FileIO()
{
	if(m_fd!=0)
		close(m_fd);
}


void FileIO::ReadToBuffer(ITextBufferReader& buffReader)
{
	Threading::AutoLock _auto(m_mutex);
	int _size = NoLockSeek(0, SEEK_END);
	SAFE_DELETE(buffReader.m_buff);
	buffReader.m_buff = new char[_size];
	buffReader.m_size = _size;
	NoLockRead(0, _size, buffReader.m_buff);
}


std::string FileIO::ReadAll()
{
	Threading::AutoLock _auto(m_mutex);
	int _size = NoLockSeek(0, SEEK_END);
	Frame::AutoBuffer _buff(_size);
	NoLockRead(0, _size, (char*)_buff);
	return std::string((char*)_buff, _size);
}


std::string FileIO::ReadBlock(int pos, int size)
{
	Threading::AutoLock _auto(m_mutex);
	Frame::AutoBuffer _buff(size);
	NoLockRead(pos, size, (char*)_buff);
	return std::string((char*)_buff, size);
}


void FileIO::NoLockRead(int pos, int size, char* buff)
{
	NoLockSeek(pos, SEEK_SET);
	
	if(read(m_fd, buff, size)<0)
		throw FRAME_EXPCEPTION_1(errno);
}


void FileIO::WriteBlock(int pos, const std::string& szData)
{
	Threading::AutoLock _auto(m_mutex);
	NoLockWrite(szData.c_str(), szData.size());
}


void FileIO::WriteBlock(int pos, const char* data, int size)
{
	Threading::AutoLock _auto(m_mutex);
	NoLockSeek(pos, SEEK_SET);
	NoLockWrite(data, size);
}


void FileIO::WriteLine(const std::string& szData)
{
	Threading::AutoLock _auto(m_mutex);
	NoLockSeek(0, SEEK_END);
	NoLockWrite(szData.c_str(), szData.size());
	NoLockWrite("\n", 1);
}


void FileIO::WriteLine(const char* data, int size)
{
	Threading::AutoLock _auto(m_mutex);
	NoLockSeek(0, SEEK_END);
	NoLockWrite(data, size);
	NoLockWrite("\n", 1);
}


void FileIO::Write(const std::string& szData)
{
	Threading::AutoLock _auto(m_mutex);
	NoLockSeek(0, SEEK_END);
	NoLockWrite(szData.c_str(), szData.size());
}


void FileIO::Write(const char* data, int size)
{
	Threading::AutoLock _auto(m_mutex);
	NoLockSeek(0, SEEK_END);
	NoLockWrite(data, size);
}


void FileIO::WriteFormat(const char* format, ...)
{
	char _acFormat[1024];
	//memset(_acFormat, '0', sizeof(_acFormat));

	va_list args;
	va_start(args, format);
	int _size = vsprintf(_acFormat, format, args);
	va_end(args);

	Write(_acFormat, _size);
}


void FileIO::NoLockWrite(const char* buff, int size)
{
	if(write(m_fd, buff, size)<0)
		throw FRAME_EXPCEPTION_1(errno);
}


int FileIO::NoLockSeek(int pos, int type)
{
	int _size = lseek(m_fd, pos, type);
	if(_size<0)		throw FRAME_EXPCEPTION_1(errno);
	return _size;
}


int FileIO::Size()
{
	Threading::AutoLock _auto(m_mutex);
	return NoLockSeek(0, SEEK_END);
}


bool FileIO::IsModified()
{
	if ( 0 != stat(m_path.c_str(), &m_stat) )
		throw FRAME_EXPCEPTION_1("IsModified failed.");

	if(m_lastModify==m_stat.st_mtime && m_lastSize==m_stat.st_size )
		return false;

	m_lastSize = m_stat.st_size;
	m_lastModify = m_stat.st_mtime;

	return true;
}

#endif
