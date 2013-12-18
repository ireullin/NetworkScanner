#ifndef __ABOUT__
#define __ABOUT__

class About
{
private:
	DateTime	m_built;
	std::string	m_version;
	std::string	m_author;
	std::string m_mail;

public:
	About()
	{
		m_built = DateTime::Building();
		m_version = "1.06";
		m_author = "Ireul Lin";
		m_mail = "ireullin@gmail.com";
	}

	DateTime Built()
	{return m_built;}

	const std::string& Version()
	{return m_version;}

	const std::string& Author()
	{return m_author;}

	const std::string& Mail()
	{return m_mail;}
};

About g_about;

#endif
