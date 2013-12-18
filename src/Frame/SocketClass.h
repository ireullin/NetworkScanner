#ifndef __SOCKETCLASS__
#define __SOCKETCLASS__

#ifdef _MSC_VER
/////////////////////////////////////////////
#include <Winsock2.h>
#include <ws2ipdef.h> 
#include <process.h>
#include <direct.h>
#include <time.h>
#pragma comment(lib, "ws2_32")
#define socklen_t int
/////////////////////////////////////////////
#else
/////////////////////////////////////////////
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
//#include <stdlib.h>

//#ifndef socklen_t
//#define socklen_t int
//#endif

/////////////////////////////////////////////
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <typeinfo>
#include <math.h>
#include <string.h>


namespace Net{

void SocketInit();
void SoecktTerminate();

class DataBlock
{
private:
	char* m_ptr;
public:
	DataBlock(int size){m_ptr = new char[size];}
	virtual ~DataBlock(){delete[] m_ptr;}
	operator char*(){return m_ptr;}
	char* operator[](int i){return &m_ptr[i];}
};


class SocketException : public std::exception
{
private:
	int m_errno;
	std::string m_errmsg;
public:
	SocketException(int error_no)
		:m_errno(error_no),m_errmsg(strerror(error_no)){}
	SocketException(const std::string& errmsg, int error_no=0)
		:m_errno(error_no),m_errmsg(errmsg){ }
	virtual ~SocketException()throw(){}
	virtual const char* what() const throw(){return m_errmsg.c_str();}
	int no(){return m_errno;}
};


/// 建立目的:
/// 消除呼叫時的struct
/// 消除作業系統差異
/// 錯誤回傳值在這邊就判斷完
class Socket
{
private:
	int m_socket;
	fd_set	m_socket_set;
	Socket(const Socket&);
	Socket& operator= (const Socket&);

	/// 可由系統變數errno取得的錯誤
	inline SocketException Exception(const char* file, int line);


public:
	Socket():m_socket(-1){}
	Socket(int socket):m_socket(socket){}
	virtual ~Socket(){};
	
	void Connect(int domain, const char* ip, int port);
#ifndef _MSC_VER
	void Connect(int domain, const char* ip, int port, double timeoutSec);
#endif


	void Bind(int domain, const char* ip, int port);
	void Bind(int domain, int port);

	void Create(int domain, int type, int protocol);

	int Accept();

	void Listen(int backlog);

	bool Select(double waitsecond=0);

	int Recv(char* buf, int len, int flags);

	int Send(const char* buf, int len, int flags);

	int Handle()
	{return m_socket;}

	int RecvFrom(char* buf, int len, int flags, std::string& szRemoteIP, int& remotePort);

	int SendTo(const char* buf, int len, int flags, int domain, const std::string& szRemoteIP, int remotePort);

	void SetOption(int level, int opt, const char* optval, int optlen);

	void GetOption(int level, int opt,       char* optval, int* optlen);

	int IOControl(int opt, int* val);

	int ReceivedBuffSize();

	bool Disconnected();

	bool IsValid();
	
	void Close();
		   
	void JoinGroup(const char* ip, int port);

	// Socket options
	std::string RemoteIP();
	std::string LocalIP();

	int			RemotePort();
	int			LocalPort();

	bool   TcpNodelay();
	void   TcpNodelay(bool on);

	//bool   TcpNodelayAck();
	//void   TcpNodelayAck(bool on);

	bool   ReuseAddress();
	void   ReuseAddress(bool on);
	
	bool   ReusePort();
	void   ReusePort(bool on);

	int    SendBuffer();
	void   SendBuffer(int size);
		   
	int    RecvBuffer();
	void   RecvBuffer(int size);

	double RecvTimeout();
	void   RecvTimeout(double second);

	double SendTimeout();
	void   SendTimeout(double second);

	int    TTL();
	void   TTL(int ttl);

	int    TOS();
	void   TOS(int tos);

