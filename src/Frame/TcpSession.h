#ifndef __TCPSESSION__
#define __TCPSESSION__

#include <string>
#include <assert.h>
#include <time.h>
#include "SocketClass.h"
#ifdef _MSC_VER

#else
#include <sys/timeb.h>
#endif

namespace Net{

#pragma pack(1)
typedef struct
{
	int length;
}TcpHeader;

typedef struct
{
	int	checksum;
}TcpTrailer;

#ifdef __GNUC__
#pragma pack()
#else
#pragma pack(4)
#endif


class TcpSession
{
public:
	enum DISCONNECT_REASON
	{
		eLOCAL_DISCONNECTED=0,
		eREMOTE_DISCONNECTED,
		eNO_HEARTBEAT,
		eNOT_TCP_TYPE
	};

private:
	TcpSession(const TcpSession&);
	TcpSession& operator=(const TcpSession&);

	Socket		m_socket;
	bool		m_isConnecting;

	/// It's an interval to send heartbeat.
	int m_heartbeatSend;
	
	/// Occur disconnecting, if over time.
	int m_heartbeatRecv;

	/// Last received time.
	time_t m_tLastRecv;
	
	/// Last sent time.
	time_t m_tLastSend;

	inline int  RandomSeed();
	inline int  GenerateChecksum(int length);
	inline bool VerifyChecksum(int length, int checksum);
	inline void SendHeartbeat();
	inline void FristConnect();
	inline void Disconnect(DISCONNECT_REASON reason);

protected:
	TcpSession(int heartbeatSec);
	TcpSession(int socket, int heartbeatSec);
	Socket& GetSocket(){return m_socket;}
	
public:
	virtual ~TcpSession();
	void Connect(const std::string& szRomteIP, int RemotePort);
	void Connect(const std::string& szRomteIP, int RemotePort, int LocalPort);
	void Close();

	bool IsConnecting()
	{return m_isConnecting;}

	std::string RemoteIP()
	{return m_socket.RemoteIP();}

	int	RemotePort()
	{return m_socket.RemotePort();}

	std::string LocalIP()
	{return m_socket.LocalIP();}

	int	LocalPort()
	{return m_socket.LocalPort();}

	bool TcpNodelay()
	{return m_socket.TcpNodelay();}
	
	void TcpNodelay(bool on)
	{m_socket.TcpNodelay(on);}

	int  SendBuffer()
	{return m_socket.SendBuffer();}

	void SendBuffer(int size)
	{m_socket.SendBuffer(size);}

	int  RecvBuffer()
	{return m_socket.RecvBuffer();}

	void RecvBuffer(int size)
	{m_socket.RecvBuffer(size);}

	/// Be called on this session connected successful.
	virtual void OnConnected(){}
	
	/// Be called on this session received a completed data.
	virtual void OnRecvLine(std::string szData){}

	/// Be called on this session received a heartbeat.
	virtual void OnHeartbeat(){}

	/// Be called on this session disconnected or be disconnected.
	virtual void OnDisconnected(DISCONNECT_REASON reason){}

	/// Run it on a thread which was indicated by you to check if heartbeat was over time.
	void CheckHeartbeat();

	/// Run it on a thread which was indicated by you to check receiving data.
	void CheckEvent(double waitSecond=0);

