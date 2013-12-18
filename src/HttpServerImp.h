#ifndef __HTTPSERVERIMP__
#define __HTTPSERVERIMP__

#include "HttpServer.h"
#include "MaskCalculator.h"


class HttpServerImp : public Net::Http::HttpServer, public Frame::Singleton<HttpServerImp>
{
private:
	friend class Frame::Singleton<HttpServerImp>;
	typedef Net::Http::HttpServer super;


	Threading::ThreadBind<HttpServerImp>*	m_tAccetp;
	bool									m_continue;

	HttpServerImp():m_tAccetp(NULL),m_continue(false){}

	std::string ReadFile(const std::string& filename);

	void Routine(void* lh, void* rh);
	
	/// default method
	void WebIndex(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse);

	///
	void WebRequestFile(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse);
	
	/// scan method
	void WebScan(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse);

	/// Queue method
	void WebQueue(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse);

	/// Sent method
	void WebSent(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse);

	/// result method
	void WebResult(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse);

	///
	void WebError(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse, const std::string& srcIP, int srcPort, const std::string& msg);

	/// clear received array
	void WebClear(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse);

	/// get about info
	void WebAbout(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse);

public:
	~HttpServerImp(){}
	
	/// 啟動web server
	void Init(int port);
	
	/// 停止web server
	void Release();
	
	/// 當有一個session連接上時會被呼叫
	virtual void OnConnected(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse,const std::string& srcIP, int srcPort);
};



std::string HttpServerImp::ReadFile(const std::string& filename)
{
	char _buff[1024*100];
	memset(_buff, 0, sizeof(_buff));
	std::ifstream _if(filename.c_str());
	if(!_if.good()) throw FRAME_EXPCEPTION_1("open index.html failed");
	_if.read(_buff, sizeof(_buff));
	_if.close();
	return std::string(_buff);
}


// example
// http://192.168.20.119:18000
void HttpServerImp::WebIndex(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse)
{
	httpResponse.Content(ReadFile( g_app.AbsolutePath() + "/html/index.html"));
}


// example
// http://192.168.20.119:18000/style.css
void HttpServerImp::WebRequestFile(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse)
{
	std::string _file = g_app.AbsolutePath() + "/html" + httpRequest.URL();
	if(!FileInfo::IsExist(_file))
	{
		throw FRAME_EXPCEPTION_1("404 page not found");
	}

	std::string _ext = FileInfo::Extension(_file);
	if(_ext=="")
	{
		throw FRAME_EXPCEPTION_1("404 page not found");
	}
	else if(_ext==".css")
	{
		httpResponse.AddParams("Content-Type", "text/css; charset=UTF-8");
	}
	else if(_ext==".html" || _ext==".htm")
	{
		httpResponse.AddParams("Content-Type", "text/html; charset=UTF-8");
	}
	else if(_ext==".js")
	{
		httpResponse.AddParams("Content-Type", "text/javascript; charset=UTF-8");
	}
	else
	{
		throw FRAME_EXPCEPTION_1("404 page not found");
	}

	httpResponse.Content(ReadFile(_file));
}


// example
// http://192.168.20.119:18000/about
void HttpServerImp::WebAbout(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse)
{
	JSON::Hash _summary;
	_summary.PushString("built",	g_about.Built().ToString() );
	_summary.PushString("version",	g_about.Version());
	_summary.PushString("author",	g_about.Author());
	_summary.PushString("mail",		g_about.Mail());
	httpResponse.Content( _summary.ToString() );
}


// example
// http://192.168.20.119:18000/clear
void HttpServerImp::WebClear(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse)
{
	DataCenter::Instance()->ClearReceived();
	
	std::stringstream _ss;
	_ss << "<html><body>";
	_ss << "<h1>cleared</h1>";
	_ss << "<p>" << DateTime::Now().ToString() << "</p>";
	_ss	<< "</body></html>";
	httpResponse.Content(_ss.str());
}


// example
// http://192.168.20.119:18000/result
void HttpServerImp::WebResult(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse)
{
	std::string _buff = DataCenter::Instance()->PeepReceived();
	httpResponse.Content(_buff);
}


// example
// http://192.168.20.119:18000/queue
void HttpServerImp::WebQueue(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse)
{
	std::string _buff = DataCenter::Instance()->PeepQueue();
	httpResponse.Content(_buff);
}


// example
// http://192.168.20.119:18000/sent
void HttpServerImp::WebSent(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse)
{
	std::string _buff = DataCenter::Instance()->PeepSent();
	httpResponse.Content(_buff);
}


