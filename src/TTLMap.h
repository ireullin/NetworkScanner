#ifndef __TTLMAP__
#define __TTLMAP__

typedef struct
{
	std::string OS;
	std::string Ver;
}TTLInfo;


class TTLMap
{
private:
	typedef std::tr1::shared_ptr<TTLInfo>	TTLInfoPtr;
	typedef std::vector<TTLInfoPtr>			ArrayTTL;
	typedef Dictionary2<int,ArrayTTL*>		MapTTL;
	

	MapTTL m_icmp;
	MapTTL m_tcpudp;

	void Push(MapTTL& map, int ttl, const std::string& os, const std::string& ver);


public:
	TTLMap();
	~TTLMap();
	std::string IcmpInfo(int ttl);
	std::string TcpUdpInfo(int ttl);
};

TTLMap::TTLMap()
{
	std::string _filename = g_app.AbsolutePath() + "/ttl.prn";
	std::ifstream _if( _filename.c_str() );
	if(!_if.good()) throw FRAME_EXPCEPTION_1("open index.html failed");
	
	char _buff[1024];
	memset(_buff, 0, sizeof(_buff));

	while(_if.getline(_buff, sizeof(_buff)))
    {    
		StringPlus::SplitString _sp(_buff, ';');
		int _ttl = atoi( StringPlus::Trim(_sp[3]).c_str());
		std::string _type = StringPlus::Trim(_sp[2]);
		std::string _ver = StringPlus::Trim(_sp[1]);
		std::string _os = StringPlus::Trim(_sp[0]);

		if(_type=="ICMP") // 只有icmp
			Push(m_icmp, _ttl, _os, _ver);
		else if(_type.find("ICMP")==std::string::npos) // 沒有icmp
			Push(m_tcpudp, _ttl, _os, _ver);
		else // 有icmp也有其他
		{
			Push(m_icmp, _ttl, _os, _ver);
			Push(m_tcpudp, _ttl, _os, _ver);
		}
    }

	_if.close();
}


TTLMap::~TTLMap()
{
	for(m_icmp.Begin(); !m_icmp.End(); ++m_icmp)
	{
		ArrayTTL* _tmp = m_icmp.Value();
		SAFE_DELETE(_tmp);
	}

	for(m_tcpudp.Begin(); !m_tcpudp.End(); ++m_tcpudp)
	{
		ArrayTTL* _tmp = m_tcpudp.Value();
		SAFE_DELETE(_tmp);
	}
}


void TTLMap::Push(MapTTL& map, int ttl, const std::string& os, const std::string& ver)
{
	ArrayTTL* _arr = NULL;

	if(map.ContainKey(ttl))
		_arr = map[ttl];
	else
	{
		_arr = new ArrayTTL();
		map.Add(ttl, _arr);
	}

	TTLInfoPtr _infoptr(new TTLInfo());
	_infoptr->OS = os;
	_infoptr->Ver = ver;

	_arr->push_back(_infoptr);	
}


std::string TTLMap::IcmpInfo(int ttl)
{
	if(!m_icmp.ContainKey(ttl))
		return "unknown";

	std::stringstream _ss;
	ArrayTTL* _arr = m_icmp[ttl];
	for(int i=0; i<_arr->size(); i++)
	{
		_ss << _arr->at(i)->OS;
		
		if( _arr->at(i)->Ver != "")
			_ss << "(" << _arr->at(i)->Ver << ")";

		if(_arr->size()-1 != i)
			_ss << "<br/>";
	}
	return _ss.str();
}

std::string TTLMap::TcpUdpInfo(int ttl)
{
	std::stringstream _ss;

	if(!m_tcpudp.ContainKey(ttl))
		return "unknown";

	ArrayTTL* _arr = m_tcpudp[ttl];
	for(int i=0; i<_arr->size(); i++)
	{
		_ss << _arr->at(i)->OS;
		
		if( _arr->at(i)->Ver != "")
			_ss << "(" << _arr->at(i)->Ver << ")";

		if(_arr->size()-1 != i)
			_ss << "<br/>";
	}
	return _ss.str();
}

TTLMap g_ttlmap;

#endif
