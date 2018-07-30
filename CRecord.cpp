#include "CRecord.h"
#include "Utils.h"

/******************************************************************************
 * Constructor
 ******************************************************************************/
CRecord::CRecord(LPCTSTR SaveDir, UINT uiNumOfImageToRec)
{
	TCHAR	strFilePath[NUM_OF_DISPLAY][SAVEDIR_NAME_BUF];
	TCHAR	strFileNumber[3];
	TCHAR	strFilePathAppend[SAVEDIR_NAME_BUF];
	UINT	i;

	uiNumOfImage = uiNumOfImageToRec;

	// 記録用ファイル名の設定
	lstrcpy(strFilePath[0], SaveDir);
	GetTimeString(strFilePath[0]);

	for(i = 1; i < uiNumOfImage; i ++){
		lstrcpy(strFilePath[i], strFilePath[0]);
	}
	lstrcpy(strFilePathAppend, strFilePath[0]);

	for(i = 0; i < uiNumOfImage; i ++){
		_itoa_s(i, strFileNumber, 3, 10);
		lstrcat(strFilePath[i], "_");
		lstrcat(strFilePath[i], strFileNumber);
		lstrcat(strFilePath[i], ".bin");
	}
	lstrcat(strFilePathAppend, ".csv");

	for(i = 0; i < uiNumOfImage; i ++){
		hFile[i] = CreateFile(strFilePath[i], GENERIC_WRITE, 0, 
					NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile[i] == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, "ファイルがオープンできません", "エラー", MB_OK | MB_ICONERROR);
		} 		
	}
	{
		hFileAppend = CreateFile(strFilePathAppend, GENERIC_WRITE, 0, 
					NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFileAppend == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, "ファイルがオープンできません", "エラー", MB_OK | MB_ICONERROR);
		}
	}
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
CRecord::~CRecord()
{
	UINT i;

	for(i = 0; i < uiNumOfImage; i ++){
		CloseHandle(hFile[i]);
	}
	CloseHandle(hFileAppend);
}

/******************************************************************************
 * Record Image
 ******************************************************************************/
VOID CRecord::RecordImage(UINT nRecImage, BYTE* ImageDataBuff)
{
	DWORD	dwWrite;

	WriteFile(hFile[nRecImage], (LPVOID*)ImageDataBuff, IMAGE_DATA_LENGTH, &dwWrite, NULL);
}

/******************************************************************************
 * Record Append Data
 ******************************************************************************/
VOID CRecord::RecordAppendData(UINT* AppendDataBuff)
{
	UINT	i;
	DWORD	dwWrite;
	TCHAR	strAppendData[NUM_OF_APPEND_DATA][APPEND_DATA_DIGIT];
	TCHAR	strAppendDataAll[NUM_OF_APPEND_DATA * APPEND_DATA_DIGIT];

	ZeroMemory(strAppendDataAll, sizeof(strAppendDataAll));
	for(i = 0; i < NUM_OF_APPEND_DATA; i ++){
		_itoa_s(AppendDataBuff[i], strAppendData[i], APPEND_DATA_DIGIT, 10);
		lstrcat(strAppendDataAll, strAppendData[i]);
		if(i == NUM_OF_APPEND_DATA - 1){
			lstrcat(strAppendDataAll, "\n");
		} else {
			lstrcat(strAppendDataAll, ",");
		}
	}
	WriteFile(hFileAppend, (LPVOID*)strAppendDataAll, lstrlen(strAppendDataAll), &dwWrite, NULL);
}