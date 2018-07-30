/******************************************************************************
 * Pre-processor
 ******************************************************************************/
#include "CUseDirectDraw.h"

#define RELEASE(x) {if(x) { x->Release(); x = NULL;}}

/******************************************************************************
 * Constructor
 ******************************************************************************/
CUseDirectDraw::CUseDirectDraw(HWND hWindow, UINT nDisplay, BOOL trueColor)
{
	initSucceeded = FALSE;
 	hWnd = hWindow;
	numOfDisplay = nDisplay;
	SetRect(&ScreenRect, 0, 0, PIXEL_SIZE * IMAGE_WIDTH, PIXEL_SIZE * IMAGE_HEIGHT);

	if(trueColor){
		useTrueColor = TRUE;
	} else {
		useTrueColor = FALSE;
	}
	if(!InitDirectDraw()){
        MessageBox(hWnd, "DirectDraw Init Error", "DirectDraw", MB_OK);
        SendMessage(hWnd, WM_CLOSE, 0, 0);
		return;
    }
    if(!InitDirectDrawSurface()){
        MessageBox(hWnd, "DirectDraw Surface Init Error", 
            "DirectDraw", MB_OK);
        SendMessage(hWnd, WM_CLOSE, 0, 0);
		return;
    }
	initSucceeded = TRUE;
}

/******************************************************************************
 * Destructor
 ******************************************************************************/
CUseDirectDraw::~CUseDirectDraw()
{
	TermDirectDrawSurface();
	TermDirectDraw();
}

/******************************************************************************
 * Check If Initialization was succeeded
 ******************************************************************************/
BOOL CUseDirectDraw::IsAvailable()
{
	return initSucceeded;
}

/******************************************************************************
 * Initialize DirectDraw Object
 ******************************************************************************/
BOOL CUseDirectDraw::InitDirectDraw()
{
    HRESULT hResult;

    hResult = DirectDrawCreate(NULL, &lpDD, NULL);
    if(hResult != DD_OK) return FALSE;

    hResult = lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
    if(hResult != DD_OK) {
        RELEASE(lpDD);
        return FALSE;
    }

	return TRUE;
}

/******************************************************************************
 * Release DirectDraw Object
 ******************************************************************************/
VOID CUseDirectDraw::TermDirectDraw()
{
	if(lpDD != NULL){
		RELEASE(lpDD);
	}
}

/******************************************************************************
 * Constructing DirectDraw Surface Object
 ******************************************************************************/
BOOL CUseDirectDraw::InitDirectDrawSurface()
{
    HRESULT				hResult;
    DDSURFACEDESC		ddSurfaceDesc;
	LPDIRECTDRAWCLIPPER lpClipper;
	UINT i;

    // Primary Surface Discription
    ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);
    ddSurfaceDesc.dwFlags = DDSD_CAPS;
    ddSurfaceDesc.dwBackBufferCount = 1;
    ddSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE; 
    hResult = lpDD->CreateSurface(&ddSurfaceDesc, &lpDDSPrimary, NULL);
    if(hResult != DD_OK)
		return FALSE;

	// Clipper for Primary Surface
	if(lpDD->CreateClipper(0,&lpClipper,NULL)!=DD_OK)
		return FALSE;
	if(lpClipper->SetHWnd(0,hWnd)!=DD_OK)
		return FALSE;
	if(lpDDSPrimary->SetClipper(lpClipper)!=DD_OK){
		return FALSE;
	} else {
		lpClipper->Release();
	}

    // Back Buffer 0 & 1 Discription
  	ddSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddSurfaceDesc.dwHeight = PIXEL_SIZE * IMAGE_HEIGHT;
	ddSurfaceDesc.dwWidth = PIXEL_SIZE * IMAGE_WIDTH;
	ddSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

	for(i = 0; i < numOfDisplay; i++){
		hResult = lpDD->CreateSurface(&ddSurfaceDesc ,&lpDDSBack[i],NULL);
		if(hResult != DD_OK)
			return FALSE;
	}

    return TRUE;
}

/******************************************************************************
 * Release DirectDraw Surface Object
 ******************************************************************************/
VOID CUseDirectDraw::TermDirectDrawSurface()
{
	UINT i;
	if(lpDD != NULL){
		RELEASE(lpDDSPrimary);
		for(i = 0; i < numOfDisplay; i ++){
			RELEASE(lpDDSBack[i]);
		}
	}
}

