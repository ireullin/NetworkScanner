#ifndef __DATACENTER__
#define __DATACENTER__

#include "IpHeader.h"
#include "TcpHeader.h"
#include "IcmpHeader.h"
#include "ScanInfo.h"
#include "JSON.h"
#include "TTLMap.h"


class DataCenter : public Frame::Singleton<DataCenter>
{
private:
	friend class Frame::Singleton<DataCenter>;
	
	Threading::ThreadBind<DataCenter>*	m_tMapping;
	bool								m_continue;
	
	Threading::Mutex					m_mtxRecvBuff;
	Threading::Mutex					m_mtxQueue;
	Threading::Mutex					m_mtxSent;
	Threading::Mutex					m_mtxReceived;

	std::queue<std::string>				m_arrRecvBuff;	// 沒有iterator的問題
	std::deque<ScanInfoPtr>				m_arrQueue;		// 考慮到要push pop and iterator
	std::list<ScanInfoPtr>				m_arrSent;		// 主要以erase為主
	std::vector<ScanInfoPtr>			m_arrReceived;


	DataCenter(){}
	void Routine(void* lh, void* rh);
	bool CheckTimout();
	bool CheckData();

public:
	~DataCenter(){}
	void Init();
	void Release();

	// Queue
	std::string PeepQueue();
	void		PushQueue(ScanInfoPtr scaninfoptr);
	ScanInfoPtr	PopQueue();
	bool		EmptyQueue();
	int			QueueSize();

	// Sent
	void		PushSent(ScanInfoPtr scaninfoptr);
	std::string	PeepSent();
	void		RemoveSent( std::list<ScanInfoPtr>::iterator& it );
	bool		EmptySent();
	int			SentSize();
	
	// RecvBuff
	void		PushRecvBuff(const char* buff, int size);
	std::string PopRecvBuff();
	int			RecvBuffSize();
	
	// Received
	std::string	PeepReceived();
	void		PushReceived(ScanInfoPtr scaninfoptr);
	void		ClearReceived();
	int			ReceivedSize();
	
};

int DataCenter::ReceivedSize()
{return m_arrReceived.size();}


int	DataCenter::RecvBuffSize()
{return m_arrRecvBuff.size();}


int	DataCenter::SentSize()
{return m_arrSent.size();}


int	DataCenter::QueueSize()
{return m_arrQueue.size();}


std::string DataCenter::PeepQueue()
{
	Threading::AutoLock _auto(m_mtxQueue);

	JSON::Hash _summary;
	_summary.Push("size", m_arrQueue.size());

	if(m_arrQueue.empty())
		return _summary.ToString();

	JSON::Array _data;
	for(int i=0; i<m_arrQueue.size(); i++)
	{
		JSON::Hash _row;

		_row.PushString("ip",	m_arrQueue[i]->IP());
		
		if(m_arrQueue[i]->Type()==eICMP_SCANTYPE)
			_row.PushString("port","");
		else
			_row.Push("port",	m_arrQueue[i]->DesPort());
	

		switch(m_arrQueue[i]->Type())
		{
		case eSYN_SCANTYPE:		_row.PushString("type",	"SYN");		break;
		case eFIN_SCANTYPE:		_row.PushString("type",	"FIN");		break;
		case eSYNACK_SCANTYPE:	_row.PushString("type",	"SYN/ACK"); break;
		case eICMP_SCANTYPE:	_row.PushString("type",	"ICMP");	break;
		default:				_row.PushString("type",	"UKNOWN");	break;
		}

		_data.PushChild(_row);
	}

	_summary.PushChild("data", _data);
	return _summary.ToString();
}


void DataCenter::PushQueue(ScanInfoPtr scaninfoptr)
{
	Threading::AutoLock _auto(m_mtxQueue);
	m_arrQueue.push_back(scaninfoptr);
}


ScanInfoPtr DataCenter::PopQueue()
{
	Threading::AutoLock _auto(m_mtxQueue);
	ScanInfoPtr _info = m_arrQueue.front();
	m_arrQueue.pop_front();
	return _info;
}


bool DataCenter::EmptyQueue()
{
	return m_arrQueue.empty();
}


void DataCenter::PushSent(ScanInfoPtr scaninfoptr)
{
	Threading::AutoLock _auto(m_mtxSent);
	m_arrSent.push_back(scaninfoptr);
}


