
extern void signal_event(int signalNo);

#include "Frame.h"
#include "MainProc.h"

std::auto_ptr<MainProc> g_ptrMainProc;

void signal_event(int signalNo)
{
	if(g_ptrMainProc->OnSignal(signalNo))
		signal(signalNo, signal_event);
}


int sub_main()
{
	// �ˬd�{�����W��
#ifdef STARTUP_LIMIT
	if(g_app.Users()>STARTUP_LIMIT)
		throw FRAME_EXPCEPTION_1("�{���Ұʼƶq�W�L�W��");
#endif


	// �o�Ӱ϶����n�h�ק�L
	// �N�����bheap�קKstack over follow.
	g_ptrMainProc.reset(new MainProc());
	std::vector<int> _arrSignal;


	// ���Usignal���ƥ�
	// �i�H�⤣�Q�B�z��signal ���Ѱ_��
	// �άO�bMainProc��OnSignal�����h�B�z�L
	_arrSignal.push_back(SIGINT);
	_arrSignal.push_back(SIGQUIT);
	_arrSignal.push_back(SIGKILL);
	_arrSignal.push_back(SIGILL);
	_arrSignal.push_back(SIGCHLD);
	_arrSignal.push_back(SIGTERM);

	for(int i=0; i<_arrSignal.size(); i++)
		signal(_arrSignal[i], signal_event);

	// �o�Ӱ϶����n�h�ק�L
	// �N��@��bMainProc��class���Y
	g_ptrMainProc->Init();
	while(g_ptrMainProc->Routine()){}


	//  �귽���񱼴N�����ӦA�B�z�T���F
	for(int i=0; i<_arrSignal.size(); i++)
		signal(_arrSignal[i], NULL);

	return 0;
}


int main(int argc, char** argv, char** env)
{
	// �N�Ҧ��ѼƦs�_��
	try
	{g_app.Set(argv, env);}
	catch(std::exception& exp)
	{std::cout << "threw an exception \"" << exp.what() << "\"." << std::endl;}
	catch(const char* errMsg)
	{std::cout << "threw an exception \"" << errMsg << "\"." << std::endl;}
	catch(const std::string& errMsg)
	{std::cout << "threw an exception \"" << errMsg << "\"." << std::endl;}
	catch(...)
	{std::cout << "threw an unknown exception."<< std::endl;}


	// �D�{��
	try
	{sub_main();}
	catch(std::exception& exp)
	{std::cout << "threw an exception \"" << exp.what() << "\"." << std::endl;}
	catch(const char* errMsg)
	{std::cout << "threw an exception \"" << errMsg << "\"." << std::endl;}
	catch(const std::string& errMsg)
	{std::cout << "threw an exception \"" << errMsg << "\"." << std::endl;}
	catch(...)
	{std::cout << "threw an unknown exception."<< std::endl;}


	// �T�O�귽�@�w�|����
	try
	{if(g_ptrMainProc.get()!=NULL)	g_ptrMainProc->Release();}
	catch(std::exception& exp)
	{std::cout << "threw an exception \"" << exp.what() << "\"." << std::endl;}
	catch(const char* errMsg)
	{std::cout << "threw an exception \"" << errMsg << "\"." << std::endl;}
	catch(const std::string& errMsg)
	{std::cout << "threw an exception \"" << errMsg << "\"." << std::endl;}
	catch(...)
	{std::cout << "threw an unknown exception."<< std::endl;}


	// g_app
	try
	{g_app.Release();}
	catch(std::exception& exp)
	{std::cout << "threw an exception \"" << exp.what() << "\"." << std::endl;}
	catch(const char* errMsg)
	{std::cout << "threw an exception \"" << errMsg << "\"." << std::endl;}
	catch(const std::string& errMsg)
	{std::cout << "threw an exception \"" << errMsg << "\"." << std::endl;}
	catch(...)
	{std::cout << "threw an unknown exception."<< std::endl;}


	// ����g_sigmsg
	try
	{g_sigmsg.Release();}
	catch(std::exception& exp)
	{std::cout << "threw an exception \"" << exp.what() << "\"." << std::endl;}
	catch(const char* errMsg)
	{std::cout << "threw an exception \"" << errMsg << "\"." << std::endl;}
	catch(const std::string& errMsg)
	{std::cout << "threw an exception \"" << errMsg << "\"." << std::endl;}
	catch(...)
	{std::cout << "threw an unknown exception."<< std::endl;}

	return 0;
}
