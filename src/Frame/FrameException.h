#ifndef __FRAMEEXCEPTION__
#define __FRAMEEXCEPTION__

#include <typeinfo>

namespace Frame{
class Exception : public std::exception
{
private:
	std::string m_what;

public:
	Exception(int errNo, const std::string& szErrMsg, const std::string& szFile, int line )
	{
		std::stringstream _ss;
		_ss << "No:" << errNo << " Message:" << szErrMsg << " (" << szFile << ":" << line << ")";
		m_what = _ss.str();
	}

	Exception(const std::string& szErrMsg, const std::string& szFile, int line )
	{
		std::stringstream _ss;
		_ss << "Message:" << szErrMsg << " (" << szFile << ":" << line << ")";
		m_what = _ss.str();
	}

	Exception(int errNo, const std::string& szFile, int line )
	{
		std::stringstream _ss;
		_ss << "No:" << errNo << " Message:" << strerror(errNo) << " (" << szFile << ":" << line << ")";
		m_what = _ss.str();
	}

	Exception(int errNo, const std::string& szErrMsg)
	{
		std::stringstream _ss;
		_ss << "No:" << errNo << " Message:" << szErrMsg;
		m_what = _ss.str();
	}

	Exception(const std::string& szErrMsg)
	{
		std::stringstream _ss;
		_ss << "Message:" << szErrMsg;
		m_what = _ss.str();
	}

	Exception(int errNo)
	{
		std::stringstream _ss;
		_ss << "No:" << errNo << " Message:" << strerror(errNo);
		m_what = _ss.str();
	}

	
	virtual const char* what() const throw()
	{return m_what.c_str();}

	virtual ~Exception()throw(){}

};
}

//#define FRAME_EXPCEPTION_1(param)	Frame::Exception(param, __FILE__, __LINE__);
//#define FRAME_EXPCEPTION_2(no, msg)	Frame::Exception(no, msg, __FILE__, __LINE__);




#endif