std::string DataCenter::PeepSent()
{
	Threading::AutoLock _auto(m_mtxSent);

	JSON::Hash _summary;
	_summary.Push("size", m_arrSent.size());

	if(m_arrSent.empty())
		return _summary.ToString();

	JSON::Array _data;
	std::list<ScanInfoPtr>::iterator _it;
	for(_it=m_arrSent.begin(); _it!=m_arrSent.end(); ++_it)
	{
		ScanInfoPtr _info = (*_it);
		JSON::Hash _row;

		_row.PushString("senttime",	_info->SentTime().ToString());
		_row.PushString("ip",		_info->IP());
		
		if(_info->Type()==eICMP_SCANTYPE)
			_row.PushString("port","");
		else
			_row.Push("port",			_info->DesPort());

		switch(_info->Type())
		{
		case eSYN_SCANTYPE:		_row.PushString("type",	"SYN");		break;
		case eFIN_SCANTYPE:		_row.PushString("type",	"FIN");		break;
		case eSYNACK_SCANTYPE:	_row.PushString("type",	"SYN/ACK"); break;
		case eICMP_SCANTYPE:	_row.PushString("type",	"ICMP");	break;
		default:				_row.PushString("type",	"UKNOWN");	break;
		}


		_data.PushChild(_row);
	}

	_summary.PushChild("data", _data);
	return _summary.ToString();
}


void DataCenter::RemoveSent( std::list<ScanInfoPtr>::iterator& it )
{
	Threading::AutoLock _auto(m_mtxSent);
	it = m_arrSent.erase(it);
	--it;
}


bool DataCenter::EmptySent()
{return m_arrSent.empty();}

void DataCenter::PushRecvBuff(const char* buff, int size)
{
	Threading::AutoLock _auto(m_mtxRecvBuff);
	std::string _buff(buff, size);
	m_arrRecvBuff.push(_buff);
}


std::string DataCenter::PopRecvBuff()
{
	Threading::AutoLock _auto(m_mtxRecvBuff);
	std::string _buff = m_arrRecvBuff.front();
	m_arrRecvBuff.pop();
	return _buff;
}


std::string	DataCenter::PeepReceived()
{
	Threading::AutoLock _auto(m_mtxReceived);

	JSON::Hash _summary;
	_summary.Push("size", m_arrReceived.size());

	if(m_arrReceived.empty())
		return _summary.ToString();

	JSON::Array _data;
	for(int i=m_arrReceived.size()-1; i>=0; i--)
	{
		JSON::Hash _row;

		_row.PushString("senttime",	m_arrReceived[i]->SentTime().ToString());
		_row.PushString("recvtime",	m_arrReceived[i]->RecvTime().ToString());
		_row.PushString("ip",		m_arrReceived[i]->IP());
		
		
		int _ttl = m_arrReceived[i]->TTL();
		switch(m_arrReceived[i]->State())
		{
		case eOPENED:	
			_row.PushString("status", "OPENING");	
			_row.Push("ttl", _ttl);
			_row.PushString("os", g_ttlmap.TcpUdpInfo(_ttl) );
			break;

		case eCLOSE:	
			_row.PushString("status", "CLOSED");
			_row.Push("ttl", _ttl);
			_row.PushString("os", g_ttlmap.TcpUdpInfo(_ttl) );
			break;

		case eTIMEOUT:	
			_row.PushString("status", "TIMEOUT");	
			_row.PushString("ttl", "");
			_row.PushString("os", "");
			break;

		case eEXISTED:
			_row.PushString("status", "EXISTED");
			_row.Push("ttl", _ttl);
			_row.PushString("os", "");
			_row.PushString("os", g_ttlmap.IcmpInfo(_ttl));
			break;

		default:		
			_row.PushString("status", "UKNOWN");
			_row.PushString("ttl", "");
			_row.PushString("os", "");
			break;
		}

		switch(m_arrReceived[i]->Type())
		{
		case eSYN_SCANTYPE:
			_row.Push("port", m_arrReceived[i]->DesPort());
			_row.PushString("type", "SYN");
			break;
		case eFIN_SCANTYPE:
			_row.Push("port", m_arrReceived[i]->DesPort());
			_row.PushString("type", "FIN");
			break;
		case eSYNACK_SCANTYPE:
			_row.Push("port", m_arrReceived[i]->DesPort());
			_row.PushString("type", "SYN/ACK");
			break;
		case eICMP_SCANTYPE:
			_row.PushString("port", "");
			_row.PushString("type", "ICMP");
			break;
		default:
			_row.PushString("port", "");
			break;
		}

		_data.PushChild(_row);
	}

	_summary.PushChild("data", _data);
	return _summary.ToString();
}


void DataCenter::PushReceived(ScanInfoPtr scaninfoptr)
{
	Threading::AutoLock _auto(m_mtxReceived);
	m_arrReceived.push_back(scaninfoptr);
}


void DataCenter::ClearReceived()
{
	Threading::AutoLock _auto(m_mtxReceived);
	m_arrReceived.clear();
}