	void Linger(bool enable, int second);
};


int Socket::Send(const char* buf, int len, int flags)
{
	int _rc = send(m_socket, buf, len, flags);
	if(_rc<0)	throw Exception(__FILE__, __LINE__);
	return _rc; 
}

int Socket::Recv(char* buf, int len, int flags)
{
	int _rc = recv(m_socket, buf, len, flags);
	if(_rc<0)	throw Exception(__FILE__, __LINE__);
	return _rc;
}

void Socket::JoinGroup(const char* ip, int port)
{
	struct ip_mreq _mreq; 
	memset(&_mreq, 0, sizeof(struct ip_mreq));
	_mreq.imr_multiaddr.s_addr = inet_addr(ip); 
	_mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	SetOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&_mreq, sizeof(_mreq));
}

int Socket::SendTo(const char* buf, int len, int flags, int domain, const std::string& szRemoteIP, int remotePort)
{
	struct sockaddr_in _addr;
	memset(&_addr, 0, sizeof(struct sockaddr_in));
	_addr.sin_family = domain;
	_addr.sin_port = htons( remotePort );
	_addr.sin_addr.s_addr = inet_addr( szRemoteIP.c_str() );

	int _rc = sendto(m_socket, buf, len, flags, (struct sockaddr*)&_addr, sizeof(sockaddr_in));
	if(_rc<0)	throw Exception(__FILE__, __LINE__);
	return _rc;
}


int Socket::RecvFrom(char* buf, int len, int flags, std::string& szRemoteIP, int& remotePort)
{
	int _addrSize = sizeof(sockaddr_in);
	struct sockaddr_in _addr;
	memset(&_addr, 0, sizeof(struct sockaddr_in));
	int _rc = recvfrom(m_socket, buf, len, flags, (struct sockaddr*)&_addr, (socklen_t*)&_addrSize);
	if(_rc<0)	throw Exception(__FILE__, __LINE__);

	char* _ip = inet_ntoa( _addr.sin_addr );
	szRemoteIP.clear();
	szRemoteIP.append(_ip);

	remotePort = ntohs(_addr.sin_port);
	
	return _rc;
}


int Socket::RecvBuffer()
{
	int _size = 0;
	int _len =sizeof(_size);
	GetOption(SOL_SOCKET, SO_RCVBUF, (char*)&_size, &_len);
	//if(_rc!=0)throw SocketException("Get RecvBuffer Failed", _rc);
	return _size;
}

void Socket::RecvBuffer(int size)
{
	SetOption(SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(int));
	//if(_rc!=0)throw SocketException("Set RecvBuffer Failed", _rc);
}

int Socket::SendBuffer()
{
	int _size = 0;
	int _len =sizeof(_size);
	GetOption(SOL_SOCKET, SO_SNDBUF, (char*)&_size, &_len);
	//if(_rc!=0)throw SocketException("Get SendBuffer Failed", _rc);
	return _size;
}

void Socket::SendBuffer(int size)
{
	SetOption(SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof(int));
	//if(_rc!=0)throw SocketException("Set SendBuffer Failed", _rc);
}


//bool Socket::TcpNodelayAck()
//{
//	int _on = 1;
//	int _len =sizeof(_on);
//	GetOption(IPPROTO_TCP,TCP_NODELAYACK,(char*)&_on, &_len);
//	return _on != 0;
//}
//
//
//void Socket::TcpNodelayAck(bool on)
//{
//	int _on = 0;
//	if(on)_on = 1;
//	SetOption(IPPROTO_TCP,TCP_NODELAYACK,(char*)&_on,sizeof(_on));
//}


bool Socket::TcpNodelay()
{
	int _on = 1;
	int _len =sizeof(_on);
	GetOption(IPPROTO_TCP,TCP_NODELAY,(char*)&_on, &_len);
	//if(_rc!=0)throw SocketException("Get TcpNodelay Failed", _rc);
	return _on != 0;
}

