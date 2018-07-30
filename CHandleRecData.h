#ifndef CHANDLERECDATA_H
#define CHANDLERECDATA_H

#include <windows.h>
#include "ConstantValues.h"


class CHandleRecData{
	private:
		HANDLE	hFile[NUM_OF_DISPLAY];
		BOOL	bAvailable;
		UINT	uiNumOfLoaded;
		UINT	uiEndFrame;
		UINT	uiPresentFrame;
		BYTE	loadBuff[NUM_OF_DISPLAY][LOAD_BUF_LENGTH][IMAGE_DATA_LENGTH];

public:
				CHandleRecData(LPCTSTR strSaveDir);
				~CHandleRecData();
		BOOL	IsAvailable();
		BOOL	IncrementFrame();
		BOOL	DecrementFrame();
		BYTE*	GetImage(UINT uiNumOfImage);
		BYTE*	GetLine(UINT uiNumOfImage, UINT uiNumOfLine);
		UINT	GetNumOfLoaded();
		VOID	SetPresentFrame(UINT uiFrameToSet);
		UINT	GetPresentFrame();
		UINT	GetEndFrame();
};


#endif
