#include "BitmapStream.h"

INT SaveBitmap(LPCTSTR strFileName, PBYTE pImage, PBYTE pOverlay, BYTE btColor, UINT uiWidth, UINT uiHeight)
{
	UINT	x, y;
	HANDLE  hFile;
	DWORD   dwWrite;
	DWORD   dwWidthBytes = uiWidth * 3;
	PBYTE   LineBuffer = NULL;
	PBYTE	pSourceLine = NULL;
	PBYTE	pOverlayLine = NULL;
	
	if(strFileName == NULL || pImage == NULL || uiWidth == 0 || uiHeight == 0){
		return -1;
	}

	hFile = CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_READ, 
						NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return -1;

	{
		BITMAPFILEHEADER   BitmapFileHeader = { 0 };

		BitmapFileHeader.bfType			= 0x4D42;
		BitmapFileHeader.bfOffBits		= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		BitmapFileHeader.bfSize			= BitmapFileHeader.bfOffBits + dwWidthBytes * uiHeight;

		dwWrite = sizeof(BITMAPFILEHEADER);
		WriteFile(hFile, &BitmapFileHeader, dwWrite, &dwWrite, NULL);
		if(dwWrite != sizeof(BITMAPFILEHEADER)){
			goto _return;
		}
	}

	LineBuffer = new BYTE[dwWidthBytes];
	if(LineBuffer == NULL){
		goto _return;
	}

	{
		BITMAPINFOHEADER   BitmapInfoHeader = { 0 };

		BitmapInfoHeader.biSize			= sizeof(BitmapInfoHeader);
		BitmapInfoHeader.biWidth		= uiWidth;
		BitmapInfoHeader.biHeight		= uiHeight;
		BitmapInfoHeader.biPlanes		= 1;
		BitmapInfoHeader.biBitCount		= 24;
		BitmapInfoHeader.biCompression	= BI_RGB;
		BitmapInfoHeader.biSizeImage	= BitmapInfoHeader.biHeight * dwWidthBytes;
		
		dwWrite = sizeof(BitmapInfoHeader);
		WriteFile(hFile, &BitmapInfoHeader, dwWrite, &dwWrite, NULL);
		if(dwWrite != sizeof(BitmapInfoHeader)){
			goto _return;
		}
	}

	if(!btColor){
		if(pOverlay == NULL){	// exporting gray scale image
			for(y = 0; y < uiHeight; y++){
				ZeroMemory(LineBuffer, dwWidthBytes);
				pSourceLine = &pImage[(uiHeight - y - 1) * uiWidth];

				for(x = 0; x < uiWidth; x++){
					LineBuffer[x * 3 + 2] = pSourceLine[x];	// Red
					LineBuffer[x * 3 + 1] = pSourceLine[x];	// Green
					LineBuffer[x * 3 + 0] = pSourceLine[x];	// Blue
				}

				dwWrite = dwWidthBytes;
				WriteFile(hFile, LineBuffer, dwWrite, &dwWrite, NULL);
				if(dwWrite != dwWidthBytes){
					goto _return;
				}
			}
		} else {				// exporting gray scale image with overlay
			for(y = 0; y < uiHeight; y++){
				ZeroMemory(LineBuffer, dwWidthBytes);
				pSourceLine = &pImage[(uiHeight - y - 1) * uiWidth];
				pOverlayLine = &pOverlay[(uiHeight - y - 1) * uiWidth];

				for(x = 0; x < uiWidth; x++){
					if(pOverlayLine[x] == 0){
						LineBuffer[x * 3 + 2] = pSourceLine[x];	// Red
						LineBuffer[x * 3 + 1] = pSourceLine[x];	// Green
						LineBuffer[x * 3 + 0] = pSourceLine[x];	// Blue
					} else if(pOverlayLine[x] < 32){
						LineBuffer[x * 3 + 2] = 0;								// Red
						LineBuffer[x * 3 + 1] = 0;								// Green
						LineBuffer[x * 3 + 0] = pOverlayLine[x] << 3;			// Blue
					} else if(pOverlayLine[x] < 96){
						LineBuffer[x * 3 + 2] = 0;								// Red
						LineBuffer[x * 3 + 1] = (pOverlayLine[x] - 32) << 2;	// Green
						LineBuffer[x * 3 + 0] = 255;							// Blue
					} else if(pOverlayLine[x] < 128){
						LineBuffer[x * 3 + 2] = 0;								// Red
						LineBuffer[x * 3 + 1] = 255;							// Green
						LineBuffer[x * 3 + 0] = 255 - ((0x1F & pOverlayLine[x]) << 3);	// Blue
					} else if(pOverlayLine[x] < 160){
						LineBuffer[x * 3 + 2] = (0x1F & pOverlayLine[x]) << 3;	// Red
						LineBuffer[x * 3 + 1] = 255;							// Green
						LineBuffer[x * 3 + 0] = 0;								// Blue
					} else if(pOverlayLine[x] < 192){
						LineBuffer[x * 3 + 2] = 255;							 // Red
						LineBuffer[x * 3 + 1] = 255 - ((0x1F & pOverlayLine[x]) << 3);	// Green
						LineBuffer[x * 3 + 0] = 0;								// Blue
					} else if(pOverlayLine[x] < 224){
						LineBuffer[x * 3 + 2] = 255;							// Red
						LineBuffer[x * 3 + 1] = 0;								//Green
						LineBuffer[x * 3 + 0] = (0x1F & pOverlayLine[x]) << 3;	// Blue
					} else {
						LineBuffer[x * 3 + 2] = 255;							// Red
						LineBuffer[x * 3 + 1] = (0x1F & pOverlayLine[x]) << 3;	// Green
						LineBuffer[x * 3 + 0] = 255;							// Blue
					}
				}

				dwWrite = dwWidthBytes;
				WriteFile(hFile, LineBuffer, dwWrite, &dwWrite, NULL);
				if(dwWrite != dwWidthBytes){
					goto _return;
				}
			}

		}
	} else {					// exporting color image
		for(y = 0; y < uiHeight; y++){
			ZeroMemory(LineBuffer, dwWidthBytes);
			pSourceLine = &pImage[(uiHeight - y - 1) * uiWidth];

			for(x = 0; x < uiWidth; x++){
				if(pSourceLine[x] < 32){
					LineBuffer[x * 3 + 2] = 0;								// Red
					LineBuffer[x * 3 + 1] = 0;								// Green
					LineBuffer[x * 3 + 0] = pSourceLine[x] << 3;			// Blue
				} else if(pSourceLine[x] < 96){
					LineBuffer[x * 3 + 2] = 0;								// Red
					LineBuffer[x * 3 + 1] = (pSourceLine[x] - 32) << 2;		// Green
					LineBuffer[x * 3 + 0] = 255;							// Blue
				} else if(pSourceLine[x] < 128){
					LineBuffer[x * 3 + 2] = 0;								// Red
					LineBuffer[x * 3 + 1] = 255;							// Green
					LineBuffer[x * 3 + 0] = 255 - ((0x1F & pSourceLine[x]) << 3); // Blue
				} else if(pSourceLine[x] < 160){
					LineBuffer[x * 3 + 2] = (0x1F & pSourceLine[x]) << 3;	// Red
					LineBuffer[x * 3 + 1] = 255;							// Green
					LineBuffer[x * 3 + 0] = 0;								// Blue
				} else if(pSourceLine[x] < 192){
					LineBuffer[x * 3 + 2] = 255;							// Red
					LineBuffer[x * 3 + 1] = 255 - ((0x1F & pSourceLine[x]) << 3);	// Green
					LineBuffer[x * 3 + 0] = 0;								// Blue
				} else if(pSourceLine[x] < 224){
					LineBuffer[x * 3 + 2] = 255;							// Red
					LineBuffer[x * 3 + 1] = 0;								//Green
					LineBuffer[x * 3 + 0] = (0x1F & pSourceLine[x]) << 3;	// Blue
				} else {
					LineBuffer[x * 3 + 2] = 255;							// Red
					LineBuffer[x * 3 + 1] = (0x1F & pSourceLine[x]) << 3;	// Green
					LineBuffer[x * 3 + 0] = 255;							// Blue
				}
			}

			dwWrite = dwWidthBytes;
			WriteFile(hFile, LineBuffer, dwWrite, &dwWrite, NULL);
			if(dwWrite != dwWidthBytes){
				goto _return;
			}
		}
	}

	return 1;


_return:
	if(LineBuffer != NULL){
		delete [] LineBuffer;
		LineBuffer = NULL;
	}

	if(hFile != INVALID_HANDLE_VALUE){
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return 1;
}