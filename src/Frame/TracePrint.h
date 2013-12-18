#ifndef __TRACEPRINT__
#define __TRACEPRINT__

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/timeb.h>


#define TRACE_OUT			stdout
#define TRACE_LINE_SIZE		1024
#define TRACE_HDR_SIZE		40

class TracePrint
{
private:
	std::string m_szFunctionName;
	std::string m_szFullName;
	std::string m_szFile;
	int			m_line;
	bool		m_show;
	bool		m_entryPoint;

	static int ThreadID();
	static std::string SplitFileName(const std::string& szName);
	static std::string SplitFunctionName(const std::string& szName);
	
	void FillHeader(char* buff, bool includeLine);

public:
	TracePrint(const char* name, const char* file, bool entry, bool show);
	~TracePrint();
	void SetLine(int line);
	void Print(const char* format, ...);
	void Record(int line);
};


TracePrint::TracePrint(const char* name, const char* file, bool entry, bool show)
	:m_line(0),m_show(show),m_entryPoint(entry)
{
	if(!m_show)
		return;

	m_szFullName.append(name);
	m_szFile = SplitFileName(file);
	m_szFunctionName = SplitFunctionName(name);

	char _buff[TRACE_LINE_SIZE];
	memset(_buff, ' ', TRACE_LINE_SIZE);
	FillHeader(_buff, false);

	int _rc = sprintf(&_buff[TRACE_HDR_SIZE], "*****  ENTER %s  *****", m_szFullName.c_str());
	_buff[TRACE_HDR_SIZE+_rc] = '\n';
	_buff[TRACE_HDR_SIZE+_rc+1] = 0;

	if(m_entryPoint)
		fprintf(TRACE_OUT, "%s", _buff);
}

TracePrint::~TracePrint()
{
	if(!m_show)
		return;

	char _buff[TRACE_LINE_SIZE];
	memset(_buff, ' ', TRACE_LINE_SIZE);
	FillHeader(_buff, false);

	int _rc = sprintf(&_buff[TRACE_HDR_SIZE], "*****  LEAVE %s  *****", m_szFullName.c_str());
	_buff[TRACE_HDR_SIZE+_rc] = '\n';
	_buff[TRACE_HDR_SIZE+_rc+1] = 0;

	if(m_entryPoint)
		fprintf(TRACE_OUT, "%s", _buff);
}


void TracePrint::Print(const char* format, ...)
{
	if(!m_show)
		return;

	char _buff[TRACE_LINE_SIZE];
	memset(_buff, ' ', TRACE_LINE_SIZE);
	FillHeader(_buff, true);

	va_list args;
	va_start(args, format);
	int _rc = vsprintf(&_buff[TRACE_HDR_SIZE], format, args);
	va_end(args);

	_buff[TRACE_HDR_SIZE+_rc] = '\n';
	_buff[TRACE_HDR_SIZE+_rc+1] = 0;

	fprintf(TRACE_OUT, "%s", _buff);
}


void TracePrint::Record(int line)
{
	SetLine(line);
	Print("RECORD POINT %s:%d", m_szFile.c_str(), line);
}


void TracePrint::FillHeader(char* buff, bool includeLine)
{
	struct timeb _tmb;    
	ftime(&_tmb);
	struct tm* _ptm = localtime(&_tmb.time);
	int _rc = 0;

	if(includeLine)
	{
		_rc = sprintf(buff, "%04d %02d:%02d:%02d.%03d %s(%s:%d)",
			ThreadID(), 
			_ptm->tm_hour, _ptm->tm_min, _ptm->tm_sec, _tmb.millitm,
			m_szFunctionName.c_str(),
			m_szFile.c_str(),
			m_line
			);
	}
	else
	{
		_rc = sprintf(buff, "%04d %02d:%02d:%02d.%03d %s(%s)",
			ThreadID(), 
			_ptm->tm_hour, _ptm->tm_min, _ptm->tm_sec, _tmb.millitm,
			m_szFunctionName.c_str(),
			m_szFile.c_str()
			);
	}

	buff[_rc] = ' ';
	buff[TRACE_HDR_SIZE-1] = ' ';
}


void TracePrint::SetLine(int line)
{
	if(!m_show)
		return;

	m_line = line;
}

std::string TracePrint::SplitFileName(const std::string& szName)
{
	int _pos = szName.find_last_of("/");
	if(_pos==std::string::npos)
		return szName;
	else
		return szName.substr(_pos+1);
}

std::string TracePrint::SplitFunctionName(const std::string& szName)
{
	int _str = -1;
	int _end = -1;
	for(int i=0; i<szName.size(); i++)
	{
		if(szName[i]==':')
			_str=i+1;

		if(szName[i]=='(')
		{
			_end = i;
			break;
		}
	}

	if( _str!=-1 && _end!=-1)	return szName.substr(_str, _end-_str);
	else						return szName;
}

int TracePrint::ThreadID()
{
	return pthread_self()%10000;
}


#endif
