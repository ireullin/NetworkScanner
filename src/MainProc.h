#ifndef __MAINPROC__
#define __MAINPROC__

#include "About.h"
#include "DataCenter.h"
#include "RawSocket.h"
#include "HttpServerImp.h"
#include "JSON.h"



/// �N��main������,�|�w���B�z�ǤJ�Ѽ�,�����ܼ�,�t�ΰT���H�ΰ�����T
/// �ɶq�קK�b�o�Ӫ��󪺫غc���P�Ѻc�����g�F��
class MainProc
{
public:
	/// �{���Ұʭn��l�ƪ���Ƴ���b���Y
	void Init();

	/// �o��method�|�@�����ƪ��Q�I�s,����^��false
	/// sleep�]�n��b�o�Ӹ��Y
	bool Routine();
	
	/// �{�������n���񪺸�Ƴ���b���Y
	void Release();

	/// ����signal�|�I�s�o��method,�p�G�^��true�N�|���s���U�@��
	bool OnSignal(int signalNo);

	void DrawHeader();
	void DrawStatus();


private:

	bool			m_isContinue;
	int				m_port;
	bool			m_refresh;
};


void MainProc::DrawStatus()
{
	std::cout << std::setfill(' ') << std::setw(10) << std::setiosflags(std::ios::left) << DataCenter::Instance()->QueueSize();
	std::cout << std::setfill(' ') << std::setw(10) << std::setiosflags(std::ios::left) << DataCenter::Instance()->SentSize();
	std::cout << std::setfill(' ') << std::setw(10) << std::setiosflags(std::ios::left) << DataCenter::Instance()->ReceivedSize();
	std::cout << std::setfill(' ') << std::setw(10) << std::setiosflags(std::ios::left) << DataCenter::Instance()->RecvBuffSize();
	std::cout << "\n" << std::flush;
}


void MainProc::DrawHeader()
{
	printf("\e[2J\e[0;0H");
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "please connect to\nhttp://" <<RawSocket::Instance()->LocalIP() << ":" << m_port;
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "press ctrl-c to exit\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << std::setfill(' ') << std::setw(10) << std::setiosflags(std::ios::left) << "queue";
	std::cout << std::setfill(' ') << std::setw(10) << std::setiosflags(std::ios::left) << "sent";
	std::cout << std::setfill(' ') << std::setw(10) << std::setiosflags(std::ios::left) << "result";
	std::cout << std::setfill(' ') << std::setw(10) << std::setiosflags(std::ios::left) << "buffer";
	std::cout << "\n";
	std::cout << std::setfill('=') << std::setw(40) <<"";
	std::cout << "\n";
	std::cout << std::flush;
}


void MainProc::Init()
{
	m_isContinue = false;

	printf("%s is starting...\n", g_app.Name().c_str());

	if(g_app.ParamSize()!=1)
	{
		printf("parameters are error\n");
		return;
	}

	ConfigController::Instance()->OpenFile("SETTING", g_app.AbsolutePath() + "/ntscan.conf");
	m_port = ConfigController::Instance()->Name("SETTING").Get("MAIN","port").ToInt();
	m_refresh = ConfigController::Instance()->Name("SETTING").Get("MAIN","keep_status").ToBoolean();

	if(g_app.ParamSize()==2)
		 m_port = g_app[1].ToInt();	

	std::string _szLogFolder = ConfigController::Instance()->Name("SETTING").Get("MAIN","log_path").ToString();
	std::string _szLogFile = _szLogFolder + "/" + DateTime::Now().ToString(DateTimeFormat::EightNumDate) + ".log";
	if(!FileInfo::IsExist(_szLogFolder))
		FileInfo::CreateFolder(_szLogFolder, "777");

	LogController::Instance()->CreateLogWriter("MAIN", _szLogFile);
	LogController::Instance()->Start();
	LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard,"%s built at %s version:%s", g_app.Name().c_str(), g_about.Built().ToString().c_str(), g_about.Version().c_str());

	Frame::ThreadPool::Instance()->MaxThreads(10);
	DataCenter::Instance()->Init();
	IcmpRawSocket::Instance()->Init();
	RawSocket::Instance()->Init();
	HttpServerImp::Instance()->Init(m_port);

	m_isContinue = true;

	DrawHeader();
}


bool MainProc::Routine()
{
	if(!m_refresh)
		DrawHeader();

	DrawStatus();
	Threading::Sleep(1);
	return m_isContinue;
}


void MainProc::Release()
{
	printf("%s will be finished...\n", g_app.Name().c_str());

	HttpServerImp::Instance()->Release();
	RawSocket::Instance()->Release();
	IcmpRawSocket::Instance()->Release();
	DataCenter::Instance()->Release();
}


bool MainProc::OnSignal(int signalNo)
{
	std::stringstream _ssMsg;
	_ssMsg << g_sigmsg[signalNo].Name() << "(" << signalNo << "): " << g_sigmsg[signalNo].Description();
	std::cout << "\n" << _ssMsg.str() << std::endl;
	
	//LogController::Instance()->Name("MAIN").WriteFormat(LogHeader::Standard,"%s ", "test");

	switch(signalNo)
	{
	case SIGINT:
	case SIGTERM:
	case SIGUSR1: // ������ߥXexception���ɭԷ|�q�o��^��,�H�T�O�Ҧ��귽�Q����
	case SIGCHLD:
		m_isContinue = false;
		return false;
	default:
		return true;
	}
}

#endif


