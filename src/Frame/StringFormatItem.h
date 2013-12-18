#ifndef __STRINGFORMATITEM__
#define __STRINGFORMATITEM__

#include <math.h>

namespace StringPlus{

class ColumnFormat;
class SimpleFormat;

class IStringFormat
{
protected:

public:
	virtual int	BufferLength()=0;
	virtual void ClearBuffer()=0;
	virtual	void Fill(const std::string& szBuff)=0;
	virtual const std::string ToString()=0;
	virtual const char* ToPtr()=0;
};


/// �O����榡�ഫ�`�N�į�
class StringFormatItem
{
private:
	friend class ColumnFormat;
	friend class SimpleFormat;

	char**		m_buff;
	char		m_empty;
	int			m_offset;
	int			m_size;
	std::string	m_szDes;
	std::string	m_szName;

	StringFormatItem(char** ptrBuff, int offset, int size, char empty, const std::string& szName, const std::string& szDes);
	StringFormatItem(const StringFormatItem&);
	StringFormatItem& operator=(const StringFormatItem&);

	inline char* Ptr()
	{return &(*m_buff)[m_offset];}

	inline char& Offset(int i)
	{return (*m_buff)[m_offset+i];}

public:
	/// ���W��
	const std::string& Name();

	/// ����
	const std::string& Description();

	/// �o�ӰϬq������
	int Size();

	/// �N�o�ӰϬq�M���w�]��
	void Clear();

	/// �ରstring
	std::string ToString();

	/// �ରdouble
	double ToDouble();

	/// �ରint
	int ToInt();

	/// �ରstring
	operator std::string();
	
	/// �Ǧ^�Ӧr�����Ѧ�
	char& operator[](int i);

	/// ���w�Ϭq����,�a�����
	void operator=(const std::string& val);
	
	/// ���w�Ϭq����,�a�k���
	//void operator=(double val);
	
	/// ���w�Ϭq����,�a�k���
	void operator=(int val);

	/// ���O�_�۵�,�PCompare
	bool operator==(const std::string& val);
	bool operator!=(const std::string& val);
	bool Compare(const std::string& val);

};


StringFormatItem::StringFormatItem(char** ptrBuff, int offset, int size, char empty, const std::string& szName, const std::string& szDes)
	:m_buff(ptrBuff),m_offset(offset),m_size(size),m_szDes(szDes),m_empty(empty),m_szName(szName)
{
}


// ��@atoi
void StringFormatItem::operator=(int val)
{
	char* _data = Ptr();
	for(int i=Size()-1; i>=0; --i)
	{
		_data[i] = (val%10)+'0';
		val = val / 10;
	}
	
	if(val!=0)throw FRAME_EXPCEPTION_1("Over the size of item.");
}


//void StringFormatItem::operator=(double val)
//{
//	/// �ثe�Q�������n���ഫ�覡	
//}


void StringFormatItem::operator=(const std::string& val)
{
	if(val.size() < Size())
	{
		Clear();
		memcpy(Ptr(), val.c_str(), val.size());
	}
	else if(val.size() == Size())
	{
		memcpy(Ptr(), val.c_str(), Size());
	}
	else
	{
		throw FRAME_EXPCEPTION_1("Over the size of item.");
	}
}


bool StringFormatItem::operator==(const std::string& val)
{return Compare(val);}


bool StringFormatItem::operator!=(const std::string& val)
{return !Compare(val);}


bool StringFormatItem::Compare(const std::string& val)
{return val.compare(0, val.size(),Ptr(), Size())==0;}


char& StringFormatItem::operator[](int i)
{
	if(i>=Size()) throw FRAME_EXPCEPTION_1("Over the size of item.");
	return Offset(i);
}

// ��@atof
double StringFormatItem::ToDouble()
{
	double	_rc=0;
	int		_cnt = 0;
	for(int i=0; i<Size(); i++)
	{
		if(((*m_buff)[m_offset+i])=='.' && _cnt==0)
		{
			_cnt++;
			continue;
		}

		if(_cnt==0)
		{
			_rc*=10;
			_rc+= Offset(i)-'0';
		}
		else
		{	
			_rc += (double)(Offset(i)-'0')/(double)pow(10,(double)_cnt);
			_cnt++;
		}
	}
	return _rc;
}


// ��@atoi
int StringFormatItem::ToInt()
{
	int _rc=0;
	for(int i=0; i<Size(); i++)
	{
		_rc*=10;
		_rc+= Offset(i)-'0';
	}
	return _rc;
}


const std::string& StringFormatItem::Description()
{return m_szDes;}


const std::string& StringFormatItem::Name()
{return m_szName;}


int StringFormatItem::Size()
{return m_size;}


void StringFormatItem::Clear()
{memset(Ptr(), m_empty, Size());}


std::string StringFormatItem::ToString()
{return std::string(Ptr(),m_size);}


StringFormatItem::operator std::string()
{return std::string(Ptr(),m_size);}

}
#endif
