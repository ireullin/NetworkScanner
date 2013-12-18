#ifndef __FILEINFO__
#define __FILEINFO__

#include <dirent.h>


class FileCollection;

class FileInfo
{
// cost of clone.
private:
	std::string m_szFileNmae;

// member method
public:
	FileInfo()
	{}
	
	FileInfo(const std::string& szFileName)
	{m_szFileNmae=szFileName;}

	FileInfo(const FileInfo& fileInfo)
	{m_szFileNmae=fileInfo.m_szFileNmae;}
	
	FileInfo& operator=(const FileInfo& fileInfo)
	{
		m_szFileNmae=fileInfo.m_szFileNmae;
		return *this;
	}

	FileInfo& operator=(const std::string& szFileName)
	{
		m_szFileNmae=szFileName;
		return *this;
	}

	/// 檔案名稱
	std::string Name()const;

	/// 取得完整路徑
	const std::string& FullPath()
	{return m_szFileNmae;}

	/// 重設完整路徑
	void FullPath(const std::string& szFileName)
	{m_szFileNmae=szFileName;}

	/// 是否存在
	bool IsExist()
	{return IsExist(m_szFileNmae);}

	/// 是不是目錄
	bool IsFolder()
	{return IsFolder(m_szFileNmae);}

	/// 建立目錄
	void CreateFolder(const std::string& szPreview="666")
	{return CreateFolder(m_szFileNmae, szPreview);}

	/// 修改權限
	void Chmod(const std::string& szPreview)
	{return Chmod(m_szFileNmae, szPreview);}

	/// 修改擁有者
	void Chown(const std::string& szOwer, const std::string& szGroup)
	{return Chown(m_szFileNmae, szOwer, szGroup);}

	/// 移除
	void Remove()
	{return Remove(m_szFileNmae);}

	/// 複製
	FileInfo CopyTo(const std::string& szNewFile);
	
	/// 搬移
	FileInfo MoveTo(const std::string& szNewFile);
	
	/// 取得副檔名
	std::string Extension()
	{return Extension(m_szFileNmae);}

	/// 取得不包含副檔名的檔名
	std::string WithoutExtension();

	/// 取得路徑
	std::string Path()
	{return Path(m_szFileNmae);}

	/// 取得目錄內所有檔案
	FileCollection ToCollection();


// static method
public:
	static bool IsExist(const std::string& szFileName);
	static bool IsFolder(const std::string& szFileName);
	static void CreateFolder(const std::string& szFileName, const std::string& szPreview);
	static void Chmod(const std::string& szFileName, const std::string& szPreview);
	static void Chown(const std::string& szFileName, const std::string& szOwer, const std::string& szGroup);
	static void Copy(const std::string& szOldFile, const std::string& szNewFile);
	static void Move(const std::string& szOldFile, const std::string& szNewFile);
	static void Remove(const std::string& szFileName);
	static std::string Extension(const std::string& szFileName);
	static std::string Path(const std::string& szFileName);
};


/// 列舉目錄中所有的檔案目錄
class FileCollection
{
private:
	std::vector<FileInfo>	m_arrFiles;

public:
	FileCollection(const std::string& szFolderPath, bool ignoreParentFolder=true);

	int Size()
	{return m_arrFiles.size();}

	FileInfo& operator[](int i)
	{return m_arrFiles[i];}
};


FileCollection::FileCollection(const std::string& szFolderPath, bool ignoreParentFolder)
{
	DIR* _dir = opendir(szFolderPath.c_str());
	if(_dir==NULL) 
		throw FRAME_EXPCEPTION_1("Open folder failed.");

	while(true)
	{
		dirent* _dirent = readdir(_dir);
		if ((_dirent == NULL) || (_dirent->d_name == NULL))
			break;

		std::string _szFile(_dirent->d_name);
		if(ignoreParentFolder && (_szFile=="." || _szFile==".." || _szFile=="/"))
			continue;

		std::string _szFullPath;
		if(szFolderPath[szFolderPath.size()-1]=='/')
			_szFullPath = szFolderPath + _szFile;
		else
			_szFullPath = szFolderPath + "/" + _szFile;

		FileInfo _info(_szFullPath);
		m_arrFiles.push_back(_info);
	}

	closedir(_dir);
}


