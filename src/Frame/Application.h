#ifndef __APPLICATION__
#define __APPLICATION__

extern int main(int argc, char** argv, char** env);
class Application;

class ApplicationItem
{
private:
	friend class Application;
	std::string m_szValue;
	ApplicationItem(const std::string& value):m_szValue(value){}
	ApplicationItem(const ApplicationItem&);
	ApplicationItem& operator=(const ApplicationItem&);

public:
	std::string& ToString()
	{return m_szValue;}

	operator std::string&()
	{return m_szValue;}

	/// 注意:如果是不能轉型數字的參數也會回傳0
	int ToInt()
	{return atoi(m_szValue.c_str());}

	/// 注意:如果是不能轉型數字的參數也會回傳0
	double ToDouble()
	{return atof(m_szValue.c_str());}

	bool ToBoolean()
	{return StringPlus::ToLower(m_szValue)=="true";}
};


class Application
{
private:
	friend int main(int, char**, char**);
	std::vector<ApplicationItem*>				m_arrParams;
	Dictionary<std::string, ApplicationItem*>	m_mapEnv;
	ShareMemory									m_shareMemory;
	bool										m_released;
	std::string									m_name;
	std::string									m_absolutePath;

	Application(const Application&);
	Application& operator=(const Application&);

	void InitShareMemory();
	void ReleaseArray();
	void ReleaseDictionary();
	void ReleaseShareMemory();

	/// 傳入參數值
	void Set(char** argv, char** env);
	void Release();


public:
	Application():m_released(false){}
	~Application();

	/// 回傳應用程式的名稱
	std::string& Name();

	/// 回傳建構日期
	std::string BuildDate();

	/// 取得傳入參數
	ApplicationItem& Params(int index);

	/// 傳入參數的數量
	int ParamSize();

	/// 取得環境變數
	ApplicationItem& Env(const std::string& szKey);

	/// 環境變數的數量
	int EnvSize();

	/// 目前啟動了多少支程式
	int Users();

	/// 程式的絕對路徑
	std::string& AbsolutePath();

	std::string FullName()
	{ return AbsolutePath() + "/" + Name(); }

	/// 是否有這個環境變數
	bool ContainEnv(const std::string& szKey)
	{return m_mapEnv.ContainKey(szKey);}

	/// 取得環境變數
	ApplicationItem& operator[](const std::string& szKey)
	{return Env(szKey);}

	/// 取得傳入參數
	ApplicationItem& operator[](int index)
	{return Params(index);}

};


void Application::Set(char** argv, char** env)
{
	for( int i = 0; argv[i]!=NULL; ++i ) 
	{
		m_arrParams.push_back(new ApplicationItem(argv[i]));
	}


	for( int i = 0; env[i]!=NULL; ++i ) 
	{
		std::string _szTmp(env[i]);
		int _pos = _szTmp.find_first_of("=");
		if(_pos==std::string::npos)
			continue;

		std::string _szKey = _szTmp.substr(0, _pos);
		std::string _szVal = _szTmp.substr(_pos+1, _szTmp.size()-_pos-1);
		m_mapEnv.Add(_szKey, new ApplicationItem(_szVal) );
	}

	InitShareMemory();
}


Application::~Application()
{Release();}


void Application::Release()
{
	if(m_released)
		return;

	ReleaseArray();
	ReleaseDictionary();
	ReleaseShareMemory();
	m_released = true;
}

void Application::ReleaseArray()
{
	if(m_arrParams.size()==0)
		return;

	for(int i=0; i<m_arrParams.size(); i++)
	{
		ApplicationItem* _item = m_arrParams[i];
		delete _item;
	}

	m_arrParams.clear();
}


void Application::ReleaseDictionary()
{
	if(m_mapEnv.Size()==0)
		return;

	m_mapEnv.Reset();
	do{
		ApplicationItem* _item = m_mapEnv.GetCurrentValue();
		delete _item;
	}while(m_mapEnv.Next());

	m_mapEnv.Clear();
}


std::string& Application::AbsolutePath()
{
	if(m_absolutePath.empty())
	{
		char _buff[1024];
		memset(_buff, 0, sizeof(_buff));
		
		char _linkname[1024];
		memset(_linkname, 0, sizeof(_linkname));
	    
		pid_t _pid = getpid();
	    sprintf(_linkname, "/proc/%d/exe", _pid);

		int _size = readlink(_linkname, _buff, sizeof(_buff));
		if(_size<=0)
			throw FRAME_EXPCEPTION_1("couldn't get absolute path");

	    char* _abspath = dirname(_buff);
		m_absolutePath = static_cast<std::string>(_abspath);
	}

	return m_absolutePath;
}



std::string& Application::Name()
{
	if(m_name.empty())
	{
		std::string _tmp = (*m_arrParams[0]).ToString();
		int _pos = _tmp.find_last_of("/");
		if(_pos==std::string::npos)
			m_name = _tmp;
		else
			m_name = _tmp.substr(_pos+1);
	}

	return m_name;
}


std::string Application::BuildDate()
{
	return DateTime::Building().ToString();
}


ApplicationItem& Application::Params(int index)
{
	if(index>=m_arrParams.size())
		throw FRAME_EXPCEPTION_1("have no param.");

	return *m_arrParams[index];
}


int Application::ParamSize()
{
	return m_arrParams.size();
}


ApplicationItem& Application::Env(const std::string& szKey)
{
	if(!m_mapEnv.ContainKey(szKey))
	{
		std::string _szMsg = "can not find env " + szKey;
		throw FRAME_EXPCEPTION_1(_szMsg);
	}

	return *m_mapEnv[szKey];
}


int Application::EnvSize()
{
	return m_mapEnv.Size();
}


int Application::Users()
{
	ShareMemoryState _state;
	m_shareMemory.State(_state);
	return _state.CurrentNumber();
}


void Application::InitShareMemory()
{
	int _key = 0;
	std::string _szApp = Name();
	for(int i=0; i<_szApp.size(); i++)
		_key += _szApp[i]*i+1;

	m_shareMemory.Open(_key, _szApp.size());
	memcpy(m_shareMemory.Adderss(), _szApp.c_str(), _szApp.size());
}


void Application::ReleaseShareMemory()
{
	if(Users()==1)
		m_shareMemory.Erase();
	else
		m_shareMemory.Detach();
}


#endif
