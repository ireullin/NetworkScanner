#ifndef __HTTPRESPONSE__
#define __HTTPRESPONSE__

#include "HttpPackage.h"


namespace Net{
namespace Http{
class HttpResponse : public Net::Http::HttpPackage
{
public:
	HttpResponse()
	{
		m_arrHead.push_back("HTTP/1.1");
		m_arrHead.push_back("200");
		m_arrHead.push_back("OK");

		AddParams("Content-Type", "text/html; charset=UTF-8");
	}

	std::string Reason()const{return m_arrHead[2];}
	void Reason(const std::string& val){m_arrHead[2]=val;}
	
	std::string Status()const{return m_arrHead[1];}
	void Status(const std::string& val){m_arrHead[1]=val;}

	std::string Version()const{return m_arrHead[0];}
	void Version(const std::string& val){m_arrHead[0]=val;}
};
}
}
#endif