void Socket::TcpNodelay(bool on)
{
	int _on = 0;
	if(on)_on = 1;
	SetOption(IPPROTO_TCP,TCP_NODELAY,(char*)&_on,sizeof(_on));
	//if(_rc!=0)throw SocketException("Set TcpNodelay Failed", _rc);
}

void Socket::RecvTimeout(double second)
{
	struct timeval _tv;
	_tv.tv_sec = floor(second);
	_tv.tv_usec = second - floor(second);

	SetOption(SOL_SOCKET, SO_RCVTIMEO, (char*)&_tv, sizeof(_tv));
}


double Socket::RecvTimeout()
{
	struct timeval _tv;
	int _size = sizeof(_tv);
	GetOption(SOL_SOCKET, SO_RCVTIMEO, (char*)&_tv, &_size);
	return _tv.tv_sec + (_tv.tv_usec/1000);
}


double Socket::SendTimeout()
{
	struct timeval _tv;
	int _size = sizeof(_tv);
	GetOption(SOL_SOCKET, SO_SNDTIMEO, (char*)&_tv, &_size);
	return _tv.tv_sec + (_tv.tv_usec/1000);
}


void Socket::SendTimeout(double second)
{
	struct timeval _tv;
	_tv.tv_sec = floor(second);
	_tv.tv_usec = second - floor(second);

	SetOption(SOL_SOCKET, SO_SNDTIMEO, (char*)&_tv, sizeof(_tv));
}

void Socket::TTL(int ttl)
{
	SetOption(IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));
}

void Socket::Linger(bool enable, int second)
{
	struct linger _lgr;
	_lgr.l_linger = second;

	if(enable)	_lgr.l_onoff = 1;
	else		_lgr.l_onoff = 0;

	SetOption(SOL_SOCKET, SO_LINGER, (char*)&_lgr, sizeof(_lgr));
}

int Socket::TTL()
{
	int _ttl = 0;
	int _size = sizeof(_ttl);
	GetOption(IPPROTO_IP, IP_TTL, (char*)&_ttl, &_size);
	return _ttl;
}

bool Socket::ReusePort()
{
	throw SocketException("the command is not is linux");

	/*int _on = 1;
	int _len =sizeof(_on);
	GetOption(SOL_SOCKET,SO_REUSEPORT ,(char*)&_on, &_len);
	//if(_rc!=0)throw SocketException("Get ReuseAddress Failed", _rc);
	return _on != 0;
	*/
	return false;
}

void Socket::ReusePort(bool on)
{
	throw SocketException("the command is not is linux");

	/*
	int _on = 0;
	if(on)_on = 1;
	SetOption(SOL_SOCKET,SO_REUSEPORT ,(char*)&_on,sizeof(_on));
	//if(_rc!=0)throw SocketException("Set ReuseAddress Failed", _rc);
	*/
}

void Socket::TOS(int tos)
{
	SetOption(IPPROTO_IP, IP_TOS, (char*)&tos, sizeof(tos));
}


int Socket::TOS()
{
	int _tos = 0;
	int _size = sizeof(_tos);
	GetOption(IPPROTO_IP, IP_TOS, (char*)&_tos, &_size);
	return _tos;
}


bool Socket::ReuseAddress()
{
	int _on = 1;
	int _len =sizeof(_on);
	GetOption(SOL_SOCKET,SO_REUSEADDR,(char*)&_on, &_len);
	//if(_rc!=0)throw SocketException("Get ReuseAddress Failed", _rc);

	return _on != 0;
}

void Socket::ReuseAddress(bool on)
{
	int _on = 0;
	if(on)_on = 1;
	SetOption(SOL_SOCKET,SO_REUSEADDR,(char*)&_on,sizeof(_on));
	//if(_rc!=0)throw SocketException("Set ReuseAddress Failed", _rc);
}

