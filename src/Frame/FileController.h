#ifndef __FILECONTROLLER__
#define __FILECONTROLLER__

#include "BufferFileIO.h"


class FileController : public Frame::Singleton<FileController>
{
private:
	friend class Frame::Singleton<FileController>;
	
	bool									m_continue;
	Dictionary<std::string, FileIO*>		m_dWriter;
	Threading::ThreadBind<FileController>*	m_tRoutine;
	Threading::Mutex						m_mutex;

	FileController()
		:m_tRoutine(NULL)
	{
		Start();
	}

	void Start()
	{
		m_continue = true;
		m_tRoutine = new Threading::ThreadBind<FileController>(&FileController::Routine, this);
		m_tRoutine->Start();
	}

	void Routine(void* lhs, void* rhs)
	{
		while(m_continue)
		{
			if(m_dWriter.Size()!=0)
			{
				m_mutex.Lock();

				m_dWriter.Reset();
				do{
					FileIO* _pWriter = m_dWriter.GetCurrentValue();
					_pWriter->CheckFlush();
				}while(m_dWriter.Next());

				m_mutex.Unlock();
			}

			Threading::Sleep(0.5);
		}
	}

	void PushToHash(const std::string& szName, FileIO* file)
	{
		Threading::AutoLock _auto(m_mutex);
		m_dWriter.Add(szName, file);
	}

public:
	~FileController()
	{
		m_continue = false;
		m_tRoutine->Join();

		if(m_dWriter.Size()==0)
			return;

		m_dWriter.Reset();
		do{
			FileIO* _pWriter = m_dWriter.GetCurrentValue();
			delete _pWriter;
		}while(m_dWriter.Next());
	}

	/// 如果檔案不存在就建立,如果存在就覆寫
	void OpenNew(const std::string& szName, const std::string& szPath, int bufferSize=0, int bufferWaitSecond=5)
	{
		if(bufferSize==0)	PushToHash(szName, new FileIO(szPath, false));
		else				PushToHash(szName, new BufferFileIO(szPath, false, bufferSize, bufferWaitSecond));
	}
	
	/// 如果檔案不存在就建立,如果存在就附加在後面
	void OpenAppend(const std::string& szName, const std::string& szPath, int bufferSize=0, int bufferWaitSecond=5)
	{
		if(bufferSize==0)	PushToHash(szName, new FileIO(szPath, true));
		else				PushToHash(szName, new BufferFileIO(szPath, true, bufferSize, bufferWaitSecond));
	}

	/// 取得對應的FileIO
	FileIO* Name(const std::string& szName)
	{
		if(!m_dWriter.ContainKey(szName)) 
		{
			std::string _szMsg = "Doesn't have the log writer named " + szName;
			throw FRAME_EXPCEPTION_1(_szMsg);
		}
		return m_dWriter[szName];
	}

	/// 檔案是否已被FileIO開啟
	bool Opened(const std::string& szName)
	{return m_dWriter.ContainKey(szName);}

};

#endif
