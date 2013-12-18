#ifndef __COLUMNFORMAT__
#define __COLUMNFORMAT__

#include "StringFormatItem.h"

namespace StringPlus{
/***************************************************************************************
�ھ� RbtClass::CTextDataDef ���c�Q�[�H��y
���ɤF�į�P�ϥΤ�K��
�@��StringFormat���󬰤@�Ӥp����memory pool
�����P�ק�ȱN�|�����s��Buffer����

Example:
	// �إߪ���
	StringFormat _sf;
	_sf.Mark("col1", 4);
	_sf.Mark("col2", 5, '0', "Just note");
	_sf.Mark("col3", 1);
	_sf.CreateBuffer();

	// ��buffer
	_sf.Fill("123456.890");
	_sf = "ABCDEFG";

	// ����
	_sf["col1"].ToString();
	_sf["col2"].ToDouble();
	_sf["col2"].ToInt();
	_sf[0].ToString();
	_sf[1].ToDouble();
	_sf[2].ToInt();

	// �ק����
	_sf["col1"] = 1234
	_sf[1] = "TEST"

	���
	if(_sf["col1"]=="1234")
	if(_sf["col2"].ToInt()>5)

	// �ק�S�w�Ϭq
	_sf["col1"][0] = '3';
	_sf[1][1] = 'F';
	_sf(10) = 'A';
	_sf(0,3) = "ABC";
	_sf(5,-3) = "EFG";


*****************************************************************************************/
class ColumnFormat : public IStringFormat
{
private:
	typedef std::map<std::string,int>	IndexMap;
	typedef std::pair<std::string,int>	IndexPair;

	char*								m_buff;
	std::vector<StringFormatItem*>		m_arrItem;
	IndexMap							m_hIndex;
	StringFormatItem*					m_pTmpItem;
	std::string							m_szName;

	ColumnFormat(const ColumnFormat&);
	ColumnFormat& operator=(const ColumnFormat&);

	StringFormatItem* Last()
	{return m_arrItem[m_arrItem.size()-1];}

public:
	ColumnFormat();

	virtual ~ColumnFormat();

	/// �е��@���Ϭq,�p�Gbuffer�w�g�Q�إ�,�I�s����k�|�^��Exception
	/// szColumn: ���W��
	///	size: �Ϭq����
	/// empty: �Ϭq�w�]���r��
	/// szDes: ����
	void Mark(const std::string& szColumn, int size, const std::string szDes="",  char empty=' ');

	/// �ھ�Mark�����e�إ�buffer,�p�G�w�g�إ߹L�|�^��Exception
	void CreateBuffer();

	/// �Nbuffer�M���ť�
	virtual void ClearBuffer();

	/// ���s��buffer���e
	virtual void Fill(const std::string& szBuff);

	/// �^��buffer����
	virtual int BufferLength();

	/// �ثe�w�q�F�X�����
	int ColumnCount();

	/// �Nbuffer�ରstring
	virtual const std::string ToString();

	/// �^��buffer������
	virtual const char* ToPtr();

	/// ���s��buffer���e
	virtual void operator=(const std::string& szBuff);

	/// �૬��string
	//operator std::string();

	/// �૬��char*
	//operator char*();

	/// �ھگ��ި��oItem
	StringFormatItem& operator[](int i);

	/// �ھ����W�٨��oItem
	StringFormatItem& operator[](const std::string& column);

	/// �Ǧ^��@�r�����Ѧ�
	char& operator()(int index);

	/// ���w�@�q�d���ରItem
	/// begin: �}�l����m
	/// end: ��������m,�t���q�᭱�}�l��
	StringFormatItem& operator()(int begin, int end);

	/// �i�H���o�Ӫ���@�ӦW�r��K��O
	const std::string& Name();
	void Name(const std::string& val);