bool Socket::Select(double waitsecond)
{
	FD_ZERO(&m_socket_set);
	FD_SET(m_socket, &m_socket_set);

	double _sec = floor(waitsecond);

	struct timeval timeout;
	timeout.tv_sec = (long)_sec;
	timeout.tv_usec = (long)((waitsecond-_sec)*1000*1000);

	int _rc = ::select(FD_SETSIZE, &m_socket_set, NULL, NULL, &timeout);
	
	if(_rc<0)
		return false;
	// 在select回傳boolean會比拋出exception好處理
	// throw Exception(__FILE__, __LINE__);

	if(_rc==0)
		return false;

	if(FD_ISSET( m_socket, &m_socket_set)>0 )
		return true;
	else
		return false;
}


int Socket::ReceivedBuffSize()
{
	int _val = 0;
	int _rc = IOControl(FIONREAD, &_val);
	if(_rc!=0) throw Exception(__FILE__, __LINE__);
	return _val;
}

int Socket::IOControl(int opt, int* val)
{
#ifdef _MSC_VER
	return ioctlsocket(m_socket, opt, (u_long*)val);
#else
	return ioctl(m_socket, opt, val);
#endif
}

void Socket::SetOption(int level, int opt, const char* optval, int optlen)
{
#ifdef _MSC_VER
  int _rc = setsockopt( m_socket, level, opt, optval, optlen);
#else
  int _rc =  setsockopt( m_socket, level, opt, optval, optlen);
#endif
  if(_rc!=0)throw Exception(__FILE__, __LINE__);
}

void Socket::GetOption(int level, int opt, char* optval, int* optlen)
{
#ifdef _MSC_VER
  int _rc = getsockopt( m_socket, level, opt, optval, optlen);
#else
  int _rc = getsockopt( m_socket, level, opt, optval, (socklen_t*)optlen);
#endif
  if(_rc!=0)throw Exception(__FILE__, __LINE__);
}

void Socket::Connect(int domain, const char* ip, int port)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = domain;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = inet_addr( ip );

	int _rc = connect( m_socket, reinterpret_cast < sockaddr* > ( &addr ), sizeof( addr ) );
	if(_rc!=0) throw Exception(__FILE__, __LINE__);
}


#ifndef _MSC_VER
void Socket::Connect(int domain, const char* ip, int port, double timeoutSec)
{
	int _rc = 0;
	bool _flag = true;

	// set non blocking
	int _argOld = fcntl(m_socket, F_GETFL, NULL); 
	int _argNew = _argOld | O_NONBLOCK;
	_rc = fcntl(m_socket, F_SETFL, _argNew); 
	if(_rc!=0)	throw Exception(__FILE__, __LINE__);

	// connect
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = domain;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = inet_addr( ip );
	connect( m_socket, reinterpret_cast < sockaddr* > ( &addr ), sizeof( addr ) );
	
	// select
	fd_set _set;
	FD_ZERO(&_set);
	FD_SET(m_socket, &_set);

	double _sec = floor(timeoutSec);

	struct timeval timeout;
	timeout.tv_sec = (long)_sec;
	timeout.tv_usec = (long)((timeoutSec-_sec)*1000*1000);

	_rc = select(FD_SETSIZE, NULL, &_set, NULL, &timeout);
	if(_rc<=0)	_flag = false;

	// undo setting
	_rc = fcntl(m_socket, F_SETFL, _argOld); 
	if(_rc!=0)	throw Exception(__FILE__, __LINE__);
	if(!_flag)	throw Exception(__FILE__, __LINE__);
}	
#endif


void Socket::Bind(int domain, int port)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = domain;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = INADDR_ANY;

	int _rc = bind(m_socket, reinterpret_cast < sockaddr* > ( &addr ), sizeof( addr ) );
	if(_rc!=0) throw Exception(__FILE__, __LINE__);
}


void Socket::Bind(int domain, const char* ip, int port)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = domain;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = inet_addr( ip );

	int _rc = bind(m_socket, reinterpret_cast < sockaddr* > ( &addr ), sizeof( addr ) );
	if(_rc!=0) throw Exception(__FILE__, __LINE__);
}


