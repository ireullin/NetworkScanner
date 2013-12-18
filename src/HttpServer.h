#ifndef __HTTPSERVER__
#define __HTTPSERVER__


#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpSession.h"


namespace Net{
namespace Http{
class HttpServer : public Net::TcpAcceptor
{
private:
	std::deque<int>		m_deqSocket;
	Threading::Mutex	m_mutex;
	bool				m_ignoreException;

	std::string			ReadLine(Net::Socket& socket);
	void				ServiceSession();
	void				ServiceSessionEx();
	void				ExtractHeader(Net::Socket& socket, HttpRequest& httpRequest);
	void				ExtractContent(Net::Socket& socket, HttpRequest& httpRequest);
	void				SendError(Net::Socket& socket);

public:
	HttpServer(bool ignoreException=true);
	virtual ~HttpServer();

	virtual void OnAccept(int socket);
	virtual void OnConnected(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse,const std::string& srcIP, int srcPort);
	
};


HttpServer::HttpServer(bool ignoreException)
	:m_ignoreException(ignoreException)
{
}


HttpServer::~HttpServer()
{}


void HttpServer::OnConnected(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse,const std::string& srcIP, int srcPort)
{
	std::stringstream _ss;
	_ss << "<html><body>";
	_ss << "<h1>Have not been implemented</h1>";
	_ss << "<h2>" << srcIP << ":"  << srcPort << "</h2>";
	_ss << "<h2>" << DateTime::Now().ToString() << "</h2>";
	_ss << "</body></html>";
	httpResponse.Content(_ss.str());
}


void HttpServer::OnAccept(int socket)
{
	m_mutex.Lock();
	m_deqSocket.push_back(socket);
	m_mutex.Unlock();

	if(m_ignoreException)
		Frame::ThreadPool::Instance()->Invoke(Frame::Runable<HttpServer>::Wrap(&HttpServer::ServiceSessionEx, this));
	else
		Frame::ThreadPool::Instance()->Invoke(Frame::Runable<HttpServer>::Wrap(&HttpServer::ServiceSession, this));
}


std::string HttpServer::ReadLine(Net::Socket& socket)
{
	std::string _szResult;
	char _c = '\n';

	while(true)
	{
		if(!socket.Select())
			continue;
	
		int _rc = socket.Recv(&_c, 1, 0);
		if(_rc<=0)
			break;

		if(_c=='\r')
			continue;

		if(_c=='\n')
			break;

		_szResult.append(&_c, 1);
	}

	return _szResult;
}


void HttpServer::ExtractContent(Net::Socket& socket, HttpRequest& httpRequest)
{
	std::string _szLength = httpRequest["Content-Length"];
	if(_szLength.empty())
		return;

	int _len = atoi(_szLength.c_str());

	DataBlock _buff(_len);

	while(true)
	{
		if(!socket.Select())
			continue;
	
		int _size = socket.ReceivedBuffSize();
		if(_size <=0)
			break;

		if(_size < _len)
			continue;

		socket.Recv(_buff, _len, 0);
		break;
	}

	httpRequest.Content((const char*)_buff);
}


void HttpServer::ExtractHeader(Net::Socket& socket, HttpRequest& httpRequest)
{
	int i=0;
	while(true)
	{
		std::string _szLine = ReadLine(socket);
	
		if(_szLine.empty())
			break;

		if(i==0)
		{
			StringPlus::SplitString _sp1(_szLine, ' ');
			
			httpRequest.Method(_sp1[0]);
			httpRequest.URL(_sp1[1]);
			httpRequest.Version(_sp1[2]);
		}
		else
		{
			int _pos = _szLine.find_first_of(":");
			if(_pos == std::string::npos)
				throw "Sever Error 500";

			httpRequest.AddParams(
				_szLine.substr(0,_pos),
				_szLine.substr(_pos+2)
				);
		}
		i++;
	}
}

void HttpServer::ServiceSessionEx()
{
	try{ServiceSession();}
	catch(...){}
}

void HttpServer::ServiceSession()
{
	if(m_deqSocket.size()==0)
		return;

	m_mutex.Lock();
	int _handle = m_deqSocket.front();
	m_deqSocket.pop_front();
	m_mutex.Unlock();


	Net::Socket		_socket(_handle);
	HttpRequest		_request;
	HttpResponse	_response;

	try
	{
		ExtractHeader(_socket, _request);
		ExtractContent(_socket, _request);
	}
	catch(...)
	{
		SendError(_socket);
		_socket.Close();
		return;
	}

	OnConnected(_request, _response, _socket.RemoteIP(), _socket.RemotePort());

	std::string _szSend = _response.ToString();

	_socket.Send(_szSend.c_str(), _szSend.size(), 0);
	_socket.Close();
}


void HttpServer::SendError(Net::Socket& socket)
{
	HttpResponse _response;
	_response.Status("500");
	_response.Reason("Sever Error");

	std::string _szSend = _response.ToString();
	socket.Send(_szSend.c_str(), _szSend.size(), 0);
}
}
}
#endif
