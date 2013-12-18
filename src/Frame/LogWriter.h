#ifndef __LOGWRITER__
#define __LOGWRITER__

class LogWriter
{
private:

	std::string					m_szLogFile;
	Threading::Mutex			m_mutex;
	std::vector<std::string>	m_arrLog;
	std::ofstream				m_fileWriter;
	int							m_maxCount;
	double						m_buffTime;
	DateTime					m_lastFlush;

public:
	LogWriter(const std::string& szLogFile);
	
	~LogWriter();
	
	/// 將Log放入Buffer
	template <typename X> void WriteFormat(X header, const char* format, ...);
	//template <typename X> void WriteString(X header, const std::string& szLog);
	
	/// 寫入檔案
	void Flush();

	void CheckFlush();

	/// 多久沒寫寫一次
	double BuffTime()				{return m_buffTime;}
	void BuffTime(double buffTime)	{m_buffTime = buffTime;}
	
	/// 滿幾筆寫一次
	int MaxCount()					{return m_maxCount;}
	void MaxCount(int maxCount)		{m_maxCount = maxCount;}

	/// 寫到哪一個檔案
	std::string& FileName()			{return m_szLogFile;}
};


LogWriter::LogWriter(const std::string& szLogFile)
	:m_maxCount(10),m_buffTime(5),m_szLogFile(szLogFile)
{
	m_mutex.Lock();
	m_fileWriter.open(m_szLogFile.c_str(), std::ios::app|std::ios::in );
	if(!m_fileWriter)
	{
		m_mutex.Unlock();
		std::string _szErr = "Open log failed. File:" + m_szLogFile;
		throw FRAME_EXPCEPTION_1(_szErr);
	}

	std::string _szStart = "***************** Start recording " + DateTime::Now().ToString() + " *****************";
	m_arrLog.push_back(_szStart);
	m_mutex.Unlock();
	m_lastFlush = DateTime::Now();
}


LogWriter::~LogWriter()
{
	
	std::string _szStart = "***************** Stop recording " + DateTime::Now().ToString() + " *****************";
	m_arrLog.push_back(_szStart);

	Flush();
	m_fileWriter.close();
}


void LogWriter::CheckFlush()
{
	if( m_arrLog.size() > m_maxCount )
	{
		Flush();
		return;
	}

	DateTimeDiff _diff = DateTime::Now() - m_lastFlush;
	if(_diff.TatolSecond() > m_buffTime)
	{
		Flush();
		return;
	}
}


template <typename X> 
void LogWriter::WriteFormat(X header, const char* format, ...)
{
	char _acFormat[1024];
	memset(_acFormat, '0', sizeof(_acFormat));

	va_list args;
	va_start(args, format);
	vsprintf(_acFormat, format, args);
	va_end(args);

	std::string _szLog;
	_szLog.append(header());
	_szLog.append(_acFormat);

	m_mutex.Lock();
	m_arrLog.push_back(_szLog);
	m_mutex.Unlock();
}


//template <typename X> 
//void LogWriter::WriteString(X header, const std::string& szLog)
//{
//	std::string _szData;
//	_szData.append(header());
//	_szData.append(szLog); // 會當在這行?
//
//	m_mutex.Lock();
//	m_arrLog.push_back(_szData);
//	m_mutex.Unlock();
//}


void LogWriter::Flush()
{
	Threading::AutoLock _auto(m_mutex);

	std::vector<std::string>::iterator _it;
	for(_it=m_arrLog.begin(); _it!=m_arrLog.end(); ++_it)
	{
		const char* _pcLine = (*_it).c_str();
		int _size = (*_it).size();
		for(int i=0; i<_size; i++)
		{
			if(isprint(_pcLine[i])==0)
				m_fileWriter << "\\x" << StringPlus::ToHex(_pcLine[i]);
			else
				m_fileWriter.put(_pcLine[i]);
		}

		m_fileWriter.put('\n');
	}
	m_fileWriter.flush();

	m_arrLog.clear();

	m_lastFlush = DateTime::Now();
}



class LogController : public Frame::Singleton<LogController>
{
private:
	friend class Frame::Singleton<LogController>;
	
	bool									m_continue;
	Dictionary<std::string, LogWriter*>		m_dWriter;
	Threading::ThreadBind<LogController>*	m_tRoutine;

	LogController()
		:m_tRoutine(NULL)
	{}

	void Routine(void* lhs, void* rhs)
	{
		while(m_continue)
		{
			if(m_dWriter.Size()!=0)
			{
				m_dWriter.Reset();
				do{
					LogWriter* _pWriter = m_dWriter.GetCurrentValue();
					_pWriter->CheckFlush();
				}while(m_dWriter.Next());
			}

			Threading::Sleep(0.5);
		}
	}

public:
	~LogController()
	{
		m_continue = false;
		m_tRoutine->Join();

		if(m_dWriter.Size()==0)
			return;

		m_dWriter.Reset();
		do{
			LogWriter* _pWriter = m_dWriter.GetCurrentValue();
			delete _pWriter;
		}while(m_dWriter.Next());
	}

	void CreateLogWriter(const std::string& szName, const std::string& szPath)
	{
		LogWriter* _pWriter = new LogWriter(szPath);
		m_dWriter.Add(szName, _pWriter);
	}

	void Start()
	{
		m_continue = true;
		m_tRoutine = new Threading::ThreadBind<LogController>(&LogController::Routine, this);
		m_tRoutine->Start();
	}

	LogWriter& Name(const std::string& szName)
	{
		if(!m_dWriter.ContainKey(szName)) 
		{
			std::string _szMsg = "Doesn't have the log writer named " + szName;
			throw FRAME_EXPCEPTION_1(_szMsg);
		}
		return (*m_dWriter[szName]);
	}

	bool Contain(const std::string& szName)
	{return m_dWriter.ContainKey(szName);}

};

#endif