// example
// http://192.168.20.119:18000/scan?ip=192.168.20.114&mask=24&local_port=19000&begin_port=1&end_port=30&timeout=5&type=SYN
void HttpServerImp::WebScan(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse)
{
	std::string _url = httpRequest.URL();
	int _pos = _url.find_first_of("?");
	Net::Http::HttpSession _session(_url.substr(_pos+1));

	std::string _ip = _session["ip"];
	int _mask = atoi(_session["mask"].c_str());
	int _localPort = atoi(_session["local_port"].c_str());
	int _beginPort = atoi(_session["begin_port"].c_str());
	int _endPort = atoi(_session["end_port"].c_str());
	int _timeout = atoi(_session["timeout"].c_str());
	std::string _type = _session["type"];


	Net::MaskCaculator _mc(_ip, _mask);
	for(int i=0; i<_mc.Size(); i++)
	{
		if(_type=="ICMP")
		{
			ScanInfoPtr _scaninfoptr(new ScanInfo());
			_scaninfoptr->IP(_mc[i]);
			_scaninfoptr->SrcPort(0);
			_scaninfoptr->DesPort(0);
			_scaninfoptr->State(eQUEUE);
			_scaninfoptr->Timeout(_timeout);
			_scaninfoptr->Type(eICMP_SCANTYPE);
			DataCenter::Instance()->PushQueue(_scaninfoptr);
			continue;
		}

		for(int j=_beginPort; j<=_endPort; j++)
		{
			ScanInfoPtr _scaninfoptr(new ScanInfo());
			_scaninfoptr->IP(_mc[i]);
			_scaninfoptr->SrcPort(_localPort);
			_scaninfoptr->DesPort(j);
			_scaninfoptr->State(eQUEUE);
			_scaninfoptr->Timeout(_timeout);

			if(_type=="SYN")
				_scaninfoptr->Type(eSYN_SCANTYPE);
			else if(_type=="FIN")
				_scaninfoptr->Type(eFIN_SCANTYPE);
			else if(_type=="SYNACK")
				_scaninfoptr->Type(eSYNACK_SCANTYPE);
			else
				throw FRAME_EXPCEPTION_1("unknown scan type");

			DataCenter::Instance()->PushQueue(_scaninfoptr);
		}
	}

	std::stringstream _ss;
	_ss << "<html><body>";
	_ss << "<h1>completed</h1>";
	_ss << "<p>" << DateTime::Now().ToString() << "</p>";
	_ss	<< "</body></html>";
	httpResponse.Content(_ss.str());
}


void HttpServerImp::WebError(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse, const std::string& srcIP, int srcPort, const std::string& msg)
{
	std::stringstream _ss;
	_ss << "<html><body>";
	_ss << "<h1>" << msg << "</h1>";
	_ss << "<p>last connected at " << DateTime::Now().ToString();
	_ss << " from " << srcIP << ":" << srcPort << "</p>";
	_ss	<< "</body></html>";
	httpResponse.Content(_ss.str());
}


void HttpServerImp::OnConnected(Net::Http::HttpRequest& httpRequest, Net::Http::HttpResponse& httpResponse,const std::string& srcIP, int srcPort)
{
	try
	{
		std::string _url = httpRequest.URL();

		LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Http, "%s", _url.c_str() );

		if(_url=="/")						WebIndex(httpRequest, httpResponse);
		else if(_url.find("/scan")==0)		WebScan(httpRequest, httpResponse);
		else if(_url.find("/queue")==0)		WebQueue(httpRequest, httpResponse);
		else if(_url.find("/sent")==0)		WebSent(httpRequest, httpResponse);
		else if(_url.find("/result")==0)	WebResult(httpRequest, httpResponse);
		else if(_url.find("/clear")==0)		WebClear(httpRequest, httpResponse);
		else if(_url.find("/about")==0)		WebAbout(httpRequest, httpResponse);
		else								WebRequestFile(httpRequest, httpResponse);
	}
	catch(std::exception& exp)
	{
		WebError(httpRequest, httpResponse, srcIP, srcPort, exp.what());
	}
	catch(...)
	{
		WebError(httpRequest, httpResponse, srcIP, srcPort, "sorry, some problems has occurred");
	}
}


void HttpServerImp::Routine(void* lh, void* rh)
{
	m_continue = true;
	while(m_continue)
	{super::Run(0.5);}
}


void HttpServerImp::Init(int port)
{
	super::Listen(port);
	m_tAccetp = new Threading::ThreadBind<HttpServerImp>(&HttpServerImp::Routine, this);
	m_tAccetp->Start();

	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard, "HTTP server has initialed");
}


void HttpServerImp::Release()
{
	m_continue = false;
	super::Close();
	m_tAccetp->Join();
	SAFE_DELETE(m_tAccetp);

	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard, "HTTP server has released");
}

#endif
