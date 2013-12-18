#ifndef __TCPHEADER__
#define __TCPHEADER__

#include <netinet/tcp.h>


namespace Net{
namespace Raw{

class TcpHeader
{
private:
	struct tcphdr m_tcphdr;

public:
	TcpHeader()
	{memset(&m_tcphdr, 0, sizeof(m_tcphdr));}

	// set
	void SourcePort(unsigned short value)			{m_tcphdr.source = htons(value);}
	void DestinationPort(unsigned short value)		{m_tcphdr.dest = htons(value);}
	void SequenceNumber(unsigned int value)			{m_tcphdr.seq = htonl(value);}
	void AcknowledgmentNumber(unsigned int value)	{m_tcphdr.ack_seq = htonl(value);}
	void Offset(unsigned char value)				{m_tcphdr.doff = value;}
	void FIN(bool value)							{m_tcphdr.fin = value?1:0;}
	void SYN(bool value)							{m_tcphdr.syn = value?1:0;}
	void RST(bool value)							{m_tcphdr.rst = value?1:0;}
	void PSH(bool value)							{m_tcphdr.psh = value?1:0;}
	void ACK(bool value)							{m_tcphdr.ack = value?1:0;}
	void URG(bool value)							{m_tcphdr.urg = value?1:0;}
	void Window(unsigned short value)				{m_tcphdr.window = htons(value);}
	void CheckSum(unsigned short value)				{m_tcphdr.check = value;}
	void UrgentPointer(unsigned short value)		{m_tcphdr.urg_ptr = htons(value);}


	// get
	unsigned short SourcePort()						{return ntohs(m_tcphdr.source);}
	unsigned short DestinationPort()				{return ntohs(m_tcphdr.dest);}
	unsigned int SequenceNumber()					{return ntohl(m_tcphdr.seq);}
	unsigned int AcknowledgmentNumber()				{return ntohl(m_tcphdr.ack_seq);}
	unsigned char Offset()							{return m_tcphdr.doff;}
	bool FIN()										{return m_tcphdr.fin!=0;}
	bool SYN()										{return m_tcphdr.syn!=0;}
	bool RST()										{return m_tcphdr.rst!=0;}
	bool PSH()										{return m_tcphdr.psh!=0;}
	bool ACK()										{return m_tcphdr.ack!=0;}
	bool URG()										{return m_tcphdr.urg!=0;}
	unsigned short Window()							{return ntohs(m_tcphdr.window);}
	unsigned short CheckSum()						{return m_tcphdr.check;}
	unsigned short UrgentPointer()					{return ntohs(m_tcphdr.urg_ptr);}
};

}
}
#endif
