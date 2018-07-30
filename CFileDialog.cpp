#include "CFileDialog.h"

/******************************************************************************
 * Constructor
 ******************************************************************************/
CFileDialog::CFileDialog()
{
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	lstrcpy(strFilePath, "*.*");
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = strFilePath;
	ofn.nMaxFile = sizeof(strFilePath);
	ofn.lpstrFilter = "All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}
/******************************************************************************
 * Destructor
 ******************************************************************************/
CFileDialog::~CFileDialog()
{
}

/******************************************************************************
 * Show Open File Dialog
 ******************************************************************************/
BOOL CFileDialog::showOpenDialog()
{
	if( !GetOpenFileName(&ofn) ){
		MessageBox(NULL, "ファイルが選択されていません", 
			TEXT("TITLE"), MB_OK | MB_ICONSTOP);
		return FALSE;
	} else {
		return TRUE;
	}
}

/******************************************************************************
 * Show Save File Dialog
 ******************************************************************************/
BOOL CFileDialog::showSaveDialog()
{
	if( !GetSaveFileName(&ofn) ){
		MessageBox(NULL, "ファイルが選択されていません", 
			TEXT("TITLE"), MB_OK | MB_ICONSTOP);
		return FALSE;
	} else {
		return TRUE;
	}
}

/******************************************************************************
 * FileFilter Setter
 ******************************************************************************/
VOID CFileDialog::setFileFilter(LPCTSTR strFilter, LPCTSTR strSuffix)
{
	ofn.lpstrFilter = strFilter;
	lstrcpy(strFilePath, strSuffix);
}

/******************************************************************************
 * Initial Directory Setter
 ******************************************************************************/
VOID CFileDialog::setInitialDir(LPCTSTR strInitDir)
{
	ofn.lpstrInitialDir = strInitDir;
}

/******************************************************************************
 * File Existence Checker
 ******************************************************************************/
BOOL CFileDialog::doesFileExist(LPCTSTR strFileToCheck)
{
	if(PathFileExists(strFileToCheck)){
		return TRUE;
	} else {
		return FALSE;
	}
}

/******************************************************************************
 * FilePath Getter
 ******************************************************************************/
VOID CFileDialog::copyStrFilePath(TCHAR* strBufToCopy)
{
	strcpy(strBufToCopy, this->strFilePath);
}
