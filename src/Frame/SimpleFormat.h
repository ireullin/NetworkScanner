#ifndef __SIMPLEFORMAT__
#define __SIMPLEFORMAT__

#include "StringFormatItem.h"

namespace StringPlus{

class SimpleFormat : public IStringFormat
{
private:
	int									m_length;
	char*								m_buff;
	StringFormatItem*					m_pTmpItem;

	void CreateBuffer();
	
public:
	SimpleFormat();
	SimpleFormat(const char* data);
	SimpleFormat(const std::string& szData);
	SimpleFormat(const std::string& szData, int length);
	SimpleFormat(int length);
	SimpleFormat(const SimpleFormat& simpleFormat);
	

	virtual ~SimpleFormat();

	/// 將buffer清為空白
	virtual void ClearBuffer();

	/// 重新填滿buffer內容
	virtual void Fill(const std::string& szBuff);

	/// 回傳buffer長度
	virtual int BufferLength();

	/// 將buffer轉為string
	virtual const std::string ToString();

	/// 回傳buffer的指標
 	virtual const char* ToPtr();

	/// 重新填滿buffer內容
	SimpleFormat& operator=(const SimpleFormat& simpleFormat);
	SimpleFormat& operator=(const std::string& szBuff);
	SimpleFormat& operator=(const char* buff);

	/// 轉型為string
	//operator std::string();

	/// 轉型為char*
	//operator char*();

	/// 傳回單一字元的參考
	char& operator()(int index);

	/// 指定一段範圍轉為Item
	/// begin: 開始的位置
	/// end: 結束的位置,負號從後面開始算
	StringFormatItem& operator()(int begin, int end);

	/// 比對是否相等,同Compare
	bool operator==(const std::string& val);
	bool operator!=(const std::string& val);
	bool Compare(const std::string& val);
};


SimpleFormat::SimpleFormat()
	:m_buff(NULL),m_pTmpItem(NULL),m_length(0)
{
}


SimpleFormat::SimpleFormat(const std::string& szData)
	:m_buff(NULL),m_pTmpItem(NULL),m_length(0)
{Fill(szData);}


SimpleFormat::SimpleFormat(const std::string& szData, int length)
	:m_buff(NULL),m_pTmpItem(NULL),m_length(length)
{
	CreateBuffer();
	Fill(szData);
}


SimpleFormat::SimpleFormat(int length)
	:m_buff(NULL),m_pTmpItem(NULL),m_length(length)
{CreateBuffer();}


SimpleFormat::SimpleFormat(const char* data)
	:m_buff(NULL),m_pTmpItem(NULL),m_length(0)
{
	m_length = strlen(data);
	CreateBuffer();
	memcpy(m_buff, data, m_length);
}

SimpleFormat::SimpleFormat(const SimpleFormat& simpleFormat)
	:m_pTmpItem(NULL)
{
	m_length = simpleFormat.m_length;
	CreateBuffer();
	memcpy(m_buff, simpleFormat.m_buff, m_length);
}


SimpleFormat::~SimpleFormat()
{
	if(m_buff!=NULL)	delete[] m_buff;
	if(m_pTmpItem!=NULL)delete m_pTmpItem;
}


void SimpleFormat::Fill(const std::string& szBuff)
{
	if(m_buff==NULL)
	{
		m_length = szBuff.size();
		CreateBuffer();
	}

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


void SimpleFormat::CreateBuffer()
{
	if(m_buff!=NULL)
		throw FRAME_EXPCEPTION_1("Buffer has been created.");

	m_buff = new char[BufferLength()];
	ClearBuffer();
}


void SimpleFormat::ClearBuffer()
{
	if(m_buff!=NULL)
		memset(m_buff, ' ', BufferLength());
}


SimpleFormat& SimpleFormat::operator=(const std::string& szBuff)
{
	Fill(szBuff);
	return *this;
}


SimpleFormat& SimpleFormat::operator=(const char* buff)
{
	int _length = strlen(buff);
	if(m_buff==NULL)
	{
		m_length = _length;
		CreateBuffer();
	}

	if(_length < BufferLength())
	{
		ClearBuffer();
		memcpy(m_buff, buff, _length);
	}
	else if(_length == BufferLength())
	{
		memcpy(m_buff, buff, BufferLength());
	}
	else
	{
		throw FRAME_EXPCEPTION_1("Over the size of buffer.");
	}

	return *this;
}

SimpleFormat& SimpleFormat::operator=(const SimpleFormat& simpleFormat)
{
	if(m_buff==NULL)
	{
		m_length = simpleFormat.m_length;
		CreateBuffer();
	}

	if(simpleFormat.m_length < BufferLength())
	{
		ClearBuffer();
		memcpy(m_buff, simpleFormat.m_buff, simpleFormat.m_length);
	}
	else if(simpleFormat.m_length == BufferLength())
	{
		memcpy(m_buff, simpleFormat.m_buff, BufferLength());
	}
	else
	{
		throw FRAME_EXPCEPTION_1("Over the size of buffer.");
	}

	return *this;
}


int SimpleFormat::BufferLength()
{return m_length;}


StringFormatItem& SimpleFormat::operator()(int begin, int end)
{
	int _end = 0;
	if(end<0)	_end = BufferLength() + end;
	else		_end = end;
	
	if(_end<=begin)throw FRAME_EXPCEPTION_1("End is smaller than begin.");

	if(m_pTmpItem!=NULL) delete m_pTmpItem;
	
	m_pTmpItem = new StringFormatItem(&m_buff, begin, _end-begin+1, ' ', "","");

	return (*m_pTmpItem);
}


const std::string SimpleFormat::ToString()
{
	if(m_buff==NULL)
		CreateBuffer();

	return std::string(m_buff, BufferLength());
}


//SimpleFormat::operator std::string()
//{
//	if(m_buff==NULL)
//		CreateBuffer();
//
//	return std::string(m_buff,BufferLength());
//}
//
//
//SimpleFormat::operator char*()
//{
//	if(m_buff==NULL)
//		CreateBuffer();
//
//	return m_buff;
//}


char& SimpleFormat::operator()(int index)
{
	if(m_buff==NULL)
		CreateBuffer();

	int _end = 0;
	if(index<0)	_end = BufferLength() + index;
	else		_end = index;

	return m_buff[_end];
}


const char* SimpleFormat::ToPtr()
{
	if(m_buff==NULL)
		CreateBuffer();

	return m_buff;
}


bool SimpleFormat::operator==(const std::string& val)
{return Compare(val);}


bool SimpleFormat::operator!=(const std::string& val)
{return !Compare(val);}


bool SimpleFormat::Compare(const std::string& val)
{return val.compare(0, val.size(), m_buff, m_length)==0;}


}


#endif
