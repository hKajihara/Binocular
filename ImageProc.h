#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include<windows.h>

VOID ImageProc(BYTE** ppImage, UINT* pAppendData);
VOID SetNumOfProcImage();
VOID SetOverlay();
VOID PostImageProc(UINT uiNumOfSrcImage, UINT uiNumOfSrcImageAfterBlind,
				   UINT uiNumOfResultImage, BOOL bRecEnable);

#endif