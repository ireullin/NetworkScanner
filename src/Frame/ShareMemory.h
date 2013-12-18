#ifndef __SHAREMEMORY__
#define __SHAREMEMORY__

#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "DateTime.h"

class ShareMemoryException : public std::exception
{
private:
	int m_errno;
	std::string m_errmsg;
public:
	ShareMemoryException(const std::string& errmsg, int error_no=0):m_errno(error_no),m_errmsg(errmsg){ }
	virtual ~ShareMemoryException()throw(){}
	virtual const char* what() const throw(){return m_errmsg.c_str();}
	int no(){return m_errno;}
};


class ShareMemoryState
{
private:
	struct shmid_ds m_ds;

public:
	ShareMemoryState()
	{memset(&m_ds, 0, sizeof(m_ds)); }

	virtual ~ShareMemoryState(){}
	
	operator struct shmid_ds*()
	{return &m_ds;}

	/// Size of segment (bytes)
	int Size()  
	{return (int)m_ds.shm_segsz;}

	/// Last attach time
	DateTime LastAttachTime()
	{return DateTime(m_ds.shm_atime);}

	/// Last detach time
	DateTime LastDetachTime()
	{return DateTime(m_ds.shm_dtime);}

	///Last change time
	DateTime LastChangeTime()
	{return DateTime(m_ds.shm_ctime);}

	/// PID of creator
	int CreatorPID()
	{return (int)m_ds.shm_cpid; }

	/// PID of last accessor
	int LastPID()
	{return (int)m_ds.shm_lpid; }

	/// No. of current attaches
	int CurrentNumber()
	{return m_ds.shm_nattch; }
};


class ShareMemorySet
{
private:
	struct ipc_perm m_perm;

public:
	ShareMemorySet()
	{memset(&m_perm, 0, sizeof(m_perm)); }

	virtual ~ShareMemorySet(){}
	
	operator struct ipc_perm*()
	{return &m_perm;}

	/// owner's user id
	int  OwnerUID()			{return m_perm.uid;}
	void OwnerUID(int val)	{m_perm.uid=val;}

	/// owner's group id
	int  OwnerGID()			{return m_perm.gid;}
	void OwnerGID(int val)	{m_perm.gid=val;}

	/// creator's user id
	int  CreatorUID()		{return m_perm.cuid;}
	void CreatorUID(int val){m_perm.cuid=val;}

	/// creator's group id
	int  CreatorGID()		{return m_perm.cgid;}
	void CreatorGID(int val){m_perm.cgid=val;}

	/// access modes	
	int  Mode()		  {return m_perm.mode;}
	void Mode(int val){m_perm.mode=val;}

#ifdef __linux__
	/// slot usage sequence number
	unsigned short	Sequence()		 {return m_perm.__seq;}
	void			Sequence(int val){m_perm.__seq=val;}
	
	/// Key
	int  Key()		{return m_perm.__key;}
	void Key(int val){m_perm.__key=val;}
#else
	/// slot usage sequence number
	unsigned short	Sequence()		 {return m_perm.seq;}
	void			Sequence(int val){m_perm.seq=val;}
	
	/// Key
	int  Key()		{return m_perm.key;}
	void Key(int val){m_perm.key=val;}
#endif
};


class ShareMemoryBasic
{
private:
	int		m_shmID;
	int		m_key;
	void*	m_address;

public:
	ShareMemoryBasic()
		:m_shmID(0),m_key(0),m_address(NULL)
	{
	}

	virtual ~ShareMemoryBasic(){}

	void* Adderss(){return m_address;}
	int Key(){return m_key;}
	int ID(){return m_shmID;}

	void Exception()
	{
		int _err = errno;
		ShareMemoryException _exp(strerror(_err), _err);
		throw _exp;
	}

	void Shmget(int key, int size, int flag)
	{
		m_key = key;
		key_t _key = static_cast<key_t>(m_key);
		m_shmID = shmget(_key, size, flag);
		if(m_shmID==-1)Exception();
	}

	void Shmat(const void *addr, int flag)
	{
		m_address = shmat(m_shmID, addr, flag);
		if(m_address==(void*)-1)Exception();
	}

	void Shmdt()
	{
		int _rc = shmdt(m_address);
		if(_rc!=0)Exception();
	}

	void Shmctl(int cmd, struct shmid_ds* buf)
	{
		int _rc = shmctl(m_shmID, cmd, buf); 
		if(_rc!=0)Exception();
	}
};


class ShareMemory
{
private:
	ShareMemoryBasic m_basic;

public:
	ShareMemory()
	{}

	virtual ~ShareMemory()
	{}

	operator ShareMemoryBasic*()
	{return &m_basic;}

	/// 記憶體位址
	void* Adderss()
	{return m_basic.Adderss();}

	/// 取得記憶體位址,如果不存在就建立,權限可讀寫
	void Open(int key, int size)
	{
		m_basic.Shmget(key, size,  IPC_CREAT | 0777);
		m_basic.Shmat(NULL, 0);
	}

	/// 把這塊記憶體清除
	void Erase()
	{m_basic.Shmctl(IPC_RMID, NULL);}

	/// 從這個記憶體離開
	void Detach()
	{m_basic.Shmdt();}

	/// 取得ShareMemory資訊
	void State(struct shmid_ds *buf)
	{ m_basic.Shmctl(IPC_STAT,buf);}

	/// 設定ShareMemory參數
	void Set(struct ipc_perm *buf)
	{ 
		struct shmid_ds _ds;
		memset(&_ds, 0, sizeof(_ds));
		memcpy(&_ds, buf, sizeof(struct ipc_perm));
		m_basic.Shmctl(IPC_SET,&_ds);
	}

	/// The following commands are NOT supported on AIX 5.2.
	void Lock()
	{m_basic.Shmctl(SHM_LOCK,NULL);}

	/// The following commands are NOT supported on AIX 5.2.
	void Unlock()
	{m_basic.Shmctl(SHM_UNLOCK ,NULL);}

};




#endif