/******************************************************************************
 * Draw Gray Scale Image
 ******************************************************************************/
VOID CUseDirectDraw::DrawGrayImage(UINT nDisplay, BYTE* pbtVisionData)
{
	if(useTrueColor){
		DrawGrayImage32(nDisplay, pbtVisionData);
	} else {
		DrawGrayImage16(nDisplay, pbtVisionData);
	}
}

/******************************************************************************
 * Draw Color Image
 ******************************************************************************/
VOID CUseDirectDraw::DrawColorImage(UINT nDisplay, BYTE* pbtVisionData)
{
	if(useTrueColor){
		DrawColorImage32(nDisplay, pbtVisionData);
	} else {
		DrawColorImage16(nDisplay, pbtVisionData);
	}
}

/******************************************************************************
 * Draw Full Color Image
 ******************************************************************************/
VOID CUseDirectDraw::DrawFullColorImage(UINT nDisplay, INT* piVisionData)
{
	if(useTrueColor){
		DrawFullColorImage32(nDisplay, piVisionData);
	} else {
		DrawFullColorImage16(nDisplay, piVisionData);
	}
}

/******************************************************************************
 * Draw Overlay Image
 ******************************************************************************/
VOID CUseDirectDraw::DrawOverlayImage(UINT nDisplay, BYTE* pbtVisionData)
{
	if(useTrueColor){
		DrawOverlayImage32(nDisplay, pbtVisionData);
	} else {
		DrawOverlayImage16(nDisplay, pbtVisionData);
	}
}

/******************************************************************************
 * Draw Gray Scale Image in 16 bit Color Mode
 ******************************************************************************/
VOID CUseDirectDraw::DrawGrayImage16(UINT nDisplay, BYTE* pbtVisionData)
{
 	RECT			rScreenRect;
	HRESULT			hResult;
	DDSURFACEDESC	ddSurfaceDesc;
	int				i, j, k, s;

	GetDrawingDomain(&rScreenRect, nDisplay);
    ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);

	hResult = lpDDSBack[nDisplay]->Lock(NULL, &ddSurfaceDesc, DDLOCK_WAIT, NULL);

	if(hResult == DD_OK){
		BYTE red, blue, green;

		//16-bit color mode
		{
			USHORT		Color;
			PUSHORT		lpColor[PIXEL_SIZE];
			
			for(i = 0; i < IMAGE_HEIGHT; i++){
				for(s = 0; s < PIXEL_SIZE; s++){
					lpColor[s] = (PUSHORT) ddSurfaceDesc.lpSurface + 
										(PIXEL_SIZE * i + s) * ddSurfaceDesc.lPitch / 2;
				}
				for(j = 0; j < IMAGE_WIDTH; j++){
					red   = 0x1F & (*pbtVisionData >> 3);
					green = 0x3F & (*pbtVisionData >> 2);
					blue  = 0x1F & (*pbtVisionData >> 3);
					Color = (red<<11) | (green<<5) | blue;
					for(k = 0; k < PIXEL_SIZE; k++){
						for(s = 0; s < PIXEL_SIZE; s++){
							*lpColor[s] = Color;
							lpColor[s]++;
						}
					}
					pbtVisionData++;
				}
			}
		}
		lpDDSBack[nDisplay]->Unlock(NULL);	
		lpDDSPrimary->Blt(&rScreenRect,lpDDSBack[nDisplay],&ScreenRect,DDFLIP_WAIT,NULL);
	}
}

/******************************************************************************
 * Draw Gray Scale Image in 32 bit Color Mode
 ******************************************************************************/
