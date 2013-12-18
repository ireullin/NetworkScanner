#ifndef __PSEUDOHEADER__
#define __PSEUDOHEADER__

//#include<netinet/tcp.h>
//#include<netinet/ip.h>

#include "TcpHeader.h"

namespace Net{
namespace Raw{

class PseudoHeader    //needed for checksum calculation
{
private:
    unsigned int m_source;
    unsigned int m_dest;
    unsigned char m_placeholder;
    unsigned char m_protocol;
    unsigned short m_tcpLength;
	Net::Raw::TcpHeader m_tcp;

public:
	void SourceIP(const std::string& szIP)		{m_source = inet_addr( szIP.c_str() );}
	void DestinationIP(const std::string& szIP)	{m_dest = inet_addr( szIP.c_str() );}
	void PlaceHolder(unsigned char value)		{m_placeholder=value;}
	void Protocol(unsigned char value)			{m_protocol=value;}
	void TcpLength(unsigned short value)		{m_tcpLength=htons(value);}
	void TcpHeader(Net::Raw::TcpHeader& value)	{memcpy(&m_tcp, &value, sizeof(Net::Raw::TcpHeader) );}


	std::string SourceIP()
	{
		unsigned char* _ip = (unsigned char*)&m_source;
		char _buff[20];
		sprintf(_buff, "%d.%d.%d.%d\0", _ip[0], _ip[1], _ip[2], _ip[3]);
		return std::string(_buff);
	}

	std::string DestinationIP()
	{
		unsigned char* _ip = (unsigned char*)&m_dest;
		char _buff[20];
		sprintf(_buff, "%d.%d.%d.%d\0", _ip[0], _ip[1], _ip[2], _ip[3]);
		return std::string(_buff);
	}
    
	unsigned char PlaceHolder()		{return m_placeholder;}
	unsigned char Protocol()		{return m_protocol;}
	unsigned short TcpLength()		{return ntohs(m_tcpLength);}
	Net::Raw::TcpHeader& TcpHeader(){return m_tcp;}
};

}
}
#endif
