#ifndef __PACKFACTORY__
#define __PACKFACTORY__

#include "IpHeader.h"
#include "TcpHeader.h"
#include "PseudoHeader.h"
#include "InetCheckSum.h"



class PackFactory
{
private:
	char* m_buff;
	int IP_SIZE;
	int TCP_SIZE;

public:
	PackFactory(const std::string& srcIP, int srcPort, const std::string& dstIP, int dstPort);
	~PackFactory();
	int Size();
	Net::Raw::TcpHeader& TcpHeader();
	Net::Raw::IpHeader& IpHeader();
	const char* Generate();
	const char* Generate_SYN();
	const char* Generate_FIN();
	const char* Generate_SYNACK();
};


PackFactory::PackFactory(const std::string& srcIP, int srcPort, const std::string& desIP, int desPort)
{
	IP_SIZE = sizeof(Net::Raw::IpHeader);
	TCP_SIZE = sizeof(Net::Raw::TcpHeader);

	m_buff = new char[IP_SIZE+TCP_SIZE];
	memset(m_buff, 0, IP_SIZE+TCP_SIZE);
	
	IpHeader().IHL(5);
    IpHeader().Version(4);
    IpHeader().TypeOfService(0);
	IpHeader().TotalLength(IP_SIZE+TCP_SIZE);
	IpHeader().ID(54321);
	IpHeader().FragmentOffset(16384);
    IpHeader().TTL(64);
    IpHeader().Protocol(IPPROTO_TCP);
    IpHeader().CheckSum(0);
	IpHeader().SourceIP(srcIP);
    IpHeader().DestinationIP(desIP);
	

	TcpHeader().SourcePort(srcPort);
	TcpHeader().DestinationPort(desPort);
	TcpHeader().SequenceNumber(1105024978);
	TcpHeader().AcknowledgmentNumber(0);
	TcpHeader().Offset(TCP_SIZE / 4);
    //TcpHeader().FIN(false);
    //TcpHeader().SYN(false);
    //TcpHeader().RST(false);
    //TcpHeader().PSH(false);
    //TcpHeader().ACK(false);
    //TcpHeader().URG(false);
    TcpHeader().Window(14600);
    TcpHeader().CheckSum(0);
    TcpHeader().UrgentPointer(0);
}
	

PackFactory::~PackFactory()
{
	SAFE_DELETE_ARRAY(m_buff);
}


int PackFactory::Size()
{
	return IP_SIZE + TCP_SIZE;
}

const char* PackFactory::Generate()
{
	IpHeader().CheckSum( Net::Raw::CheckSum((unsigned short *) m_buff, IpHeader().TotalLength() >> 1));

	Net::Raw::PseudoHeader _pshdr;
	_pshdr.SourceIP( IpHeader().SourceIP() );
	_pshdr.DestinationIP( IpHeader().DestinationIP() );
	_pshdr.PlaceHolder(0);
	_pshdr.Protocol(IPPROTO_TCP);
	_pshdr.TcpLength( TCP_SIZE );
	_pshdr.TcpHeader( TcpHeader() );

	TcpHeader().CheckSum( Net::Raw::CheckSum((unsigned short*)&_pshdr, sizeof(Net::Raw::PseudoHeader)) );

	return m_buff;
}


const char* PackFactory::Generate_SYN()
{
	TcpHeader().SYN(true);
	return Generate();
}


const char* PackFactory::Generate_FIN()
{
	TcpHeader().FIN(true);
	return Generate();
}


const char* PackFactory::Generate_SYNACK()
{
	TcpHeader().SYN(true);
	TcpHeader().ACK(true);
	return Generate();
}


Net::Raw::TcpHeader& PackFactory::TcpHeader()
{
	return *((Net::Raw::TcpHeader*)(m_buff+IP_SIZE));
}


Net::Raw::IpHeader& PackFactory::IpHeader()
{
	return *((Net::Raw::IpHeader*)m_buff);
}


#endif
