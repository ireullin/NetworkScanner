#ifndef __CONFIGITEM__
#define __CONFIGITEM__

class Config;

class ConfigItem
{
public:
	enum ConfigItemType
	{
		eSECTION=0,
		ePAIR,
		eNOTE,
		eEMPTY
	};

private:
	friend class Config;

	std::string			m_szKey;
	std::string			m_szVal;
	ConfigItemType		m_type;

	ConfigItemType Type()
	{return m_type;}

	void SetValue(const std::string& szValue)
	{m_szVal=szValue;}


	ConfigItem(const std::string& szBuffer)
	{
		std::string _szBuff = StringPlus::Trim(szBuffer);

		// 场常琌フ
		if(_szBuff.empty())
		{
			m_szKey = _szBuff;
			m_type = eEMPTY;
			return;
		}

		// 爹秆
		if(_szBuff[0]=='#')
		{
			m_szKey = _szBuff;
			m_type = eNOTE;
			return;
		}

		// 戈
		int _pos = _szBuff.find_first_of("=");
		if(_pos!=std::string::npos)
		{
			m_szKey = _szBuff.substr(0,_pos);
			m_szVal = _szBuff.substr(_pos+1);
			m_type = ePAIR;
			return;
		}

		// section
		if(_szBuff[0]=='[' && _szBuff[_szBuff.size()-1]==']')
		{
			m_szKey = _szBuff.substr(1,_szBuff.size()-2);
			m_type = eSECTION;
			return;
		}

		throw FRAME_EXPCEPTION_1("Unkonwn configure format.");
	}

	std::string InternalBuff()
	{
		if(m_type==eSECTION)		return std::string("[" + m_szKey + "]");
		else if(m_type==ePAIR)		return std::string(m_szKey + "=" + m_szVal);
		else						return m_szKey;
	}

	std::string& Key()
	{return m_szKey;}

public:
	std::string& ToString()
	{return m_szVal;}

	operator std::string&()
	{return m_szVal;}

	/// 猔種:狦琌ぃ锣计把计穦肚0
	int ToInt()
	{return atoi(m_szVal.c_str());}

	/// 猔種:狦琌ぃ锣计把计穦肚0
	double ToDouble()
	{return atof(m_szVal.c_str());}

	const char* ToCharPtr()
	{return m_szVal.c_str();}

	bool ToBoolean()
	{return StringPlus::ToLower(m_szVal)=="true";}
};

#endif
