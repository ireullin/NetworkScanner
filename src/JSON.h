#ifndef __JSON__
#define __JSON__

namespace JSON
{
/*
template <typename T> 
std::string FromArray(T& arr, bool doubleQuotation = false)
{
	if(arr.empty())
		return "[]";

	std::stringstream _ss;
	_ss << "[";

	typename T::iterator _it;
	for(_it=arr.begin(); _it!=arr.end(); ++_it)
	{
		if(doubleQuotation)		_ss << "\"" << (*_it) << "\",";
		else					_ss << (*_it) << ",";
	}

	std::string _buff = _ss.str();
	_buff[_buff.size()-1] = ']';
	return _buff;
}


template <typename T> 
std::string FromReserveArray(T& arr, bool doubleQuotation = false)
{
	if(arr.empty())
		return "[]";

	std::stringstream _ss;
	_ss << "[";

	typename T::reverse_iterator _it;
	for(_it=arr.rbegin(); _it!=arr.rend(); ++_it)
	{
		if(doubleQuotation)		_ss << "\"" << (*_it) << "\",";
		else					_ss << (*_it) << ",";
	}

	std::string _buff = _ss.str();
	_buff[_buff.size()-1] = ']';
	return _buff;
}
*/

class IJSON
{
public:
	virtual	std::string ToString()=0;
};


class Array : public IJSON
{
private:
	std::vector<std::string> m_arr;

public:
	Array(){}
	~Array(){}

	template <typename T>
	void Push(T val)
	{
		std::stringstream _ss;
		_ss << val;
		m_arr.push_back(_ss.str());
	}

	void PushString(const std::string& val)
	{
		m_arr.push_back("\"" + val + "\"");
	}

	void PushChild(IJSON& obj)
	{
		m_arr.push_back( obj.ToString() );
	}

	void Reverse()
	{
		std::reverse(m_arr.begin(),m_arr.end()); 
	}

	virtual std::string ToString()
	{
		if(m_arr.empty())
			return "[]";

		std::stringstream _ss;
		_ss << "[";

		std::vector<std::string>::iterator _it;
		for(_it=m_arr.begin(); _it!=m_arr.end(); ++_it)
			_ss << (*_it) << ",";


		std::string _buff = _ss.str();
		_buff[_buff.size()-1] = ']';
		return _buff;
	}
};


class Hash : public IJSON
{
private:
	std::vector<std::string> m_arr;

public:
	Hash(){}
	~Hash(){}

	template <typename T>
	void Push(const std::string& key, T val)
	{
		std::stringstream _ss;
		_ss << "\"" << key << "\":" << val;
		m_arr.push_back(_ss.str());
	}

	void PushString(const std::string& key, const std::string& val)
	{
		std::stringstream _ss;
		_ss << "\"" << key << "\":\"" << val << "\"";
		m_arr.push_back(_ss.str());
	}

	void PushChild(const std::string& key, IJSON& obj)
	{
		std::stringstream _ss;
		_ss << "\"" << key << "\":" << obj.ToString();
		m_arr.push_back(_ss.str());
	}

	virtual std::string ToString()
	{
		if(m_arr.empty())
			return "{}";

		std::stringstream _ss;
		_ss << "{";

		std::vector<std::string>::iterator _it;
		for(_it=m_arr.begin(); _it!=m_arr.end(); ++_it)
			_ss << (*_it) << ",";


		std::string _buff = _ss.str();
		_buff[_buff.size()-1] = '}';
		return _buff;
	}
};

}
#endif
