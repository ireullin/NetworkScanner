#ifndef __RQWSOCKET__
#define __RQWSOCKET__

#include "IpHeader.h"
#include "TcpHeader.h"
#include "PseudoHeader.h"
#include "InetCheckSum.h"
#include "PackFactory.h"
#include "IcmpPackFactory.h"
#include "IcmpRawSock.h"


class RawSocket : public Frame::Singleton<RawSocket>
{
private:
	friend class Frame::Singleton<RawSocket>;

	Threading::ThreadBind<RawSocket>*	m_tRecv;
	Threading::ThreadBind<RawSocket>*	m_tSend;
	Net::Socket							m_sockSend;
	Net::Socket							m_sockRecv;
	bool								m_continue;
	Threading::Mutex					m_mSocket;
	Threading::Mutex					m_mInfo;
	std::string							m_localIP;

	RawSocket():m_tRecv(NULL),m_continue(false){}

	void ReceiveRoutine(void* lh, void* rh);
	void SendRoutine(void* lh, void* rh);
	void LockSend(SCAN_TYPE type, const char* buff, int size, const std::string& ip, int port);
	std::string GetLocalIP();


public:
	~RawSocket(){}
	void Init();
	void Release();
	const std::string& LocalIP()
	{return m_localIP;}
};


void RawSocket::LockSend(SCAN_TYPE type, const char* buff, int size, const std::string& ip, int port)
{
	Threading::AutoLock _auto(m_mSocket);
	try
	{
		m_sockSend.SendTo(buff, size, MSG_DONTWAIT, AF_INET, ip, port);
		
		switch(type)
		{
		case eSYN_SCANTYPE:
			LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Syn,"%s:%d", ip.c_str(), port );
			break;
		case eFIN_SCANTYPE:
			LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Fin,"%s:%d", ip.c_str(), port );
			break;
		case eSYNACK_SCANTYPE:
			LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::SynAck,"%s:%d", ip.c_str(), port );
			break;
		default:
			LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard,"UnknownType:%d %s:%d",type, ip.c_str(), port );
			break;
		}
	}
	catch(Net::SocketException& exp)
	{
		switch(type)
		{
		case eSYN_SCANTYPE:
			LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Syn,"%s:%d error occurred(%d)%s" , ip.c_str(), port , exp.no(), exp.what());
			break;
		case eFIN_SCANTYPE:
			LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Fin,"%s:%d error occurred(%d)%s", ip.c_str(), port , exp.no(), exp.what());
			break;
		case eSYNACK_SCANTYPE:
			LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::SynAck,"%s:%d error occurred(%d)%s", ip.c_str(), port , exp.no(), exp.what());
			break;
		default:
			LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard,"UnknownType:%d %s:%d error occurred(%d)%s",type, ip.c_str(), port , exp.no(), exp.what());
			break;
		}
	}
}


void RawSocket::SendRoutine(void* lh, void* rh)
{
	int _count = 0;
	while(m_continue)
	{
		if(DataCenter::Instance()->EmptyQueue())
		{
			Threading::Sleep(0.5);
			continue;
		}

		// 避免網路卡資源卡死
		if(_count==100)
		{
			_count = 0;
			Threading::Sleep(0.1);
			continue;
		}

		ScanInfoPtr _info = DataCenter::Instance()->PopQueue();
		
		if(_info->Type()==eSYN_SCANTYPE)
		{
			PackFactory _pack(m_localIP, _info->SrcPort(), _info->IP(), _info->DesPort() );
			const char* _buff = _pack.Generate_SYN();
			LockSend(_info->Type(), _buff, _pack.Size(), _info->IP(), _info->DesPort());
		}
		else if(_info->Type()==eFIN_SCANTYPE)
		{
			PackFactory _pack(m_localIP, _info->SrcPort(), _info->IP(), _info->DesPort() );
			const char* _buff = _pack.Generate_FIN();
			LockSend(_info->Type(), _buff, _pack.Size(), _info->IP(), _info->DesPort());
		}
		else if(_info->Type()==eSYNACK_SCANTYPE)
		{
			PackFactory _pack(m_localIP, _info->SrcPort(), _info->IP(), _info->DesPort() );
			const char* _buff = _pack.Generate_SYNACK();
			LockSend(_info->Type(), _buff, _pack.Size(), _info->IP(), _info->DesPort());
		}
		else if(_info->Type()==eICMP_SCANTYPE)
		{
			// 轉由別人代發
			IcmpRawSocket::Instance()->Send(_info->IP());
		}

		_info->SentTime(DateTime::Now());
		_info->State(eSENT);
		
		DataCenter::Instance()->PushSent(_info);		
		_count++;
	}
}


