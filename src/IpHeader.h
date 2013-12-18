#ifndef __IPHEADER__
#define __IPHEADER__


#include <netinet/ip.h>


namespace Net{
namespace Raw{

class IpHeader
{
private:
	struct iphdr m_iphdr;

public:
	IpHeader()
	{memset(&m_iphdr, 0, sizeof(m_iphdr));}

	// set
	void IHL(unsigned char value)				{m_iphdr.ihl = value;}
	void Version(unsigned char value)			{m_iphdr.version=value;}
	void TypeOfService(unsigned char value)		{m_iphdr.tos = value;}
	void TotalLength(unsigned short value)		{m_iphdr.tot_len = htons(value);}
	void ID(unsigned short value)				{m_iphdr.id = htons(value);}
	void FragmentOffset(unsigned short value)	{m_iphdr.frag_off = htons(value);}
	void TTL(unsigned char value)				{m_iphdr.ttl = value;}
	void Protocol(unsigned char value)			{m_iphdr.protocol = value;}
	void CheckSum(unsigned short value)			{m_iphdr.check = value;}
	void SourceIP(const std::string& szIP)		{m_iphdr.saddr = inet_addr( szIP.c_str() );}
	void DestinationIP(const std::string& szIP)	{m_iphdr.daddr = inet_addr( szIP.c_str() );}

	// get
	unsigned char IHL()							{return m_iphdr.ihl;}
	unsigned char Version()						{return m_iphdr.version;}
	unsigned char TypeOfService()				{return m_iphdr.tos;}
	unsigned short TotalLength()				{return ntohs(m_iphdr.tot_len);}
	unsigned short ID()							{return ntohs(m_iphdr.id);}
	unsigned short FragmentOffset()				{return ntohs(m_iphdr.frag_off);}
	unsigned char TTL()							{return m_iphdr.ttl;}
	unsigned char Protocol()					{return m_iphdr.protocol;}
	unsigned short CheckSum()					{return m_iphdr.check;}
	
	std::string SourceIP()
	{
		unsigned char* _ip = (unsigned char*)&m_iphdr.saddr;
		char _buff[20];
		sprintf(_buff, "%d.%d.%d.%d\0", _ip[0], _ip[1], _ip[2], _ip[3]);
		return std::string(_buff);
	}

	std::string DestinationIP()
	{
		unsigned char* _ip = (unsigned char*)&m_iphdr.daddr;
		char _buff[20];
		sprintf(_buff, "%d.%d.%d.%d\0", _ip[0], _ip[1], _ip[2], _ip[3]);
		return std::string(_buff);
	}
};

}
}
#endif
