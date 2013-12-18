#ifndef __HTTPPACKAGE__
#define __HTTPPACKAGE__

#include <vector>
#include <sstream>

namespace Net{
namespace Http{
class HttpPackage
{
protected:
	std::vector<std::string>				m_arrHead;
	Dictionary2<std::string, std::string>	m_hParams;
	std::string								m_szContent;

public:
	std::string operator[](const std::string& szKey)
	{
		std::string _szVal = "";
		m_hParams.TryGet(szKey, _szVal);
		return _szVal;
	}

	void AddParams(const std::string& szKey, const std::string& szValue)
	{
		if(m_hParams.ContainKey(szKey))
			m_hParams[szKey] = szValue;
		else
			m_hParams.Add(szKey, szValue);
	}

	std::string ToString()
	{
		if(!m_szContent.empty())
		{
			std::stringstream _ss;
			_ss << m_szContent.size();
			AddParams("Content-Length", _ss.str());
		}


		std::string _szResult;
		for(int i=0; i<m_arrHead.size(); i++)
		{
			_szResult.append(m_arrHead[i]);
			_szResult.append(" ");
		}

		_szResult.append("\n");
	
		for(m_hParams.Begin(); !m_hParams.End(); ++m_hParams)
		{
			_szResult.append(m_hParams.Key());
			_szResult.append(": ");
			_szResult.append(m_hParams.Value());
			_szResult.append(" \n");
		}

		_szResult.append("\n");

		if(!m_szContent.empty())
			_szResult.append(m_szContent);


		return _szResult;
	}

	std::string Content(){return m_szContent;}
	void Content(const std::string& val){m_szContent=val;}
};

}
}

#endif
