#ifndef __MASKCACULATOR__
#define __MASKCACULATOR__

namespace Net
{
class MaskCaculator
{
private:
	std::vector<std::string> m_arrIp;

	std::string ToIPString(unsigned long ip);

public:
	MaskCaculator(const std::string& ip, unsigned int mask);
	int Size();
	const std::string& operator[](int i);
	const std::string& Subnet();
	const std::string& Broadcast();

};


MaskCaculator::MaskCaculator(const std::string& ip, unsigned int mask)
{
	if(mask<=0 || mask >32)
		throw FRAME_EXPCEPTION_1("over the mask range");

	//unsigned int _mask = inet_addr("255.255.255.255");
	unsigned int _mask = 0xffffffff;
	_mask = htonl(_mask << (32-mask));

	unsigned int _ip = inet_addr(ip.c_str());
	unsigned int _subnet = _ip & _mask;
	
	unsigned long _count = pow(2, (32-mask));
	for(unsigned long i=0; i<_count; i++)
	{
		unsigned int _subip = htonl( ntohl(_subnet) + i);
		m_arrIp.push_back(ToIPString(_subip));
	}
}


std::string MaskCaculator::ToIPString(unsigned long ip)
{
	unsigned char* _ip = (unsigned char*)&ip;
	char _buff[20];
	sprintf(_buff, "%d.%d.%d.%d\0", _ip[0], _ip[1], _ip[2], _ip[3]);
	return std::string(_buff);
}


const std::string& MaskCaculator::Subnet()
{
	return m_arrIp[0];
}
	

const std::string& MaskCaculator::Broadcast()
{
	return m_arrIp[ Size()-1 ];
}


int MaskCaculator::Size()
{
	return m_arrIp.size();
}


const std::string& MaskCaculator::operator[](int i)
{
	return m_arrIp[i];
}
}


#endif