void Socket::Listen(int backlog)
{
	int _rc = listen(m_socket, backlog);
	if(_rc!=0) throw Exception(__FILE__, __LINE__);
}


int Socket::Accept()
{
	int _rc = accept( m_socket, 0, 0 );
	if(_rc<0)throw Exception(__FILE__, __LINE__);
	return _rc;
}


void Socket::Create(int domain, int type, int protocol)
{
	m_socket = socket( domain, type, protocol );
	if(m_socket<0)throw Exception(__FILE__, __LINE__);
}


bool Socket::Disconnected()
{
	char byte;
	return recv(m_socket, &byte, sizeof (byte), MSG_PEEK) <= 0;
}

void Socket::Close()
{ 
	shutdown( m_socket, 2 );
#ifdef _MSC_VER
	closesocket( m_socket );
#else
	close( m_socket );
#endif
}

bool Socket::IsValid()
{
#ifdef _MSC_VER
	return m_socket != INVALID_SOCKET;
#else
	return m_socket >= 0;
#endif
}


SocketException Socket::Exception(const char* file, int line)
{
#ifdef _MSC_VER
	int _err = WSAGetLastError();
	char _szMsg[256];
	memset(_szMsg, 0, sizeof(_szMsg));
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, _err, 0, _szMsg, sizeof(_szMsg), NULL);
	char _szError[512];
	memset(_szError, 0, sizeof(_szError));
	sprintf(_szError, "%s (%s:%d)", _szMsg, file, line);
	SocketException _errExc(_szError);

#else
	char _szError[512];
	memset(_szError, 0, sizeof(_szError));
	sprintf(_szError, "%s (%s:%d)", strerror(errno), file, line);
	SocketException _errExc(_szError);
#endif	

	return _errExc;
}

std::string Socket::RemoteIP()
{
	struct sockaddr_in _addr;
	memset(&_addr, 0, sizeof(struct sockaddr_in));
	socklen_t _size = sizeof(_addr);

	if( getpeername( m_socket,(struct sockaddr*)&_addr, &_size)<0)
		throw Exception(__FILE__, __LINE__);
		//return "unknown";

	char* _ip = inet_ntoa( _addr.sin_addr );
	std::string _szIP(_ip);
	return _szIP;
}

std::string Socket::LocalIP()
{
	struct sockaddr_in _addr;
	memset(&_addr, 0, sizeof(struct sockaddr_in));
	socklen_t _size = sizeof(_addr);

	if( getsockname( m_socket,(struct sockaddr*)&_addr, &_size)<0)
		throw Exception(__FILE__, __LINE__);
		//return "unknown";

	char* _ip = inet_ntoa( _addr.sin_addr );
	std::string _szIP(_ip);
	return _szIP;
}

int Socket::RemotePort()
{
	struct sockaddr_in _addr;
	memset(&_addr, 0, sizeof(struct sockaddr_in));
	socklen_t _size = sizeof(_addr);

	if( getpeername( m_socket,(struct sockaddr*)&_addr, &_size)<0)
		throw Exception(__FILE__, __LINE__);
		//return -1;

	return ntohs(_addr.sin_port);
}

int Socket::LocalPort()
{
	struct sockaddr_in _addr;
	memset(&_addr, 0, sizeof(struct sockaddr_in));
	socklen_t _size = sizeof(_addr);

	if( getsockname( m_socket,(struct sockaddr*)&_addr, &_size)<0)
		throw Exception(__FILE__, __LINE__);
		//return -1;

	return ntohs(_addr.sin_port);
}


void Initial()
{
#ifdef _MSC_VER
	WORD version = MAKEWORD( 2, 2 );
	WSADATA data;
	WSAStartup( version, &data );
#else
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset( &sa.sa_mask );
	sa.sa_flags = 0;
	sigaction( SIGPIPE, &sa, 0 );
#endif
}


void Terminate()
{
#ifdef _MSC_VER
	WSACleanup();
#endif
}



}// namespace
#endif /*__SOCKETCLASS__*/
