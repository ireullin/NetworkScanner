#ifndef __TCPACCEPTOR__
#define __TCPACCEPTOR__


#include "SocketClass.h"

namespace Net{

class TcpAcceptor
{
private:
	TcpAcceptor(const TcpAcceptor&);
	TcpAcceptor& operator= (const TcpAcceptor&);

	Socket m_socket;

protected:
	TcpAcceptor(){}

public:
	virtual ~TcpAcceptor(){}

	void Listen(int port, int backlog=5, bool reuse=true);
	void Run(double seconds=0);
	void Close(){m_socket.Close();}
	virtual void OnAccept(int socket){}

};

void TcpAcceptor::Listen(int port, int backlog, bool reuse)
{
	m_socket.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP );
	
	if(reuse)
		m_socket.ReuseAddress(reuse);
	
	m_socket.Bind(AF_INET, port);
	m_socket.Listen(backlog);
}

void TcpAcceptor::Run(double seconds)
{
	if(m_socket.Select(seconds))
	{
		int _socket = m_socket.Accept();
		OnAccept(_socket);
	}
}

}

#endif /*__TCPACCEPTOR__*/
