#ifndef __DATECLASS__
#define __DATECLASS__

#include <stdio.h>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#endif

#pragma pack(1)

class DateTime;
class DateTimeDiff
{
private:
	friend class DateTime;
	time_t m_timet;
	
	/// Only be constructed by DataTime.
	DateTimeDiff(time_t timet)
	{
		m_timet		= timet;
	}

public:
	/// Partial of seconds.
	int Second()		
	{
		int _sur1	= m_timet % (24*60*60);
		int _sur2	= _sur1 % (60*60);
		int _sec	= _sur2 % (60);
		return _sec;
	}
	
	/// Partial of Minutes.
	int Minute()		
	{
		int _sur1	= m_timet % (24*60*60);
		int _sur2	= _sur1 % (60*60);
		int _min	= _sur2 / (60);
		return _min;
	}
	
	/// Partial of Hours.
	int Hour()			
	{
		int _sur1	= m_timet % (24*60*60);
		int _hour	= _sur1 / (60*60);
		return _hour;
	}
	
	/// Partial of Days.
	int Day()
	{
		int _day	= m_timet / (24*60*60);
		return _day;
	}
	
	/// Tatol seconds.
	int TatolSecond()	{return (int)m_timet;}
	
	/// Total minutes.
	double TatolMinute(){return (double)m_timet/(60);}
	
	/// Total hours.
	double TatolHour()	{return (double)m_timet/(60*60);}
	
	/// Total days.
	double TatolDay()	{return (double)m_timet/(60*60*24);}
};

class DateTime
{
private:
	typedef struct tm TM;
	time_t m_timet;
	TM* GetTm(){return localtime(&m_timet);}
	int m_milsec;

	void Init(int year, int month, int day, int hour, int minute, int second)
	{
		m_milsec = 0;

		TM _tm;
		memset(&_tm, 0, sizeof(TM));
		_tm.tm_year = year-1900;
		_tm.tm_mon	= month-1;
		_tm.tm_mday = day;
		_tm.tm_hour = hour;
		_tm.tm_min	= minute;
		_tm.tm_sec	= second;
		m_timet = mktime(&_tm);
	}

	class SplitString
	{
	private:
		std::vector<std::string> m_vec;