VOID CUseDirectDraw::DrawGrayImage32(UINT nDisplay, BYTE* pbtVisionData)
{
 	RECT			rScreenRect;
	HRESULT			hResult;
	DDSURFACEDESC	ddSurfaceDesc;
	int				i, j, k, s;

	GetDrawingDomain(&rScreenRect, nDisplay);
    ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);

	hResult = lpDDSBack[nDisplay]->Lock(NULL, &ddSurfaceDesc, DDLOCK_WAIT, NULL);

	if(hResult == DD_OK){
		BYTE red, blue, green;

		{
			COLORREF	Color;
			LPCOLORREF	lpColor[PIXEL_SIZE];
			
			for(i = 0; i < IMAGE_HEIGHT; i++){
				for(s = 0; s < PIXEL_SIZE; s++){
					lpColor[s] = (LPCOLORREF) ddSurfaceDesc.lpSurface + 
											(PIXEL_SIZE * i + s) * ddSurfaceDesc.lPitch / 4;
				}
				for(j = 0; j < IMAGE_WIDTH; j++){
					red   = *pbtVisionData;
					green = *pbtVisionData;
					blue  = *pbtVisionData;
					Color = 0x00FFFFFF & ((blue<<16) | (green<<8) | red);
					for(k = 0; k < PIXEL_SIZE; k++){
						for(s = 0; s < PIXEL_SIZE; s++){
							*lpColor[s] = Color;
							lpColor[s]++;
						}
					}
					pbtVisionData++;
				}
			}
		}

		lpDDSBack[nDisplay]->Unlock(NULL);
		lpDDSPrimary->Blt(&rScreenRect,lpDDSBack[nDisplay],&ScreenRect,DDFLIP_WAIT,NULL);
	}
}

/******************************************************************************
 * Draw Color Image in 16 bit Color Mode
 ******************************************************************************/
VOID CUseDirectDraw::DrawColorImage16(UINT nDisplay, BYTE* pbtVisionData)
{
 	RECT			rScreenRect;
	HRESULT			hResult;
	DDSURFACEDESC	ddSurfaceDesc;
	int				i, j, k, s;

	GetDrawingDomain(&rScreenRect, nDisplay);
    ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);

	hResult = lpDDSBack[nDisplay]->Lock(NULL, &ddSurfaceDesc, DDLOCK_WAIT, NULL);

	if(hResult == DD_OK){
		BYTE red, blue, green;

		{
			USHORT		Color;
			PUSHORT		lpColor[PIXEL_SIZE];
			
			for(i = 0; i < IMAGE_HEIGHT; i++){
				for(s = 0; s < PIXEL_SIZE; s++){
					lpColor[s] = (PUSHORT) ddSurfaceDesc.lpSurface + 
										(PIXEL_SIZE * i + s) * ddSurfaceDesc.lPitch / 2;
				}

				for(j = 0; j < IMAGE_WIDTH; j++){
					if(*pbtVisionData < 32){
						red   = 0;
						green = 0;
						blue  = *pbtVisionData;
						Color = (red<<11) | (green<<5) | blue;
					} else if(*pbtVisionData < 96){
						red   = 0;
						green = *pbtVisionData - 32;
						blue  = 31;
						Color = (red<<11) | (green<<5) | blue;
					} else if(*pbtVisionData < 128){
						red   = 0;
						green = 63;
						blue  = 31 - (0x1F & *pbtVisionData);
						Color = (red<<11) | (green<<5) | blue;
					} else if(*pbtVisionData < 160){
						red   = 0x1F & *pbtVisionData;
						green = 63;
						blue  = 0;
						Color = (red<<11) | (green<<5) | blue;
					} else if(*pbtVisionData < 192){
						red   = 31;
						green = 63 - ((0x1F & *pbtVisionData) << 1);
						blue  = 0;
						Color = (red<<11) | (green<<5) | blue;
					} else if(*pbtVisionData < 224){
						red   = 31;
						green = 0;
						blue  = 0x1F & *pbtVisionData;
						Color = (red<<11) | (green<<5) | blue;
					} else {
						red   = 31;
						green = (0x1F & *pbtVisionData) << 1;
						blue  = 31;
						Color = (red<<11) | (green<<5) | blue;
					}

					for(k = 0; k < PIXEL_SIZE; k++){
						for(s = 0; s < PIXEL_SIZE; s++){
							*lpColor[s] = Color;
							lpColor[s]++;
						}
					}
					pbtVisionData++;
				}
			}
		}

		lpDDSBack[nDisplay]->Unlock(NULL);
		lpDDSPrimary->Blt(&rScreenRect,lpDDSBack[nDisplay],&ScreenRect,DDFLIP_WAIT,NULL);
	}
}

/******************************************************************************
 * Draw Color Image in 32 bit Color Mode
 ******************************************************************************/
