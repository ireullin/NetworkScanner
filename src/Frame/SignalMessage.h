#ifndef __SIGNALMESSAGE__
#define __SIGNALMESSAGE__

class SignalMessage;
class SignalMessageItem
{
private:
	friend class SignalMessage;
	std::string m_szName;
	std::string m_szDesc;
	SignalMessageItem(const std::string& szName, const std::string& szDescription):m_szName(szName),m_szDesc(szDescription){}
	SignalMessageItem(const SignalMessageItem&);
	SignalMessageItem& operator=(const SignalMessageItem&);

public:
	const std::string& Name(){return m_szName;}
	const std::string& Description(){return m_szDesc;}
};


class SignalMessage
{
private:
	friend int main(int, char**, char**);
	SignalMessage(const SignalMessage&);
	SignalMessage& operator= (const SignalMessage&);
	Dictionary<int, SignalMessageItem*> m_dItem;

	void Add(const std::string& szSignal, int signal, const std::string& szDesc)
	{m_dItem.Add(signal, new SignalMessageItem(szSignal, szDesc));}

	void Init()
	{
		// from sys/signal.h
		Add("unknown",	   -1	,"unknown signal number");
		Add("SIGHUP",	   1	,"hangup, generated when terminal disconnects");
		Add("SIGINT",	   2	,"interrupt, generated from terminal special char");
		Add("SIGQUIT",	   3	,"(*) quit, generated from terminal special char");
		Add("SIGILL",		4	,"(*) illegal instruction (not reset when caught)");
		Add("SIGTRAP",	   5	,"(*) trace trap (not reset when caught)");
		Add("SIGABRT",    6	,"(*) abort process");
		Add("SIGEMT",	   7	,"EMT intruction");
		Add("SIGFPE",	   8	,"(*) floating point exception");
		Add("SIGKILL",	   9	,"kill (cannot be caught or ignored)");
		Add("SIGBUS",	  10	,"(*) bus error (specification exception)");
		Add("SIGSEGV",	  11	,"(*) segmentation violation");
		Add("SIGSYS",	  12	,"(*) bad argument to system call");
		Add("SIGPIPE",	  13	,"write on a pipe with no one to read it");
		Add("SIGALRM",	  14	,"alarm clock timeout");
		Add("SIGTERM",	  15	,"software termination signal");
		Add("SIGURG", 	  16	,"(+) urgent contition on I/O channel");
		Add("SIGSTOP",	  17	,"(@) stop (cannot be caught or ignored)");
		Add("SIGTSTP",	  18	,"(@) interactive stop");
		Add("SIGCONT",	  19	,"(!) continue (cannot be caught or ignored)");
		Add("SIGCHLD",   20	,"(+) sent to parent on child stop or exit");
		Add("SIGTTIN",   21	,"(@) background read attempted from control terminal");
		Add("SIGTTOU",   22	,"(@) background write attempted to control terminal");
		Add("SIGIO",	  23	,"(+) I/O possible, or completed");
		Add("SIGXCPU",	  24	,"cpu time limit exceeded (see setrlimit())");
		Add("SIGXFSZ",	  25	,"file size limit exceeded (see setrlimit())");
		Add("SIGMSG",    27	,"input data is in the ring buffer");
		Add("SIGWINCH",  28	,"(+) window size changed");
		Add("SIGPWR",    29	,"(+) power-fail restart");
		Add("SIGUSR1",   30	,"user defined signal 1 (a thread threw an exception)");
		Add("SIGUSR2",   31	,"user defined signal 2");
		Add("SIGPROF",   32	,"profiling time alarm (see setitimer)");
		Add("SIGDANGER", 33	,"system crash imminent; free up some page space");
		Add("SIGVTALRM", 34	,"virtual time alarm (see setitimer)");
		Add("SIGMIGRATE", 35	,"migrate process");
		Add("SIGPRE",	  36	,"programming exception");
		Add("SIGVIRT",   37	,"AIX virtual time alarm");
		Add("SIGALRM1",  38	,"m:n condition variables - RESERVED - DON'T USE");
		Add("SIGWAITING", 39	,"m:n scheduling - RESERVED - DON'T USE");
		Add("SIGRECONFIG", 58 	,"Reserved for Dynamic Reconfiguration Operations");
		Add("SIGCPUFAIL", 59	,"Predictive De-configuration of Processors -");
		Add("SIGKAP",    60    ,"keep alive poll from native keyboard");
		Add("SIGRETRACT", 61   ,"monitor mode should be relinguished");
		Add("SIGSOUND",  62    ,"sound control has completed");
		Add("SIGSAK",    63	,"secure attention key");
	}
	
	void Release()
	{
		if(m_dItem.Size()==0)	return;
		m_dItem.Reset();
		do{
			SignalMessageItem* _item = m_dItem.GetCurrentValue();
			delete _item;
		}while(m_dItem.Next());
		m_dItem.Clear();
	}


public:
	SignalMessage(){}
	~SignalMessage()
	{Release();}


	SignalMessageItem& operator[](int i)
	{
		// 要用到的時候才載入,節省資源
		if(m_dItem.Size()==0)			Init();
		if(!m_dItem.ContainKey(i))		i = -1;
		return (*m_dItem[i]);
	}

};

#endif
