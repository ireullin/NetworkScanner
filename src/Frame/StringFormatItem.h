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


/// 記憶體格式轉換注意效能
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
	/// 欄位名稱
	const std::string& Name();

	/// 註解
	const std::string& Description();

	/// 這個區段的長度
	int Size();

	/// 將這個區段清為預設值
	void Clear();

	/// 轉為string
	std::string ToString();

	/// 轉為double
	double ToDouble();

	/// 轉為int
	int ToInt();

	/// 轉為string
	operator std::string();
	
	/// 傳回該字元的參考
	char& operator[](int i);

	/// 指定區段的值,靠左對齊
	void operator=(const std::string& val);
	
	/// 指定區段的值,靠右對齊
	//void operator=(double val);
	
	/// 指定區段的值,靠右對齊
	void operator=(int val);

	/// 比對是否相等,同Compare
	bool operator==(const std::string& val);
	bool operator!=(const std::string& val);
	bool Compare(const std::string& val);

};


StringFormatItem::StringFormatItem(char** ptrBuff, int offset, int size, char empty, const std::string& szName, const std::string& szDes)
	:m_buff(ptrBuff),m_offset(offset),m_size(size),m_szDes(szDes),m_empty(empty),m_szName(szName)
{
}


// 實作atoi
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
//	/// 目前想不到比較好的轉換方式	
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

// 實作atof
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


// 實作atoi
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
