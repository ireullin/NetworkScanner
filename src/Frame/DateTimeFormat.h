#ifndef __DATETIMEFORMAT__
#define __DATETIMEFORMAT__


// 定義DateTime中ToString的格式
namespace DateTimeFormat
{

/// Standard English format.
/// Ex. Jun 7 2011
struct FuntorEngDate
{
	std::string operator()(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		char _acBuff[20];
		sprintf( _acBuff, "%s %d %d\0", DateTime::ENG_MONTHS[month], day, year);
		std::string _szBuff = static_cast<std::string>(_acBuff);
		return _szBuff;
	}

};
struct FuntorEngDate EngDate;

/// 8 yards date format without symbol.
/// Ex. 20110701
struct FuntorEightNumDate
{
	std::string operator()(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		char _acBuff[20];
		sprintf( _acBuff, "%04d%02d%02d\0", year, month, day);
		std::string _szBuff = static_cast<std::string>(_acBuff);
		return _szBuff;
	}

};
struct FuntorEightNumDate EightNumDate;

/// 6 yards time format without symbol.
/// Ex. 132325
struct FuntorSixNumTime
{
	std::string operator()(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		char _acBuff[20];
		sprintf( _acBuff, "%02d%02d%02d\0", hour, minute, second);
		std::string _szBuff = static_cast<std::string>(_acBuff);
		return _szBuff;
	}

};
struct FuntorSixNumTime SixNumTime;

/// 6 yards time format without symbol.
/// Ex. 122342021
struct FuntorNineNumTime
{
	std::string operator()(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		char _acBuff[20];
		sprintf( _acBuff, "%02d%02d%02d%03d\0", hour, minute, second, millisecond);
		std::string _szBuff = static_cast<std::string>(_acBuff);
		return _szBuff;
	}

};
struct FuntorNineNumTime NineNumTime;

/// 證交所用的fix格式
/// 20070806-13:45:21.254
struct FuntorTweFixFormat
{
	std::string operator()(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		char _acBuff[25];
		sprintf( _acBuff, "%04d%02d%02d-%02d:%02d:%02d.%03d\0", year, month, day, hour, minute, second, millisecond);
		std::string _szBuff = static_cast<std::string>(_acBuff);
		return _szBuff;
	}

};
struct FuntorTweFixFormat TweFixFormat;


/// 標準時間格式
/// 13:45:21.254
struct FuntorStandardTimeFormat
{
	std::string operator()(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		char _acBuff[25];
		sprintf( _acBuff, "%02d:%02d:%02d.%03d\0", hour, minute, second, millisecond);
		std::string _szBuff = static_cast<std::string>(_acBuff);
		return _szBuff;
	}

};
struct FuntorStandardTimeFormat StandardTimeFormat;


/// 沒有任何標點符號的日期時間
/// 20070806134521254
struct FuntorSeventeenNum
{
	std::string operator()(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		char _acBuff[25];
		sprintf( _acBuff, "%04d%02d%02d%02d%02d%02d%03d\0", year, month, day, hour, minute, second, millisecond);
		std::string _szBuff = static_cast<std::string>(_acBuff);
		return _szBuff;
	}

};
struct FuntorSeventeenNum SeventeenNum;

}

#endif
