#include "CHandleRecData.h"
#include "CFileAccess.h"
#include "Global.h"


/******************************************************************************
 * Constructor
 ******************************************************************************/
CHandleRecData::CHandleRecData(LPCTSTR strSaveDir)
{
	TCHAR	strFilePath[NUM_OF_DISPLAY][SAVEDIR_NAME_BUF];
	TCHAR	strNumOfLoaded[32];
	TCHAR	*tmpStr;
	DWORD	dwRead;
	UINT	i, j;

	CFileAccess* pFD = new CFileAccess();
	pFD->setFileFilter("Binary Format\0*.bin\0All\0*.*\0", "*.bin");
	pFD->setInitialDir(strSaveDir);

	if(pFD->showOpenDialog()){
		for(i = 0; i < uiNumOfDisplay; i ++ ){
			pFD->copyStrFilePath(strFilePath[i]);
			tmpStr = strstr(strFilePath[i], "_");
			tmpStr++;
			_itoa(i, tmpStr, 10);
			lstrcat(strFilePath[i], ".bin");
			if(pFD->doesFileExist(strFilePath[i])){
				hFile[i] = CreateFile(strFilePath[i], GENERIC_READ, 0, 
						NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				uiNumOfLoaded = i + 1;
			} 
		}
		_itoa_s(uiNumOfLoaded, strNumOfLoaded, 32, 10);
		lstrcat(strNumOfLoaded, "個のファイルを読み込みました");
		MessageBox(NULL, strNumOfLoaded, "ファイル読み込み", MB_OK | MB_ICONINFORMATION);
		uiPresentFrame = 0;
		bAvailable = TRUE;
	} else {
		bAvailable = FALSE;
	}

	delete(pFD);

	// ファイル情報を再生用バッファへ格納
	for(i = 0; i < uiNumOfLoaded; i ++){
		for(j = 0; j < LOAD_BUF_LENGTH; j ++){
			ReadFile(hFile[i], (LPVOID*)loadBuff[i][j], IMAGE_DATA_LENGTH, &dwRead, NULL);
			if(dwRead != IMAGE_DATA_LENGTH){
				uiEndFrame = j;
				break;
			}
		}
	}

	// ファイルハンドラのクローズ
	for(i = 0; i < uiNumOfLoaded; i ++){
		CloseHandle(hFile[i]);
	}
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
CHandleRecData::~CHandleRecData()
{
}

/******************************************************************************
 * Is Available
 ******************************************************************************/
BOOL CHandleRecData::IsAvailable()
{
	return bAvailable;
}

/******************************************************************************
 * Increment Frame
 ******************************************************************************/
BOOL CHandleRecData::IncrementFrame()
{
	if(uiPresentFrame < uiEndFrame){
		uiPresentFrame++;
		return TRUE;
	} else {
		return FALSE;
	}
}

/******************************************************************************
 * Decrement Frame
 ******************************************************************************/
BOOL CHandleRecData::DecrementFrame()
{
	if(uiPresentFrame != 0){
		uiPresentFrame --;
		return TRUE;
	} else {
		return FALSE;
	}
}

/******************************************************************************
 * Get Image
 ******************************************************************************/
BYTE* CHandleRecData::GetImage(UINT uiNumOfImage)
{
	return loadBuff[uiNumOfImage][uiPresentFrame];
}

/******************************************************************************
 * Get Line
 ******************************************************************************/
BYTE* CHandleRecData::GetLine(UINT uiNumOfImage, UINT uiNumOfLine)
{
	return &loadBuff[uiNumOfImage][uiPresentFrame][uiNumOfLine * IMAGE_WIDTH];
}

/******************************************************************************
 * Get Number of Loaded Files
 ******************************************************************************/
UINT CHandleRecData::GetNumOfLoaded()
{
	return uiNumOfLoaded;
}

/******************************************************************************
 * Get Present Frame Number
 ******************************************************************************/
UINT CHandleRecData::GetPresentFrame()
{
	return uiPresentFrame;
}

/******************************************************************************
 * Set Present Frame Number
 ******************************************************************************/
VOID CHandleRecData::SetPresentFrame(UINT uiFrameToSet)
{
	if(uiFrameToSet >= 0 && uiFrameToSet <= uiEndFrame){
		uiPresentFrame = uiFrameToSet;
	}
}

/******************************************************************************
 * Get End Frame Number
 ******************************************************************************/
UINT CHandleRecData::GetEndFrame()
{
	return uiEndFrame;
}
