#ifndef __ICMPRAWSOCK__
#define __ICMPRAWSOCK__


class IcmpRawSocket : public Frame::Singleton<IcmpRawSocket>
{
private:
	friend class Frame::Singleton<IcmpRawSocket>;

	Net::Socket								m_sockIcmp;
	bool									m_continue;
	Threading::Mutex						m_mSocket;
	Threading::ThreadBind<IcmpRawSocket>*	m_tRecv;

	IcmpRawSocket(){}
	void ReceiveRoutine(void* lh, void* rh);

public:
	~IcmpRawSocket(){}
	void Init();
	void Release();
	void Send(const std::string& ip);
};


void IcmpRawSocket::Send(const std::string& ip)
{
	Threading::AutoLock _auto(m_mSocket);
	try
	{
		IcmpPackFactory _pack;
		const char* _buff = _pack.Generate();
		m_sockIcmp.SendTo(_buff, _pack.Size(), 0, AF_INET, ip, 0);
		LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Icmp,"%s", ip.c_str() );
	}
	catch(Net::SocketException& exp)
	{
		LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Icmp,"%s error occurred(%d)%s" , ip.c_str(), exp.no(), exp.what());
	}

}


void IcmpRawSocket::ReceiveRoutine(void* lh, void* rh)
{
	int _size = 0;
	std::string _szIP;
	int _port = 0;
	const int BUFF_SIZE=1024;
	int READ_SIZE = sizeof(Net::Raw::IpHeader) + sizeof(Net::Raw::IcmpHeader);
	char* _buff = new char[BUFF_SIZE];

	while(m_continue)
	{
		try{_size = m_sockIcmp.RecvFrom(_buff, BUFF_SIZE, 0, _szIP, _port);}
		catch(...){	_size = -1;	}

		if(_size != READ_SIZE)
			continue;

		DataCenter::Instance()->PushRecvBuff(_buff, _size);
	}
}


void IcmpRawSocket::Init()
{
	m_continue = true;

	m_sockIcmp.Create(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	m_sockIcmp.SendTimeout(10);
	m_sockIcmp.RecvTimeout(10);

	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard, "ICMP socket has initialed");

	m_tRecv = new Threading::ThreadBind<IcmpRawSocket>(&IcmpRawSocket::ReceiveRoutine, this);
	m_tRecv->Start();
}


void IcmpRawSocket::Release()
{
	m_continue = false;

	m_sockIcmp.Close();
	m_tRecv->Join();
	SAFE_DELETE(m_tRecv);

	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard, "ICMP socket has released");
}


#endif