	/// ���O�_�۵�,�PCompare
	bool operator==(const std::string& val);
	bool operator!=(const std::string& val);
	bool Compare(const std::string& val);
};


ColumnFormat::ColumnFormat()
	:m_buff(NULL),m_pTmpItem(NULL)
{
}


ColumnFormat::~ColumnFormat()
{
	if(m_buff!=NULL)	delete[] m_buff;

	if(m_pTmpItem!=NULL)delete m_pTmpItem;

	for(int i=0; i<m_arrItem.size(); i++)
		delete m_arrItem[i];
}


void ColumnFormat::Fill(const std::string& szBuff)
{
	if(m_buff==NULL)
		CreateBuffer();

	if(szBuff.size() < BufferLength())
	{
		ClearBuffer();
		memcpy(m_buff, szBuff.c_str(), szBuff.size());
	}
	else if(szBuff.size() == BufferLength())
	{
		memcpy(m_buff, szBuff.c_str(), BufferLength());
	}
	else
	{
		throw FRAME_EXPCEPTION_1("Over the size of buffer.");
	}
}


void ColumnFormat::Mark(const std::string& szColumn, int size, const std::string szDes, char empty)
{
	if(m_buff!=NULL)
		throw FRAME_EXPCEPTION_1("Buffer has been created.");

	if(m_arrItem.empty())
		m_arrItem.push_back(new StringFormatItem(&m_buff, 0, size, empty, szColumn, szDes));
	else
		m_arrItem.push_back(new StringFormatItem(&m_buff, BufferLength(), size, empty, szColumn, szDes));

	m_hIndex.insert( IndexPair(szColumn, m_arrItem.size()-1));
}


void ColumnFormat::CreateBuffer()
{
	if(m_buff!=NULL)
		throw FRAME_EXPCEPTION_1("Buffer has been created.");

	m_buff = new char[BufferLength()];
	ClearBuffer();
}


void ColumnFormat::ClearBuffer()
{
	if(m_buff!=NULL)
		memset(m_buff, ' ', BufferLength());
}


void ColumnFormat::operator=(const std::string& szBuff)
{Fill(szBuff);}


int ColumnFormat::BufferLength()
{
	if(m_arrItem.empty())	return 0;
	else					return Last()->m_offset + Last()->m_size;
}


int ColumnFormat::ColumnCount()
{return m_arrItem.size();}


StringFormatItem& ColumnFormat::operator[](int i)
{
	if(m_buff==NULL)
		CreateBuffer();

	return (*m_arrItem[i]);
}


StringFormatItem& ColumnFormat::operator[](const std::string& column)
{
	if(m_buff==NULL)
		CreateBuffer();

	return (*m_arrItem[ m_hIndex[column] ]);
}


StringFormatItem& ColumnFormat::operator()(int begin, int end)
{
	int _end = 0;
	if(end<0)	_end = BufferLength() + end;
	else		_end = end;
	
	if(_end<=begin)throw FRAME_EXPCEPTION_1("End is smaller than begin.");

	if(m_pTmpItem!=NULL) delete m_pTmpItem;
	
	m_pTmpItem = new StringFormatItem(&m_buff, begin, _end-begin, ' ', "","");

	return (*m_pTmpItem);
}


const std::string ColumnFormat::ToString()
{
	if(m_buff==NULL)
		CreateBuffer();

	return std::string(m_buff, BufferLength());
}


//ColumnFormat::operator std::string()
//{
//	if(m_buff==NULL)
//		CreateBuffer();
//
//	return std::string(m_buff,BufferLength());
//}
//
//
//ColumnFormat::operator char*()
//{
//	if(m_buff==NULL)
//		CreateBuffer();
//
//	return m_buff;
//}


char& ColumnFormat::operator()(int index)
{
	if(m_buff==NULL)
		CreateBuffer();

	return m_buff[index];
}


const char* ColumnFormat::ToPtr()
{
	if(m_buff==NULL)
		CreateBuffer();

	return m_buff;
}


const std::string& ColumnFormat::Name()
{return m_szName;}


void ColumnFormat::Name(const std::string& val)
{m_szName=val;}


bool ColumnFormat::operator==(const std::string& val)
{return Compare(val);}


bool ColumnFormat::operator!=(const std::string& val)
{return !Compare(val);}


bool ColumnFormat::Compare(const std::string& val)
{return val.compare(0, val.size(), m_buff, BufferLength())==0;}

}
#endif
