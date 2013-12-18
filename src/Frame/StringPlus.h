/*************************************************************
* Some functions which support std::string. 
*
* author: Ireul
* last modified: 2011/07/19
*
**************************************************************/
#ifndef __STRING_PLUS__
#define __STRING_PLUS__

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <ctype.h>


namespace StringPlus
{
	/// Remove "\r\n" or "\n" at a string's end.
	std::string Chomp(const std::string& value)
	{
		int i = value.size();
		if(value[i-1]!='\n')	return value;
		if(value[i-2]!='\r')	return value.substr(0, i-1);
		return value.substr(0, i-2);
	}

	/// Convert an unprintable character to \xXX if it was involved in a string.
	std::string ToPrintable(const std::string& value)
	{
		std::stringstream _ss;
		for(int i=0; i<value.size(); i++)
		{
			if(isprint(value[i]))	_ss << value[i];
			else					_ss << "\\x" << std::setfill('0') << std::setw(2) << std::setiosflags(std::ios::right) << std::hex << static_cast<int>(value[i]); 
		}
		return _ss.str();
	}


	std::string ToBinaryString(const char* data, int size, std::string split=",")
	{
		std::stringstream _ss;
		for(int i=0; i<size; i++)
		{
			for(int j=7; j>=0; j--)
			{
				if((data[i] & (1 << j)))_ss << "1";
				else					_ss << "0";
			}
			if(i!=size-1) _ss << split;
		}
		return _ss.str();
	}


	std::string ToHexString(const char* data, int size, std::string header="0x", std::string split=" ")
	{
		std::stringstream _ss;
		for(int i=0; i<size; i++)
		{
			_ss << header << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned short>(static_cast<unsigned char>(data[i]));
			if(i!=size-1) _ss << split;
		}
		return _ss.str();
	}


	/// Convert a unsigned char to a hexadecimal string without "0x".
	std::string ToHex(unsigned char uc)
	{
		char _buff[8];
		int _len = sprintf(_buff, "%02X\0", uc);
		return std::string(_buff); 
	}

	/// Convert a unsigned char to a hexadecimal string within "0x".
	std::string To0xHex(unsigned char uc)
	{
		char _buff[8];
		int _len = sprintf(_buff, "0x%02X\0", uc);
		return std::string(_buff); 
	}

	/// Compare chars regardless of case.
	bool CmpCharIgnoreCase(char cA, char cB)
	{
		return tolower(cA)==tolower(cB);
	}

	/// Compare strings regardless of case.
	bool CmpStrIgnoreCase(const std::string& szA, const std::string& szB)
	{
		if(szA.size()!=szB.size()) return false;
		return std::equal(szA.begin(), szA.end(), szB.begin(),CmpCharIgnoreCase);
	}

	/// Convert the string to lowercase.
	std::string ToLower(const std::string& szParam)
	{
		std::string _szParam(szParam);
		std::transform(_szParam.begin(), _szParam.end(), _szParam.begin(), tolower);
		return _szParam;
	}

	/// Convert the string to uppercase.
	std::string ToUpper(const std::string& szParam)
	{
		std::string _szParam(szParam);
		std::transform(_szParam.begin(), _szParam.end(), _szParam.begin(), toupper);
		return _szParam;
	}

	/// Remove space chars of left side.
	std::string TrimLeft(const std::string& szParam, char symbol=' ')
	{
		if(szParam.size()==0)
			return szParam;

		if(szParam[0]!=symbol)
			return szParam;

		int _end = szParam.find_first_not_of(symbol);
		if(_end==std::string::npos)	
			_end=szParam.size();
		
		std::string _szParam(szParam);
		_szParam.erase(_szParam.begin(), _szParam.begin()+_end);
		return _szParam;
	}

	/// Remove space chars of right side.
	std::string TrimRight(const std::string& szParam, char symbol=' ')
	{
		if(szParam.size()==0)
			return szParam;

		if(szParam[szParam.size()-1]!=symbol)
			return szParam;

		int _str = szParam.find_last_not_of(symbol);
		if(_str==std::string::npos)	
			_str=-1;

		std::string _szParam(szParam);
		_szParam.erase(_szParam.begin()+1+_str, _szParam.end());
		return _szParam;
	}

	/// Remove whole space chars.
	std::string Trim(const std::string& szParam, char symbol=' ')
	{
		std::string _rc1 = TrimLeft(szParam, symbol);
		std::string _rc2 = TrimRight(_rc1, symbol);
		return _rc2;
	}

	/// Replace all old strings by the new string in the source string.
	std::string Replace(const std::string& szSrc,const std::string& szOld,const std::string& szNew)
	{
		if(szNew.find(szOld)!=std::string::npos)
			throw FRAME_EXPCEPTION_1("unlimited recurrence");

		std::string _szBuff(szSrc);

		int _oldLength	= szOld.size();
		int _found		= _szBuff.find(szOld);

		while( _found!=std::string::npos )
		{
			_szBuff.replace(_found, _oldLength, szNew);
			_found = _szBuff.find(szOld);
		}	

		return _szBuff;
	}

	/// Right-aligns characters in this string, padding on the left with the symbol character.
	std::string PadLeft (const std::string& szSrc, int length , char symbol)
	{
		if(szSrc.size()>length)
			return szSrc.substr(szSrc.size()-length , length);

		if(szSrc.size()==length)
			return szSrc;

		std::stringstream _ss;
		_ss << std::setfill(symbol) << std::setw(length) << std::setiosflags(std::ios::right) << szSrc;
		return _ss.str();
	}

	/// Right-aligns characters in this string, padding on the left with the symbol character.
	std::string PadLeft (int src, int length , char symbol)
	{
		if(sizeof(src)>length)
			throw FRAME_EXPCEPTION_1("The length is smaller than size of int");

		std::stringstream _ss;
		_ss << std::setfill(symbol) << std::setw(length) << std::setiosflags(std::ios::right) << src;
		return _ss.str();
	}


	/// Left-aligns characters in this string, padding on the right with the symbol character.
	std::string PadRight(const std::string& szSrc, int length , char symbol)
	{
		if(szSrc.size()>length)
			return szSrc.substr(0, length);

		if(szSrc.size()==length)
			return szSrc;

		std::stringstream _ss;
		_ss << std::setfill(symbol) << std::setw(length) << std::setiosflags(std::ios::left) << szSrc;
		return _ss.str();
	}

	/// Left-aligns characters in this string, padding on the right with the symbol character.
	std::string PadRight(int src, int length , char symbol)
	{
		if(sizeof(src)>length)
			throw FRAME_EXPCEPTION_1("The length is smaller than size of int");

		std::stringstream _ss;
		_ss << std::setfill(symbol) << std::setw(length) << std::setiosflags(std::ios::left) << src;
		return _ss.str();
	}

	/// Split a string by a char.
	class SplitString
	{
	private:
		std::vector<std::string> m_vec;

	public:
		SplitString(const std::string& str, char symbol)
		{
			std::stringstream* _pss = new std::stringstream();
			for(int i=0; i<str.size(); ++i)
			{
				if(str[i]==symbol)
				{
					m_vec.push_back(_pss->str());
					delete _pss;
					_pss = new std::stringstream();
				}
				else
				{
					(*_pss) << str[i];
				}
			}
			m_vec.push_back(_pss->str());
			delete _pss;
		}

		int Size()
		{return m_vec.size();}

		std::string& operator[](int i)
		{return m_vec[i];}
	};
}

#endif
