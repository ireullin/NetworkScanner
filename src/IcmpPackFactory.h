#ifndef __ICMPPACKFACTOY__
#define __ICMPPACKFACTOY__

class IcmpPackFactory
{
private:
	char* m_buff;
	int ICMP_SIZE;


public:
	IcmpPackFactory();
	~IcmpPackFactory();
	int Size();
	Net::Raw::IcmpHeader& IcmpHeader();
	const char* Generate();
};


IcmpPackFactory::IcmpPackFactory()
{
	ICMP_SIZE = sizeof(Net::Raw::IcmpHeader);

	m_buff = new char[ICMP_SIZE];
	memset(m_buff, 0, ICMP_SIZE);

	IcmpHeader().Type(Net::Raw::eECHO_REQUEST);
	IcmpHeader().Code(Net::Raw::eNETWORK_UNREACHABLE);
	IcmpHeader().Sequence(0);
	IcmpHeader().ID(0);
	IcmpHeader().Checksum(0);
	IcmpHeader().Checksum( Net::Raw::CheckSum((unsigned short *) m_buff, ICMP_SIZE) );
}
	

IcmpPackFactory::~IcmpPackFactory()
{
	SAFE_DELETE_ARRAY(m_buff);
}


int IcmpPackFactory::Size()
{
	return ICMP_SIZE;
}

const char* IcmpPackFactory::Generate()
{
	return m_buff;
}



Net::Raw::IcmpHeader& IcmpPackFactory::IcmpHeader()
{
	return *((Net::Raw::IcmpHeader*)(m_buff));
}


#endif
