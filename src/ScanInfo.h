#ifndef __SCANINFO__
#define __SCANINFO__


enum SCAN_TYPE
{
	eUNSET_SCANTYPE=0,
	eSYN_SCANTYPE,
	eFIN_SCANTYPE,
	eSYNACK_SCANTYPE,
	eICMP_SCANTYPE
};

enum SCAN_STATE
{
	eUNSET=0,
	eQUEUE,
	eSENT,
	eOPENED,
	eEXISTED,
	eTIMEOUT,
	eCLOSE
};


class ScanInfo
{
private:
	char		m_ip[16];
	int			m_srcport;
	int			m_desport;
	SCAN_TYPE	m_type;
	DateTime	m_senttime;
	DateTime	m_recvtime;
	int			m_timeout;
	SCAN_STATE	m_state;
	int			m_ttl;

public:
	ScanInfo():m_srcport(0),m_desport(0),m_type(eUNSET_SCANTYPE),m_timeout(0),m_state(eUNSET)
	{memset(m_ip, 0, sizeof(m_ip));}

	int SrcPort()				{return m_srcport;}
	int DesPort()				{return m_desport;}
	SCAN_TYPE Type()			{return m_type;}
	DateTime& SentTime()		{return m_senttime;}
	DateTime& RecvTime()		{return m_recvtime;}
	int Timeout()				{return m_timeout;}
	SCAN_STATE State()			{return m_state;}
	std::string IP()			{return std::string(m_ip);}
	int TTL()					{return m_ttl;}

	void SrcPort(int val)		{m_srcport=val;}
	void DesPort(int val)		{m_desport=val;}
	void Type(SCAN_TYPE val)	{m_type=val;}
	void SentTime(DateTime val)	{m_senttime=val;}
	void RecvTime(DateTime val)	{m_recvtime=val;}
	void Timeout(int val)		{m_timeout=val;}
	void State(SCAN_STATE val)	{m_state=val;}
	void TTL(int val)			{m_ttl=val;}
	
	void IP(const std::string& val)
	{memcpy(m_ip, val.c_str(), val.size());}
};

typedef std::tr1::shared_ptr<ScanInfo> ScanInfoPtr;


#endif
