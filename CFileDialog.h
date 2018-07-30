#ifndef CFILEDIALOG_H
#define CFILEDIALOG_H

#include <windows.h>
#include <commdlg.h>
#include <Shlwapi.h>

#pragma comment(lib,"comdlg32.lib")
#pragma comment(lib,"shlwapi.lib")

class CFileDialog{
	private:
		OPENFILENAME	ofn;
		TCHAR			strFilePath[256];
	public:
				CFileDialog();
				~CFileDialog();
		BOOL	showOpenDialog();
		BOOL	showSaveDialog();
		VOID	setFileFilter(LPCTSTR strFilter, LPCTSTR strSuffix);
		VOID	setInitialDir(LPCTSTR strInitDir);
		VOID	copyStrFilePath(TCHAR* strBufToCopy);
		BOOL	doesFileExist(LPCTSTR strFileToCheck);
};

#endif