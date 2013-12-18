#ifndef __HTTPREQUEST__
#define __HTTPREQUEST__

#include "HttpPackage.h"

namespace Net{
namespace Http{
class HttpRequest : public Net::Http::HttpPackage
{
public:
	HttpRequest()
	{
		m_arrHead.push_back("");
		m_arrHead.push_back("");
		m_arrHead.push_back("");
	}

	std::string Method()const{return m_arrHead[0];}
	void Method(const std::string& val){m_arrHead[0]=val;}
	
	std::string URL()const{return m_arrHead[1];}
	void URL(const std::string& val){m_arrHead[1]=val;}

	std::string Version()const{return m_arrHead[2];}
	void Version(const std::string& val){m_arrHead[2]=val;}
};
}
}
#endif
