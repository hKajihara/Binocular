#ifndef CRECORD_H
#define CRECORD_H

#include <windows.h>
#include "ConstantValues.h"

class CRecord{
	private:
		HANDLE	hFile[NUM_OF_DISPLAY];
		HANDLE	hFileAppend;
		UINT	uiNumOfImage;
	public:
				CRecord(LPCTSTR SaveDir, UINT uiNumOfImageToRec);
				~CRecord();
		VOID	RecordImage(UINT nRecImage, BYTE* ImageDataBuff);
		VOID	RecordAppendData(UINT* AppendDataBuff);
};

#endif
