#pragma once

///////////////////////////////////////////////////////
//	***             * * * *      ****        ****    //
//	***  	      *         *     **          **     //
//	***  	     *           *    **          **     //
//	***  	     *           *    **************     //
//	***	     **  *           *    **          **     //
//	***********   *         *     **          **     //
//	***********	    * * * *      ****        ****    // 
///////////////////////////////////////////////////////2009-08-20 @loach
/*
*
* �˿�����ɶ�������ѧϰ֮��,����������ҵ��;
* ���������ҵ��;,һ�鷨��׷���Լ��е�!
*
* ���ߣ� loach.h(������)
*
* ʱ�䣺 2009-08-20
*/
#include "qyutil/qyconfig.h"
#include <string>

DEFINE_NAMESPACE(qy)

class QYUTIL_API QyDirectory
{
public:
	/** for win32 */
	int getDirectoryA(HMODULE handle ,char* pszPath , int len);
	int getDirectoryW(HMODULE handle ,wchar_t* pszPath , int len);

	std::string  getDirectoryA(HMODULE handle);
	std::wstring getDirectoryW(HMODULE handle);

	/** win32 system: documents + "\\" */
	const wchar_t* getSystemDocumentPath(void);

	/** win32 system: appdata + "\\" */
	const wchar_t* getSystemAppDataPath(void);

	/**  */	
	bool createFolder(const wchar_t* folder);
	bool createDirectory(const wchar_t* path);
	bool createDirectoryA(const char* path);

	bool deleteFolder(const std::wstring& folder);
	void removeFolderByRecursive(const std::wstring &folder);

	bool isAbsoluteFilePathA(const char* pszPath);
    bool isAbsoluteFilePathW(const wchar_t* pszPath);

	bool getFilePathA(/*out*/char* szPath,size_t iPathLen/*MAX_PATH+1*/ , const char* szFile);
    bool getFilePathW(/*out*/wchar_t* szPath,size_t iPathLen/*MAX_PATH+1*/ , const wchar_t* szFile);

	void pathW(wchar_t* path,size_t len);
	void pathW(std::wstring& path);

	void pathA(char* path , size_t len);
	void pathA(std::string& path);
};

END_NAMESPACE(qy)

