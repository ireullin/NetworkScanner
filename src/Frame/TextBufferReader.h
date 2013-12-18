#ifndef __TEXTBUFFERREADER__
#define __TEXTBUFFERREADER__

/*************************** 將整個檔案讀進記憶體 *****************************************
Example:
	TextReader _reader;
	FileController::Instance()->Name("TEST").ReadLines(_reader);
	for(_reader.Begin(); _reader.End(); ++_reader)
		std::cout << _reader.ReadLine() << std::endl;

******************************************************************************************/
class FileIO;

class ITextBufferReader
{
protected:
	friend class FileIO;
	char*		m_buff;
	int			m_size;

	ITextBufferReader():m_buff(NULL),m_size(0){}
	virtual ~ITextBufferReader(){SAFE_DELETE_ARRAY(m_buff);}
};

/// 根據換行符號一行行讀出來
/// LineReader _reader;
/// for(_reader.begin(); _reader.end(); ++_reader)
class LineReader : public ITextBufferReader
{
private:
	typedef ITextBufferReader super;
	int			m_str;
	int			m_end;
	std::string	m_endsign;

	LineReader(const LineReader&);
	LineReader& operator= (const LineReader&);

	void NextOffset()
	{	
		char* _p = strstr(&m_buff[m_str], m_endsign.c_str());
		if(_p==NULL)	m_end = m_size-1;
		else			m_end = _p-m_buff;
	}

public:
	LineReader()
		:m_str(0),m_end(0),m_endsign("\n")
	{}

	/// 指定換行符號
	LineReader(const std::string& szEndSign)
		:m_str(0),m_end(0),m_endsign(szEndSign)
	{}
	
	~LineReader()
	{}

	int Size()
	{return m_size;}

	void Begin()
	{
		m_str=0;
		NextOffset();
	}

	bool End()
	{return m_str < m_size;}

	std::string ReadLine()
	{return std::string(&m_buff[m_str], m_end-m_str);}

	void Next()
	{
		m_str = m_end + m_endsign.size();
		NextOffset();
	}

	LineReader& operator++()
	{
		Next();
		return (*this);
	}

	const LineReader operator++(int i)
	{
		Next();
		LineReader _tr;
		return _tr;
	}
};


/// 每次讀出固定長度
/// FormatReader _reader(100);
/// for(_reader.begin(); _reader.end(); ++_reader)
class FormatReader : public ITextBufferReader
{
private:
	typedef ITextBufferReader super;
	int		m_offset;
	int		m_lineLength;

	FormatReader(const FormatReader&);
	FormatReader& operator= (const FormatReader&);


public:
	FormatReader(int length)
		:m_lineLength(length),m_offset(0)
	{}

	~FormatReader()
	{}

	/// 總資料長度
	int Size()
	{return m_size;}

	/// 移到資料前端
	void Begin()
	{m_offset = 0;}

	/// 是否到資料底端
	bool End()
	{return m_offset < m_size;}

	/// 讀出目前位置的筆數
	std::string ReadLine()
	{return std::string(&m_buff[m_offset], m_lineLength);}

	/// 移到下一筆
	FormatReader& operator++()
	{
		Next();
		return (*this);
	}

	/// 移到下一筆
	const FormatReader operator++(int i)
	{
		Next();
		FormatReader _tr(m_lineLength);
		return _tr;
	}

	/// 移到下一筆
	void Next()
	{m_offset += m_lineLength;}

	/// 資料總筆數
	int Count()
	{return m_size/m_lineLength;}

	/// 資料長度是否完整
	bool Good()
	{return (m_size%m_lineLength)==0;}

	/// 目前到第幾筆資料
	int Offset()
	{return m_offset/m_lineLength;}

};


#endif
