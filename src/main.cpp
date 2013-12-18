
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
	// 檢查程式的上限
#ifdef STARTUP_LIMIT
	if(g_app.Users()>STARTUP_LIMIT)
		throw FRAME_EXPCEPTION_1("程式啟動數量超過上限");
#endif


	// 這個區塊不要去修改他
	// 將實體放在heap避免stack over follow.
	g_ptrMainProc.reset(new MainProc());
	std::vector<int> _arrSignal;


	// 註冊signal的事件
	// 可以把不想處理的signal 註解起來
	// 或是在MainProc的OnSignal中不去處理他
	_arrSignal.push_back(SIGINT);
	_arrSignal.push_back(SIGQUIT);
	_arrSignal.push_back(SIGKILL);
	_arrSignal.push_back(SIGILL);
	_arrSignal.push_back(SIGCHLD);
	_arrSignal.push_back(SIGTERM);

	for(int i=0; i<_arrSignal.size(); i++)
		signal(_arrSignal[i], signal_event);

	// 這個區塊不要去修改他
	// 將實作放在MainProc的class裡頭
	g_ptrMainProc->Init();
	while(g_ptrMainProc->Routine()){}


	//  資源釋放掉就不應該再處理訊號了
	for(int i=0; i<_arrSignal.size(); i++)
		signal(_arrSignal[i], NULL);

	return 0;
}


int main(int argc, char** argv, char** env)
{
	// 將所有參數存起來
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


	// 主程式
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


	// 確保資源一定會釋放
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


	// 釋放g_sigmsg
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
