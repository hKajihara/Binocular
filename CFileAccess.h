#ifndef CFILEACCESS_H
#define CFILEACCESS_H

#include <windows.h>
#include <commdlg.h>
#include <Shlwapi.h>

#pragma comment(lib,"comdlg32.lib")
#pragma comment(lib,"shlwapi.lib")

class CFileAccess{
	private:
		OPENFILENAME	ofn;
		TCHAR			strFilePath[256];
	public:
				CFileAccess();
				~CFileAccess();
		BOOL	showOpenDialog();
		BOOL	showSaveDialog();
		VOID	setFileFilter(LPCTSTR strFilter, LPCTSTR strSuffix);
		VOID	setInitialDir(LPCTSTR strInitDir);
		VOID	copyStrFilePath(TCHAR* strBufToCopy);
		BOOL	doesFileExist(LPCTSTR strFileToCheck);
};

#endif