	public:
		SplitString(std::string str, char symbol)
		{
			std::stringstream* _pss = new std::stringstream();
			for(int i=0; i<str.size(); ++i)
			{
				if(str[i]==symbol)
				{
					std::string _szContent = _pss->str();
					if(!_szContent.empty())
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

		int Size(){return m_vec.size();}
		std::string& operator[](int i){return m_vec[i];}
	};

public:
	/// Months in English.
	static const char ENG_MONTHS[][4];

	DateTime(int year, int month, int day, int hour, int minute, int second)
	{
		Init(year, month, day, hour, minute, second);
	}

	DateTime(DateTime dateTime, int hour, int minute, int second)
	{
		Init(dateTime.Year(), dateTime.Month(), dateTime.Day(), hour, minute,second);
	}

	DateTime(int year, int month, int day)
	{
		Init(year, month, day, 0,0,0);
	}

	DateTime(time_t TotalSecond)
	{
		m_milsec = 0;
		m_timet =TotalSecond;
	}
	
	/// construct by current time.
	DateTime()
	{
#ifdef _MSC_VER
		SYSTEMTIME _systime; 
		GetLocalTime( &_systime );
		Init( _systime.wYear, _systime.wMonth, _systime.wDay, _systime.wHour, _systime.wMinute, _systime.wSecond);
		m_milsec = _systime.wMilliseconds;
#else
		struct timeb _tmb;    
		ftime(&_tmb);
		m_timet = _tmb.time;
		m_milsec = _tmb.millitm;
#endif		
	}

	~DateTime()
	{
	}

	/// Add seconds to current time.
	void AddSecond(int sec)	{m_timet += sec;}
	
	/// Add minutes to current time.
	void AddMinute(int min)	{AddSecond(min*60);}
	
	/// Add Hours to current time.
	void AddHour(int hour)	{AddMinute(hour*60);}
	
	/// Add days to current time.
	void AddDay(int day)	{AddHour(day*24);}
	
	/// Total seconds from 1900/1/1
	time_t TotalSecond()	{return m_timet;}
	
	/// Partial of milliseconds.
	int Millisecond ()		{return m_milsec;}
	
	/// Partial of seconds.
	int Second()			{return GetTm()->tm_sec;}
	
	/// Partial of minutes.
	int Minute()			{return GetTm()->tm_min;}
	
	/// Partial of hours.
	int Hour()				{return GetTm()->tm_hour;}
	
	/// Partial of days.
	int Day()				{return GetTm()->tm_mday;}
	
	/// Partial of months.
	int Month()				{return GetTm()->tm_mon+1;}
	
	/// Partial of years.
	int Year()				{return GetTm()->tm_year+1900;}
	
	/// Convert the current time to default string format. 
	/// Ex. 2011/06/03 12:20:23.456
	std::string ToString()
	{
		char _acBuff[30];
		sprintf( _acBuff, "%04d/%02d/%02d %02d:%02d:%02d.%03d\0", 
			Year(),
			Month(),
			Day(),
			Hour(),
			Minute(),
			Second(),
			Millisecond());
		std::string _szBuff = static_cast<std::string>(_acBuff);
		return _szBuff;
	}

	/// Convert the current time to string with appointed format. 
	template <typename X>
	std::string ToString(X format)
	{
		return format(
			Year(),
			Month(),
			Day(),
			Hour(),
			Minute(),
			Second(),
			Millisecond());
	}

	/// Get current time with default format.
	static DateTime Now()
	{
		DateTime _dt;
		return _dt;
	}


	/// Get building time with default format.
	static DateTime Building()
	{
		int _year, _mon=1, _day, _hour, _min, _sec;
		
		SplitString _spDate(__DATE__, ' ');
		_year = atoi(_spDate[2].c_str());
		for(int i=1; i<=12; i++)
		{
			if( _spDate[0].compare(ENG_MONTHS[i])==0 )
			{
				_mon = i;
				break;
			}
		}
		_day = atoi(_spDate[1].c_str());


		SplitString _spTime(__TIME__, ':');

		_hour = atoi(_spTime[0].c_str());
		_min = atoi(_spTime[1].c_str());
		_sec = atoi(_spTime[2].c_str());

		DateTime _dt(_year, _mon, _day, _hour, _min, _sec);
		
		return _dt;
	}


	/// Time difference with two DateTimes.
	DateTimeDiff operator-(DateTime& dt)
	{
		time_t _diff = this->TotalSecond() - dt.TotalSecond();
		DateTimeDiff _dateTimeDiff( abs(_diff) );
		return _dateTimeDiff;
	}

	bool operator<(const DateTime& dt) const
	{
		if(this->m_timet==dt.m_timet)	return this->m_milsec<dt.m_milsec;
		else							return this->m_timet<dt.m_timet;
	}
		
	bool operator>(const DateTime& dt) const
	{
		if(this->m_timet==dt.m_timet)	return this->m_milsec>dt.m_milsec;
		else							return this->m_timet>dt.m_timet;
	}

	bool operator<=(const DateTime& dt) const
	{
		if(this->m_timet==dt.m_timet)	return this->m_milsec<=dt.m_milsec;
		else							return this->m_timet<=dt.m_timet;
	}

	bool operator>=(const DateTime& dt) const
	{
		if(this->m_timet==dt.m_timet)	return this->m_milsec>=dt.m_milsec;
		else							return this->m_timet>=dt.m_timet;
	}

	bool operator==(const DateTime& dt) const
	{
		if(this->m_timet==dt.m_timet)	return this->m_milsec==dt.m_milsec;
		else							return false;
	}
	
};

const char DateTime::ENG_MONTHS[][4] ={"NIL","Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}; 

#ifdef __GNUC__
#pragma pack()
#else
#pragma pack(4)
#endif

#endif /*__DATECLASS__*/