FileCollection FileInfo::ToCollection()
{
	return FileCollection(m_szFileNmae);
}


std::string FileInfo::Extension(const std::string& szFileName)
{
	int _pos = szFileName.find_last_of(".");
	if(_pos==std::string::npos)	return "";
	else						return szFileName.substr(_pos);
}


std::string FileInfo::Path(const std::string& szFileName)
{
	int _pos = szFileName.find_last_of("/");
	return szFileName.substr(0, _pos);
}


std::string FileInfo::WithoutExtension()
{
	std::string _szName = Name();
	int _pos = _szName.find_last_of(".");
	return _szName.substr(0, _pos);
}


std::string FileInfo::Name() const
{
	int _pos = m_szFileNmae.find_last_of("/");
	return m_szFileNmae.substr(_pos+1);
}


FileInfo FileInfo::CopyTo(const std::string& szNewFile)
{
	Copy(m_szFileNmae, szNewFile);
	return FileInfo(szNewFile);
}
	

FileInfo FileInfo::MoveTo(const std::string& szNewFile)
{
	Move(m_szFileNmae, szNewFile);
	return FileInfo(szNewFile);
}


void FileInfo::Remove(const std::string& szFileName)
{
	char _buff[1024];
	sprintf(_buff, "rm -rf %s\0", szFileName.c_str());
	if(system(_buff)!=0) throw FRAME_EXPCEPTION_1("Remove failed.");
}


void FileInfo::Move(const std::string& szOldFile, const std::string& szNewFile)
{
	char _buff[1024];
	sprintf(_buff, "mv %s %s\0", szOldFile.c_str(), szNewFile.c_str());
	if(system(_buff)!=0) throw FRAME_EXPCEPTION_1("Move failed.");
}


void FileInfo::Copy(const std::string& szOldFile, const std::string& szNewFile)
{
	char _buff[1024];
	sprintf(_buff, "cp -R %s %s\0", szOldFile.c_str(), szNewFile.c_str());
	if(system(_buff)!=0) throw FRAME_EXPCEPTION_1("Copy failed.");
}


void FileInfo::Chown(const std::string& szFileName, const std::string& szOwer, const std::string& szGroup)
{
	char _buff[1024];
	sprintf(_buff, "chown %s:%s %s\0", szOwer.c_str(), szGroup.c_str(), szFileName.c_str());
	if(system(_buff)!=0) throw FRAME_EXPCEPTION_1("Chown failed.");
}


void FileInfo::Chmod(const std::string& szFileName, const std::string& szPreview)
{
	char _buff[1024];
	sprintf(_buff, "chmod %s %s\0", szPreview.c_str(), szFileName.c_str());
	if(system(_buff)!=0) throw FRAME_EXPCEPTION_1("Chmod failed.");
}


void FileInfo::CreateFolder(const std::string& szFileName, const std::string& szPreview)
{
	char _buff[1024];
	sprintf(_buff, "mkdir -m %s -p %s\0", szPreview.c_str(), szFileName.c_str());
	if(system(_buff)!=0) throw FRAME_EXPCEPTION_1("Create folder failed.");
}


bool FileInfo::IsFolder(const std::string& szFileName)
{
	struct stat _info;
	if ( 0 != stat(szFileName.c_str(), &_info) )
		throw FRAME_EXPCEPTION_1("IsFolder failed.");

	return S_ISDIR(_info.st_mode);
}


bool FileInfo::IsExist(const std::string& szFileName)
{
	FILE* fp = fopen(szFileName.c_str(), "r");
	bool _rc = false;
	if (fp) 
	{
		fclose(fp);
		_rc = true;
	} 
	return _rc;
}


#endif