VOID CUseDirectDraw::DrawColorImage32(UINT nDisplay, BYTE* pbtVisionData)
{
 	RECT			rScreenRect;
	HRESULT			hResult;
	DDSURFACEDESC	ddSurfaceDesc;
	int				i, j, k, s;

	GetDrawingDomain(&rScreenRect, nDisplay);
    ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);

	hResult = lpDDSBack[nDisplay]->Lock(NULL, &ddSurfaceDesc, DDLOCK_WAIT, NULL);

	if(hResult == DD_OK){
		BYTE red, blue, green;

		{
			COLORREF	Color;
			LPCOLORREF	lpColor[PIXEL_SIZE];
			
			for(i = 0; i < IMAGE_HEIGHT; i++){
				for(s = 0; s < PIXEL_SIZE; s++){
					lpColor[s] = (LPCOLORREF) ddSurfaceDesc.lpSurface + 
											(PIXEL_SIZE * i + s) * ddSurfaceDesc.lPitch / 4;
				}
				for(j = 0; j < IMAGE_WIDTH; j++){
					if(*pbtVisionData < 32){
						red   = 0;
						green = 0;
						blue  = *pbtVisionData << 3;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else if(*pbtVisionData < 96){
						red   = 0;
						green = (*pbtVisionData - 32) << 2;
						blue  = 255;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else if(*pbtVisionData < 128){
						red   = 0;
						green = 255;
						blue  = 255 - ((0x1F & *pbtVisionData) << 3);
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else if(*pbtVisionData < 160){
						red   = (0x1F & *pbtVisionData) << 3;
						green = 255;
						blue  = 0;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else if(*pbtVisionData < 192){
						red   = 255;
						green = 255 - ((0x1F & *pbtVisionData) << 3);
						blue  = 0;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else if(*pbtVisionData < 224){
						red   = 255;
						green = 0;
						blue  = (0x1F & *pbtVisionData) << 3;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else {
						red   = 255;
						green = (0x1F & *pbtVisionData) << 3;
						blue  = 255;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					}
					for(k = 0; k < PIXEL_SIZE; k++){
						for(s = 0; s < PIXEL_SIZE; s++){
							*lpColor[s] = Color;
							lpColor[s]++;
						}
					}
					pbtVisionData++;
				}
			}
		}

		lpDDSBack[nDisplay]->Unlock(NULL);
		lpDDSPrimary->Blt(&rScreenRect,lpDDSBack[nDisplay],&ScreenRect,DDFLIP_WAIT,NULL);
	}
}

/******************************************************************************
 * Draw Full Color Image in 16 bit Color Mode
 ******************************************************************************/
VOID CUseDirectDraw::DrawFullColorImage16(UINT nDisplay, INT* piVisionData)
{
 	RECT			rScreenRect;
	HRESULT			hResult;
	DDSURFACEDESC	ddSurfaceDesc;
	int				i, j, k, s;

	GetDrawingDomain(&rScreenRect, nDisplay);
    ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);

	hResult = lpDDSBack[nDisplay]->Lock(NULL, &ddSurfaceDesc, DDLOCK_WAIT, NULL);

	if(hResult == DD_OK){
		BYTE red, blue, green;

		{
			USHORT		Color;
			PUSHORT		lpColor[PIXEL_SIZE];
			
			for(i = 0; i < IMAGE_HEIGHT; i++){
				for(s = 0; s < PIXEL_SIZE; s++){
					lpColor[s] = (PUSHORT) ddSurfaceDesc.lpSurface + 
										(PIXEL_SIZE * i + s) * ddSurfaceDesc.lPitch / 2;
				}

				for(j = 0; j < IMAGE_WIDTH; j++){
					red = (*piVisionData & 0x00FF00000) >> (3 + 16);
					green = (*piVisionData & 0x0000FF000) >> (2 + 8);
					blue = (*piVisionData & 0x0000000FF) >> 3;

					Color = (red<<11) | (green<<5) | blue;

					for(k = 0; k < PIXEL_SIZE; k++){
						for(s = 0; s < PIXEL_SIZE; s++){
							*lpColor[s] = Color;
							lpColor[s]++;
						}
					}
					piVisionData++;
				}
			}
		}

		lpDDSBack[nDisplay]->Unlock(NULL);
		lpDDSPrimary->Blt(&rScreenRect,lpDDSBack[nDisplay],&ScreenRect,DDFLIP_WAIT,NULL);
	}
}

/******************************************************************************
 * Draw Full Color Image in 32 bit Color Mode
 ******************************************************************************/
VOID CUseDirectDraw::DrawFullColorImage32(UINT nDisplay, INT* piVisionData)
{
 	RECT			rScreenRect;
	HRESULT			hResult;
	DDSURFACEDESC	ddSurfaceDesc;
	int				i, j, k, s;

	GetDrawingDomain(&rScreenRect, nDisplay);
    ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);

	hResult = lpDDSBack[nDisplay]->Lock(NULL, &ddSurfaceDesc, DDLOCK_WAIT, NULL);

	if(hResult == DD_OK){

		{
			COLORREF	Color;
			LPCOLORREF	lpColor[PIXEL_SIZE];
			
			for(i = 0; i < IMAGE_HEIGHT; i++){
				for(s = 0; s < PIXEL_SIZE; s++){
					lpColor[s] = (LPCOLORREF) ddSurfaceDesc.lpSurface + 
											(PIXEL_SIZE * i + s) * ddSurfaceDesc.lPitch / 4;
				}
				for(j = 0; j < IMAGE_WIDTH; j++){
					Color = 0x00FFFFFF & (*piVisionData);

					for(k = 0; k < PIXEL_SIZE; k++){
						for(s = 0; s < PIXEL_SIZE; s++){
							*lpColor[s] = Color;
							lpColor[s]++;
						}
					}
					piVisionData ++;
				}
			}
		}

		lpDDSBack[nDisplay]->Unlock(NULL);
		lpDDSPrimary->Blt(&rScreenRect,lpDDSBack[nDisplay],&ScreenRect,DDFLIP_WAIT,NULL);
	}
}

/******************************************************************************
 * Draw Overlay Image in 16 bit Color Mode
 ******************************************************************************/
VOID CUseDirectDraw::DrawOverlayImage16(UINT nDisplay, BYTE* pbtVisionData)
{
 	RECT			rScreenRect;
	HRESULT			hResult;
	DDSURFACEDESC	ddSurfaceDesc;
	int				i, j, k, s;

	GetDrawingDomain(&rScreenRect, nDisplay);
    ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);

	hResult = lpDDSBack[nDisplay]->Lock(NULL, &ddSurfaceDesc, DDLOCK_WAIT, NULL);

	if(hResult == DD_OK){
		BYTE red, blue, green;

		{
			USHORT		Color;
			PUSHORT		lpColor[PIXEL_SIZE];
			
			for(i = 0; i < IMAGE_HEIGHT; i++){
				for(s = 0; s < PIXEL_SIZE; s++){
					lpColor[s] = (PUSHORT) ddSurfaceDesc.lpSurface + 
										(PIXEL_SIZE * i + s) * ddSurfaceDesc.lPitch / 2;
				}
				for(j = 0; j < IMAGE_WIDTH; j++){
					if(*pbtVisionData < 32){
						red   = 0;
						green = 0;
						blue  = *pbtVisionData;
						Color = (red<<11) | (green<<5) | blue;
					} else if(*pbtVisionData < 96){
						red   = 0;
						green = *pbtVisionData - 32;
						blue  = 31;
						Color = (red<<11) | (green<<5) | blue;
					} else if(*pbtVisionData < 128){
						red   = 0;
						green = 63;
						blue  = 31 - (0x1F & *pbtVisionData);
						Color = (red<<11) | (green<<5) | blue;
					} else if(*pbtVisionData < 160){
						red   = 0x1F & *pbtVisionData;
						green = 63;
						blue  = 0;
						Color = (red<<11) | (green<<5) | blue;
					} else if(*pbtVisionData < 192){
						red   = 31;
						green = 63 - ((0x1F & *pbtVisionData) << 1);
						blue  = 0;
						Color = (red<<11) | (green<<5) | blue;
					} else if(*pbtVisionData < 224){
						red   = 31;
						green = 0;
						blue  = 0x1F & *pbtVisionData;
						Color = (red<<11) | (green<<5) | blue;
					} else {
						red   = 31;
						green = (0x1F & *pbtVisionData) << 1;
						blue  = 31;
						Color = (red<<11) | (green<<5) | blue;
					}
					if(*pbtVisionData != 0){
						for(k = 0; k < PIXEL_SIZE; k++){
							for(s = 0; s < PIXEL_SIZE; s++){
								*lpColor[s] = Color;
								lpColor[s]++;
							}
						}
					} else {
						for(s = 0; s < PIXEL_SIZE; s++){
							lpColor[s] += PIXEL_SIZE;
						}
					}
					pbtVisionData++;
				}
			}
		}

		lpDDSBack[nDisplay]->Unlock(NULL);
		lpDDSPrimary->Blt(&rScreenRect,lpDDSBack[nDisplay],&ScreenRect,DDFLIP_WAIT,NULL);
	}
}

/******************************************************************************
 * Draw Overlay Image in 32 bit Color Mode
 ******************************************************************************/
VOID CUseDirectDraw::DrawOverlayImage32(UINT nDisplay, BYTE* pbtVisionData)
{
 	RECT			rScreenRect;
	HRESULT			hResult;
	DDSURFACEDESC	ddSurfaceDesc;
	int				i, j, k, s;

	GetDrawingDomain(&rScreenRect, nDisplay);
    ZeroMemory(&ddSurfaceDesc, sizeof(DDSURFACEDESC));
    ddSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);

	hResult = lpDDSBack[nDisplay]->Lock(NULL, &ddSurfaceDesc, DDLOCK_WAIT, NULL);

	if(hResult == DD_OK){
		BYTE red, blue, green;

		{
			COLORREF	Color;
			LPCOLORREF	lpColor[PIXEL_SIZE];
			
			for(i = 0; i < IMAGE_HEIGHT; i++){
				for(s = 0; s < PIXEL_SIZE; s++){
					lpColor[s] = (LPCOLORREF) ddSurfaceDesc.lpSurface + 
											(PIXEL_SIZE * i + s) * ddSurfaceDesc.lPitch / 4;
				}

				for(j = 0; j < IMAGE_WIDTH; j++){
					if(*pbtVisionData < 32){
						red   = 0;
						green = 0;
						blue  = *pbtVisionData << 3;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else if(*pbtVisionData < 96){
						red   = 0;
						green = (*pbtVisionData - 32) << 2;
						blue  = 255;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else if(*pbtVisionData < 128){
						red   = 0;
						green = 255;
						blue  = 255 - ((0x1F & *pbtVisionData) << 3);
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else if(*pbtVisionData < 160){
						red   = (0x1F & *pbtVisionData) << 3;
						green = 255;
						blue  = 0;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else if(*pbtVisionData < 192){
						red   = 255;
						green = 255 - ((0x1F & *pbtVisionData) << 3);
						blue  = 0;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else if(*pbtVisionData < 224){
						red   = 255;
						green = 0;
						blue  = (0x1F & *pbtVisionData) << 3;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					} else {
						red   = 255;
						green = (0x1F & *pbtVisionData) << 3;
						blue  = 255;
						Color = 0x00FFFFFF & ((red<<16) | (green<<8) | blue);
					}
					if(*pbtVisionData != 0){
						for(k = 0; k < PIXEL_SIZE; k++){
							for(s = 0; s < PIXEL_SIZE; s++){
								*lpColor[s] = Color;
								lpColor[s]++;
							}
						}
					} else {
						for(s = 0; s < PIXEL_SIZE; s++){
							lpColor[s] += PIXEL_SIZE;
						}
					}
					pbtVisionData++;
				}
			}
		}

		lpDDSBack[nDisplay]->Unlock(NULL);
		lpDDSPrimary->Blt(&rScreenRect,lpDDSBack[nDisplay],&ScreenRect,DDFLIP_WAIT,NULL);
	}
}

/******************************************************************************
 * Get the Drawng Domain for DirectDraw
 ******************************************************************************/
VOID CUseDirectDraw::GetDrawingDomain(LPRECT lpClientRect, UINT nDisplay)
{
	POINT	ClientPos;
	UINT	uiHorizontal, uiVertical, uiImageHeight, uiImageWidth;
	
	ZeroMemory(&ClientPos,sizeof(ClientPos));
	ClientToScreen(hWnd,&ClientPos);

	uiImageHeight = PIXEL_SIZE * IMAGE_HEIGHT;
	uiImageWidth = PIXEL_SIZE * IMAGE_WIDTH;

	uiHorizontal = nDisplay % NUM_OF_ROW_DISPLAY;
	uiVertical = nDisplay / NUM_OF_ROW_DISPLAY;

	lpClientRect->bottom = (uiVertical + 1) * (uiImageHeight + IMAGE_INTERVAL);
	lpClientRect->top = IMAGE_INTERVAL + uiVertical * (uiImageHeight + IMAGE_INTERVAL);
	lpClientRect->right	= (uiHorizontal + 1) * (uiImageWidth + IMAGE_INTERVAL);
	lpClientRect->left = IMAGE_INTERVAL + uiHorizontal * (uiImageWidth + IMAGE_INTERVAL);

	OffsetRect(lpClientRect,ClientPos.x,ClientPos.y);
}