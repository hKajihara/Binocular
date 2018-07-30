#ifndef CUSEDIRECTDRAW_H
#define CUSEDIRECTDRAW_H

#include <windows.h>
#include <ddraw.h>
#include "ConstantValues.h"

#pragma comment(lib,"ddraw.lib")


class CUseDirectDraw{
	private:
		LPDIRECTDRAW        lpDD;
		LPDIRECTDRAWSURFACE lpDDSPrimary;
		LPDIRECTDRAWSURFACE lpDDSBack[NUM_OF_DISPLAY];
		HWND				hWnd;
		UINT				numOfDisplay;
		RECT				ScreenRect;
		BOOL				useTrueColor;
		BOOL				initSucceeded;
		BOOL	InitDirectDraw();
		VOID	TermDirectDraw();
		BOOL	InitDirectDrawSurface();
		VOID	TermDirectDrawSurface();
		VOID	GetDrawingDomain(LPRECT, UINT);
		VOID	DrawGrayImage16(UINT nDisplay, BYTE* pbtVisionData);
		VOID	DrawGrayImage32(UINT nDisplay, BYTE* pbtVisionData);
		VOID	DrawColorImage16(UINT nDisplay, BYTE* pbtVisionData);
		VOID	DrawColorImage32(UINT nDisplay, BYTE* pbtVisionData);
		VOID	DrawFullColorImage16(UINT nDisplay, INT* piVisionData);
		VOID	DrawFullColorImage32(UINT nDisplay, INT* piVisionData);
		VOID	DrawOverlayImage16(UINT nDisplay, BYTE* pbtVisionData);
		VOID	DrawOverlayImage32(UINT nDisplay, BYTE* pbtVisionData);
	public:
				CUseDirectDraw(HWND hWnd, UINT nDisplay, BOOL trueColor);
				~CUseDirectDraw();
		BOOL	IsAvailable();
		VOID	DrawGrayImage(UINT nDisplay, BYTE* pbtVisionData);
		VOID	DrawColorImage(UINT nDisplay, BYTE* pbtVisionData);
		VOID	DrawFullColorImage(UINT nDisplay, INT* piVisionData);
		VOID	DrawOverlayImage(UINT nDisplay, BYTE* pbtVisionData);
};

#endif