bool DataCenter::CheckTimout()
{
	if(m_arrSent.empty())
		return false;

	std::list<ScanInfoPtr>::iterator _it;
	for (_it = m_arrSent.begin(); _it!=m_arrSent.end(); ++_it)
	{
		bool _correct = true;
		ScanInfoPtr _info = (*_it);
		DateTimeDiff _diff = DateTime::Now() - _info->SentTime();

		// icmp scan timeout
		if(_diff.TatolSecond()>_info->Timeout() && _info->Type()==eICMP_SCANTYPE)
		{
			_info->State(eTIMEOUT);
		}

		// syn scan timeout
		else if(_diff.TatolSecond()>_info->Timeout() && _info->Type()==eSYN_SCANTYPE)
		{
			_info->State(eTIMEOUT);
		}

		// fin scan timeout(open)
		else if(_diff.TatolSecond()>_info->Timeout() && _info->Type()==eFIN_SCANTYPE)
		{
			_info->State(eTIMEOUT);
		}

		// syn/ack scan timeout(open)
		else if(_diff.TatolSecond()>_info->Timeout() && _info->Type()==eSYNACK_SCANTYPE)
		{
			_info->State(eTIMEOUT);
		}
		else
			_correct = false;

		if(_correct)
		{
			_info->RecvTime(DateTime::Now());
			RemoveSent(_it);
			PushReceived(_info);
		}
	}

	return true;
}


bool DataCenter::CheckData()
{
	if(m_arrRecvBuff.empty())
		return false;

	std::string _recvbuff = PopRecvBuff();

	Net::Raw::TcpHeader* _pTcphdr = NULL;
	Net::Raw::IcmpHeader* _pIcmphdr = NULL;
	Net::Raw::IpHeader* _pIphdr = (Net::Raw::IpHeader*)_recvbuff.c_str();


	if(_pIphdr->Protocol()==IPPROTO_TCP)
		_pTcphdr = (Net::Raw::TcpHeader*)(_recvbuff.c_str()+sizeof(Net::Raw::IpHeader));
	else if(_pIphdr->Protocol()==IPPROTO_ICMP)
		_pIcmphdr= (Net::Raw::IcmpHeader*)(_recvbuff.c_str()+sizeof(Net::Raw::IpHeader));
	else
		return true;


	std::list<ScanInfoPtr>::iterator _it;
	for (_it = m_arrSent.begin(); _it!=m_arrSent.end(); ++_it)
	{
		bool _correct = true;
		ScanInfoPtr _info = (*_it);
		DateTimeDiff _diff = DateTime::Now() - _info->SentTime();

		// icmp scan open
		if(_pIcmphdr!=NULL && _info->Type()==eICMP_SCANTYPE && _pIphdr->SourceIP()==_info->IP() && _pIcmphdr->Type()==Net::Raw::eECHO_REPLY )
		{
			_info->State(eEXISTED);
			_info->TTL(_pIphdr->TTL());
		}

		// syn scan open
		else if(_pTcphdr!=NULL && _info->Type()==eSYN_SCANTYPE && _pIphdr->SourceIP()==_info->IP() && _pTcphdr->SourcePort()==_info->DesPort() && _pTcphdr->SYN() && _pTcphdr->ACK())
		{
			_info->State(eOPENED);
			_info->TTL(_pIphdr->TTL());
		}

		// fin scan close
		else if(_pTcphdr!=NULL && _info->Type()==eFIN_SCANTYPE && _pIphdr->SourceIP()==_info->IP() && _pTcphdr->SourcePort()==_info->DesPort() && _pTcphdr->RST())
		{
			_info->State(eCLOSE);
			_info->TTL(_pIphdr->TTL());
		}

		// syn/ack scan close
		else if(_pTcphdr!=NULL && _info->Type()==eSYNACK_SCANTYPE && _pIphdr->SourceIP()==_info->IP() && _pTcphdr->SourcePort()==_info->DesPort() && _pTcphdr->RST())
		{
			_info->State(eCLOSE);
			_info->TTL(_pIphdr->TTL());
		}
		else
			_correct = false;

		if(_correct)
		{
			_info->RecvTime(DateTime::Now());
			RemoveSent(_it);
			PushReceived(_info);
		}
	}

	return true;
}

void DataCenter::Routine(void* lh, void* rh)
{
	while(m_continue)
	{
		if(!CheckTimout())
		{
			Threading::Sleep(0.5);
			continue;
		}

		if(!CheckData())
		{
			Threading::Sleep(0.5);
			continue;
		}

	}// while
}// method


void DataCenter::Init()
{
	m_continue = true;
	m_tMapping = new Threading::ThreadBind<DataCenter>(&DataCenter::Routine, this);
	m_tMapping->Start();

	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard, "Data center has initialed");
}


void DataCenter::Release()
{
	m_continue = false;
	m_tMapping->Join();
	SAFE_DELETE(m_tMapping);

	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard, "Data center has released");
}


#endif
