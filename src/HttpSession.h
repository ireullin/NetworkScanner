#ifndef __HTTPSESSION__
#define __HTTPSESSION__

namespace Net{
namespace Http{
class HttpSession
{
private:
	Dictionary2<std::string, std::string> m_dSession;

public:
	HttpSession(const std::string& szContent)
	{
		StringPlus::SplitString _sp(szContent, '&');
	
		for(int i=0; i<_sp.Size(); i++)
		{
			int _pos = _sp[i].find_first_of("=");

			m_dSession.Add(
				_sp[i].substr(0, _pos),
				_sp[i].substr(_pos+1)
				);
		}
	}


	std::string operator[](const std::string& index)
	{
		if(m_dSession.ContainKey(index))
			return m_dSession[index];
		else
			return "";
	}
};
}
}
#endif
