#ifndef __CONFIG__
#define __CONFIG__


#include "ConfigItem.h"


/*************************************************************************************
為了保持資料的完整性(空行,註解,順序等)
所以將所整個完整檔案存成Array,需要時在一個個找
資料重新存檔後行頭行尾的空格會消失
每插入一個新的pair會加在該Section的第一行
效能消耗最高的?在一個既有的Section中新增pair
其他修改原有的pair與新增section消耗的資源均比搜尋多一次RefreshIndex
同一個section中有兩個相同的key值,只會抓到第一個
**************************************************************************************/
class Config
{
private:
	friend class ConfigController;
	std::vector<ConfigItem*>				m_list;
	Dictionary<std::string,int>				m_index;
	std::string								m_szFileName;
	
	Config(const std::string& szFileName);
	void RefreshIndex();
	void ReleaseList();

public:
	~Config();
	std::string& FileName();
	void Save();
	void Reload();
	bool Contain(const std::string& szSection, const std::string& szKey);
	ConfigItem& Get(const std::string& szSection, const std::string& szKey);
	template <typename X> void Set(const std::string& szSection, const std::string& szKey, X value);
	
	// debug
	void Show();
};


Config::Config(const std::string& szFileName)
	:m_szFileName(szFileName)
{
	Reload();
}


Config::~Config()
{
	void ReleaseList();
}


std::string& Config::FileName()
{return m_szFileName;}

bool Config::Contain(const std::string& szSection, const std::string& szKey)
{
	if(!m_index.ContainKey(szSection))
		return false;

	int _str = m_index[szSection];
	for(int i=_str+1; i<m_list.size(); i++)
	{
		if(m_list[i]->Type() == ConfigItem::eSECTION)
			break;

		if(m_list[i]->Type() != ConfigItem::ePAIR)
			continue;

		if(m_list[i]->Key()==szKey)
			return true;
	}

	return false;
}


ConfigItem& Config::Get(const std::string& szSection, const std::string& szKey)
{
	if(!m_index.ContainKey(szSection))
		throw FRAME_EXPCEPTION_1("Dosen't have the section.");

	int _str = m_index[szSection];
	for(int i=_str+1; i<m_list.size(); i++)
	{
		if(m_list[i]->Type() == ConfigItem::eSECTION)
			break;

		if(m_list[i]->Type() != ConfigItem::ePAIR)
			continue;

		if(m_list[i]->Key()==szKey)
			return (*m_list[i]);
	}

	throw FRAME_EXPCEPTION_1("Dosen't have the key.");
}


template <typename X>  
void Config::Set(const std::string& szSection, const std::string& szKey, X value)
{
	std::stringstream _ss;
	_ss << szKey << "=" << value;

	if(!m_index.ContainKey(szSection))
	{
		m_list.push_back(new ConfigItem(""));
		m_list.push_back(new ConfigItem("[" + szSection + "]"));
		m_list.push_back(new ConfigItem(_ss.str()));
		RefreshIndex();
		return;
	}

	int _str = m_index[szSection];
	for(int i=_str+1; i<m_list.size(); i++)
	{
		
		if(m_list[i]->Type() == ConfigItem::eSECTION)
			break;

		if(m_list[i]->Type() != ConfigItem::ePAIR)
			continue;

		if(m_list[i]->Key()==szKey)
		{
			std::stringstream _ssVal;
			_ssVal << value;

			m_list[i]->SetValue(_ssVal.str());
			RefreshIndex();
			
			return;
		}
	}
		
	m_list.insert(m_list.begin()+m_index[szSection]+1, new ConfigItem(_ss.str()));
	RefreshIndex();
}


void Config::RefreshIndex()
{
	m_index.Clear();
	for(int i=0; i<m_list.size(); i++)
	{
		if(m_list[i]->Type()==ConfigItem::eSECTION)
		{
			m_index.Add( m_list[i]->Key(), i);
		}
	}
}


void Config::Reload()
{
	ReleaseList();

	std::ifstream _ifs(m_szFileName.c_str(), std::ifstream::in);
	if(!_ifs)
		return;
	
	/* 用>>會忽略空行
	std::string _szLine;
	while(_ifs>>_szLine)
	{
		m_list.push_back(new ConfigItem(_szLine));
	}*/

	char _buff[1024];
	while(!_ifs.eof())
	{
		memset(_buff, 0, sizeof(_buff));
		_ifs.getline(_buff, sizeof(_buff));
		m_list.push_back(new ConfigItem(_buff));
	}
	_ifs.close();

	RefreshIndex();
}


void Config::Save()
{
	std::ofstream _ofs(m_szFileName.c_str(), std::ios::out);
	if(!_ofs)
		return;
	
	for(int i=0; i<m_list.size(); i++)
	{
		_ofs <<  m_list[i]->InternalBuff() << "\n";
	}

	_ofs.close();
}


void Config::ReleaseList()
{
	for(int i=0; i<m_list.size(); i++)
	{
		ConfigItem* _item = m_list[i];
		SAFE_DELETE(_item);
	}
}


void Config::Show()
{
	for(int i=0; i<m_list.size(); i++)
	{
		std::cout << "{" <<  m_list[i]->Key() << "}" << std::endl;
	}
}


class ConfigController : public Frame::Singleton<ConfigController>
{
private:
	friend class Frame::Singleton<ConfigController>;

	Dictionary<std::string,Config*>  m_hConfig;

	ConfigController(){}

public:
	~ConfigController()
	{
		if(m_hConfig.Size()==0)
			return;

		m_hConfig.Reset();
		do{
			Config* _pConfig = m_hConfig.GetCurrentValue();
			SAFE_DELETE(_pConfig);
		}while(m_hConfig.Next());
	}
	
	void OpenFile(const std::string& szName, const std::string& szFilePath)
	{
		if(m_hConfig.Size()!=0)
		{
			if(m_hConfig.ContainKey(szName))
				throw FRAME_EXPCEPTION_1("Has had a same config of name.");

			m_hConfig.Reset();
			do{
				Config* _pConfig = m_hConfig.GetCurrentValue();

				if(_pConfig->FileName()==szFilePath)
					throw FRAME_EXPCEPTION_1("The file has been loaded.");

			}while(m_hConfig.Next());
		}
	
		Config* _pConfig = new Config(szFilePath);
		m_hConfig.Add(szName, _pConfig);
	}
	
	Config& Name(const std::string& szName)
	{return (*m_hConfig[szName]);}
};



#endif
