#ifndef __ICMPHEADER__
#define __ICMPHEADER__

namespace Net{
namespace Raw{

enum IcmpType
{
	eECHO_REPLY =0,
	eDISTINATION_UNREACHABLE =3,
	eSOURCE_QUENCH = 4,
	eREDIRECT = 5,
	eECHO_REQUEST = 8,
	eTIME_EXEEDED_FOR_A_DATAGRAM = 11,
	ePARAMETER_PROBLEM_ON_A_DATAGRAM = 12,
	eTIMESTAMP_REQUEST = 13,
	eTIMESTAMP_REPLAY = 14,
	eINFORMATION_REQUEST = 15,
	eINFORMATION_REPLY = 16,
	eADDRESS_MASK_REQUEST = 17,
	eADDRESS_MASK_REPLY = 18
};

enum IcmpCode
{
	eNETWORK_UNREACHABLE=0,
	eHOST_UNREACHABLE,
	ePROTOCOL_UNREACHABLE,
	ePORT_UNREACHABLE,
	eFRAGMENTATION_NEEDED_AND_DF_SET,
	eSOURCE_ROUTE_FAILED,
	eDESTINATION_NETWORK_UNKNOWN,
	eDESTINATION_HOST_UNKNOWN,
	eSOURCE_HOST_ISOLATED,
	eCOMMUNICATION_WITH_DESTINATION_NETWORK_ADMINISTRAIVELY_PROHIBITED,
	eCOMMUNICATION_WITH_DESTINATION_HOST_ADMINISTRAIVELY_PROHIBITED,
	eNETWORK_UNREACHABLE_FOR_TYPE_OF_SERVICE,
	eHOST_UNREACHABLE_FOR_TYPE_OF_SERVICE
};

class IcmpHeader
{
private:
	unsigned char  m_type;
	unsigned char  m_code;
	unsigned short m_checksum;
	unsigned short m_ID;
	unsigned short m_sequence;

public:
	IcmpHeader()
		:m_type(0),m_code(0),m_checksum(0),m_ID(0),m_sequence(0){}

	unsigned char  Type()		{return m_type;}
	unsigned char  Code()		{return m_code;}
	unsigned short Checksum()	{return m_checksum;}
	unsigned short ID()			{return m_ID;}
	unsigned short Sequence()	{return m_sequence;}

	void Type(unsigned char type)			{m_type=type;}
	void Code(unsigned char code)			{m_code=code;}
	void Checksum(unsigned short checksum)	{m_checksum=checksum;}
	void ID(unsigned short id)				{m_ID=id;}
	void Sequence(unsigned short seq)		{m_sequence=seq;}
};

}
}
#endif