void RawSocket::ReceiveRoutine(void* lh, void* rh)
{
	const int BUFF_SIZE=65536;
	const int PACKAGE_SIZE = sizeof(Net::Raw::IpHeader) + sizeof(Net::Raw::TcpHeader);
	std::string _szIP;
	int _port = 0;
	int _size = 0;
	char* _buff = new char[BUFF_SIZE];

	while(m_continue)
	{
		try{_size = m_sockRecv.RecvFrom(_buff, BUFF_SIZE, 0, _szIP, _port);}
		catch(...){	_size = -1;	}
		
		if(_size < PACKAGE_SIZE)
			continue;

		if(DataCenter::Instance()->EmptySent())
			continue;

		Net::Raw::IpHeader* _pIphdr = (Net::Raw::IpHeader*)_buff;
		
		if(_pIphdr->Protocol()!=IPPROTO_TCP)
			continue;

		Net::Raw::TcpHeader* _pTcphdr = (Net::Raw::TcpHeader*)(_buff+sizeof(Net::Raw::IpHeader));

		DataCenter::Instance()->PushRecvBuff(_buff, _size);
	}

	m_sockRecv.Close();

	SAFE_DELETE_ARRAY(_buff);
}


void RawSocket::Release()
{
	m_continue = false;

	m_sockSend.Close();
	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard, "TCP send socket has released");
	
	m_sockRecv.Close();
	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard, "TCP receive socket has released");
	
	m_tRecv->Join();
	m_tSend->Join();

	SAFE_DELETE(m_tRecv);
	SAFE_DELETE(m_tSend);
}


void RawSocket::Init()
{
	m_continue = true;

	m_localIP = GetLocalIP();

	int _on=1;
	m_sockSend.Create(AF_INET, SOCK_RAW, IPPROTO_RAW);
	m_sockSend.RecvTimeout(10);
	m_sockSend.SendTimeout(10);
	m_sockSend.SetOption(IPPROTO_IP, IP_HDRINCL, (const char*)&_on, sizeof(_on));
	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard, "TCP send socket has initialed");

	m_sockRecv.Create(AF_INET , SOCK_RAW , IPPROTO_TCP);
	m_sockRecv.RecvTimeout(10);
	m_sockRecv.SendTimeout(10);
	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard, "TCP receive socket has initialed");

	m_tRecv = new Threading::ThreadBind<RawSocket>(&RawSocket::ReceiveRoutine, this);
	m_tRecv->Start();

	m_tSend = new Threading::ThreadBind<RawSocket>(&RawSocket::SendRoutine, this);
	m_tSend->Start();

}


std::string RawSocket::GetLocalIP()
{
	std::string _ip = "";
	try
	{
		Net::Socket _sock;
		_sock.Create(AF_INET, SOCK_DGRAM, 0);
		_sock.Connect(AF_INET, "8.8.8.8", 53); // google dns
		_ip = _sock.LocalIP();
		_sock.Close();
	}
	catch(...){}

	if(_ip=="")
	{
		LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard,"Cann't get local ip");
		throw FRAME_EXPCEPTION_1("cann't get local ip");
	}

	return _ip;
}

#endif