	/// Send a data
	void SendLine(const std::string& szData);

};


void TcpSession::Close()
{
	if(m_isConnecting)
		Disconnect(eLOCAL_DISCONNECTED);
}


void TcpSession::Disconnect(DISCONNECT_REASON reason)
{
	m_isConnecting = false;
	m_socket.Close();
	OnDisconnected(reason);
}


void TcpSession::CheckHeartbeat()
{
	time_t _now;
	time(&_now);

	if( (_now-m_tLastRecv) > m_heartbeatRecv)
	{
		Disconnect(eNO_HEARTBEAT);
		return;
	}
	
	if( (_now-m_tLastSend) > m_heartbeatSend)
	{
		SendHeartbeat();
		time(&m_tLastSend);
	}
}


void TcpSession::CheckEvent(double waitSecond)
{
	if(!m_isConnecting)
		return;

	if(!m_socket.Select(waitSecond))
		return;

	int _size = m_socket.ReceivedBuffSize();
	if(_size<=0)
	{
		Disconnect(eREMOTE_DISCONNECTED);
		return;
	}

	if (_size > m_socket.RecvBuffer())
		throw SocketException("ReceiveBuffer may be too small.");

	int _lenHeader = sizeof(TcpHeader);
	if( _size <= _lenHeader)
		return;

	TcpHeader _header;
	m_socket.Recv((char*)&_header, _lenHeader, MSG_PEEK);
	int _lenBody	= ntohl(_header.length);
	int _lenTrailer = sizeof(TcpTrailer);
	int _lenTotal	= _lenHeader + _lenBody + _lenTrailer;

	if( _size < _lenTotal)
		return ;

	DataBlock _buff(_lenTotal);
	m_socket.Recv(_buff, _lenTotal, 0);

	TcpTrailer* _pTrailer = reinterpret_cast<TcpTrailer*>(_buff[_lenHeader+_lenBody]);
	
	int _checksum	= (int)ntohl( _pTrailer->checksum );
	
	if( !VerifyChecksum(_lenBody, _checksum) )
	{
		Disconnect(eNOT_TCP_TYPE);
		return;
	}

	if(_lenBody==0)
	{
		time(&m_tLastRecv);
		OnHeartbeat();
	}
	else
	{
		OnRecvLine( std::string(_buff[_lenHeader], _lenBody) );
	}
}


int TcpSession::RandomSeed()
{
#ifdef _MSC_VER
	srand(GetTickCount());
#else
	struct timeb _tmb;    
	ftime(&_tmb);
	srand(_tmb.millitm);
#endif
	int _seed = rand()%10000;
	return _seed==0 ? 1:_seed;
}


void TcpSession::SendLine(const std::string& szData)
{
	int _length	= szData.size();

	TcpHeader _header;
	_header.length = htonl(_length);

	TcpTrailer _trailer;
	_trailer.checksum = (int)htonl(GenerateChecksum(_length));

	std::string _szSend;
	_szSend.append( (char*)&_header, sizeof(TcpHeader));
	_szSend.append( szData);
	_szSend.append( (char*)&_trailer, sizeof(TcpTrailer));

	m_socket.Send(_szSend.c_str(), _szSend.size(), 0);
}


void TcpSession::SendHeartbeat()
{
	int _length	= 0;

	TcpHeader _header;
	_header.length = htonl(0);

	TcpTrailer _trailer;
	_trailer.checksum = (int)htonl(GenerateChecksum(_length));

	std::string _szSend;
	_szSend.append( (char*)&_header, sizeof(TcpHeader));
	_szSend.append( (char*)&_trailer, sizeof(TcpTrailer));

	m_socket.Send(_szSend.c_str(), _szSend.size(), 0);
}


bool TcpSession::VerifyChecksum(int length, int checksum)
{
	int _seed = checksum%10000;
	int _tmp1 = length % 10000;
	int _tmp2 = _tmp1*10000;
	int _newChecksum = _tmp2 + _seed;
	return _newChecksum==checksum;
}

int TcpSession::GenerateChecksum(int length)
{
	int _seed = RandomSeed();
	int _tmp1 = length % 10000;
	int _tmp2 = _tmp1 * 10000;
	return _tmp2 + _seed;
}

void TcpSession::FristConnect()
{
	m_isConnecting = true;
	time(&m_tLastRecv);
	time(&m_tLastSend);
}


void TcpSession::Connect(const std::string& szRomteIP, int RemotePort)
{
	m_socket.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP );
	m_socket.Connect(AF_INET, szRomteIP.c_str(), RemotePort);
	FristConnect();
	OnConnected();
}


void TcpSession::Connect(const std::string& szRomteIP, int RemotePort, int LocalPort)
{
	m_socket.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP );
	m_socket.Bind(AF_INET, LocalPort);
	m_socket.Connect(AF_INET, szRomteIP.c_str(), RemotePort);
	FristConnect();
	OnConnected();
}


TcpSession::~TcpSession()
{
}


TcpSession::TcpSession(int heartbeatSec)
{
	m_heartbeatRecv = heartbeatSec;
	m_heartbeatSend = heartbeatSec*0.3;
}


TcpSession::TcpSession(int socket, int heartbeatSec)
	:m_socket(socket)
{
	m_heartbeatRecv = heartbeatSec;
	m_heartbeatSend = heartbeatSec*0.3;
	FristConnect();
}

}//namespace
#endif /*__TCPSESSION__*/
