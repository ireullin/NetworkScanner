#ifndef __LOGHEADER__
#define __LOGHEADER__

namespace LogHeader
{
	

/// 不需要Header的時候用這個
class FuntorBlankHeader
{
public:
	std::string operator()()
	{return "";}
};
FuntorBlankHeader Blank;


class FuntorHttpHeader
{
private:
	char m_acBuff[25];
public:
	std::string operator()()
	{
		memset(m_acBuff, ' ', sizeof(m_acBuff));
		sprintf( m_acBuff, "[ID:%010u %s]   [HTTP] ", Threading::CurrentThreadID(), DateTime::Now().ToString(DateTimeFormat::StandardTimeFormat).c_str());
		return std::string(m_acBuff);
	}
};
FuntorHttpHeader Http;


class FuntorSynHeader
{
private:
	char m_acBuff[25];
public:
	std::string operator()()
	{
		memset(m_acBuff, ' ', sizeof(m_acBuff));
		sprintf( m_acBuff, "[ID:%010u %s]    [SYN] ", Threading::CurrentThreadID(), DateTime::Now().ToString(DateTimeFormat::StandardTimeFormat).c_str());
		return std::string(m_acBuff);
	}
};
FuntorSynHeader Syn;

class FuntorFinHeader
{
private:
	char m_acBuff[25];
public:
	std::string operator()()
	{
		memset(m_acBuff, ' ', sizeof(m_acBuff));
		sprintf( m_acBuff, "[ID:%010u %s]    [FIN] ", Threading::CurrentThreadID(), DateTime::Now().ToString(DateTimeFormat::StandardTimeFormat).c_str());
		return std::string(m_acBuff);
	}
};
FuntorFinHeader Fin;

class FuntorSynackHeader
{
private:
	char m_acBuff[25];
public:
	std::string operator()()
	{
		memset(m_acBuff, ' ', sizeof(m_acBuff));
		sprintf( m_acBuff, "[ID:%010u %s][SYN/ACK] ", Threading::CurrentThreadID(), DateTime::Now().ToString(DateTimeFormat::StandardTimeFormat).c_str());
		return std::string(m_acBuff);
	}
};
FuntorSynackHeader SynAck;

class FuntorIcmpHeader
{
private:
	char m_acBuff[25];
public:
	std::string operator()()
	{
		memset(m_acBuff, ' ', sizeof(m_acBuff));
		sprintf( m_acBuff, "[ID:%010u %s]   [ICMP] ", Threading::CurrentThreadID(), DateTime::Now().ToString(DateTimeFormat::StandardTimeFormat).c_str());
		return std::string(m_acBuff);
	}
};
FuntorIcmpHeader Icmp;

class FuntorStandardHeader
{
private:
	char m_acBuff[25];
public:
	std::string operator()()
	{
		memset(m_acBuff, ' ', sizeof(m_acBuff));
		sprintf( m_acBuff, "[ID:%010u %s]          ", Threading::CurrentThreadID(), DateTime::Now().ToString(DateTimeFormat::StandardTimeFormat).c_str());
		return std::string(m_acBuff);
	}
};
FuntorStandardHeader Standard;


class FuntorDebugHeader
{
private:
	char m_acBuff[25];
public:
	std::string operator()()
	{
		memset(m_acBuff, ' ', sizeof(m_acBuff));
		sprintf( m_acBuff, "[ID:%010u %s] DEBUG    ", Threading::CurrentThreadID(), DateTime::Now().ToString(DateTimeFormat::StandardTimeFormat).c_str());
		return std::string(m_acBuff);
	}
};
FuntorDebugHeader Debug;


}


#endif
