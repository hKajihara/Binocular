#include <windows.h>
#include <mmsystem.h>
#include "resource.h"
#include "ConstantValues.h"
#include "Global.h"
#include "Display.h"
#include "HandleRecData.h"
#pragma comment(lib,"winmm.lib")

/******************************************************************************
 * Global Variables
 ******************************************************************************/
HBITMAP		hBitmap;
HMENU		hMenu, hSubMenu;
UINT		uiTargetDisplay;

/******************************************************************************
 * Procedure Declaration
 ******************************************************************************/
VOID InitiateDisplay(HWND);
VOID PaintBackGround(HWND);
VOID RButtonUp(HWND, LPARAM);
extern void OnClose(HWND);

/******************************************************************************
/* Display Dialog Procedure
 ******************************************************************************/
INT_PTR CALLBACK DisplayDlgProc(HWND hDlgWnd, UINT uiMessage, 
							WPARAM wParameter, LPARAM lParameter)
{
	switch(uiMessage)
	{
		case WM_INITDIALOG:
			InitiateDisplay(hDlgWnd);
			break;
		case WM_MOVE:
			if(bReplaying){
				DrawOneFrame(hDlgWnd);
			} else {
				InvalidateRect(hDlgWnd, NULL, TRUE);
			}
			break;
		case WM_PAINT:
			if(bReplaying){
				DrawOneFrame(hDlgWnd);
			}
			PaintBackGround(hDlgWnd);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParameter))
			{
				case ID_POPUP_GRAY:
					btUseColor[uiTargetDisplay] = 0;
					break;
				case ID_POPUP_COLOR:
					btUseColor[uiTargetDisplay] = 1;
					break;
				default:
					break;
			}
			break;
		case WM_RBUTTONUP:
			RButtonUp(hDlgWnd, lParameter);
			break;
		case WM_CLOSE:
			OnClose(hCtrlWnd);
			break;
	}
	return 0;
}

/******************************************************************************
/* Initiate Display
 ******************************************************************************/
VOID InitiateDisplay(HWND hWnd)
{
	HDC		hDC;
	UINT	uiBitsPix;
	UINT	uiDisplayWidth, uiDisplayHeight;

	// ウィンドハンドラをグローバルに
	hDisplayWnd = hWnd;

	// 画面カラーモードの取得
	hDC = GetDC(hWnd);
	uiBitsPix = GetDeviceCaps(hDC, BITSPIXEL);

	// 描画クラスの初期化
	if(uiBitsPix == 32){
		pUDD = new CUseDirectDraw(hWnd, uiNumOfDisplay, TRUE);
	} else {
		pUDD = new CUseDirectDraw(hWnd, uiNumOfDisplay, FALSE);
	}
	if(!pUDD->IsAvailable()){
		MessageBox(NULL, TEXT("DirectDrawが初期化できません"),
			TEXT("起動エラー"), MB_OK | MB_ICONSTOP);
		PostQuitMessage(0);
	}

	// ディスプレイサイズの設定
	uiDisplayWidth = 15 + IMAGE_INTERVAL + (PIXEL_SIZE * IMAGE_WIDTH + IMAGE_INTERVAL) * NUM_OF_ROW_DISPLAY;
	uiDisplayHeight = 38 + IMAGE_INTERVAL + (PIXEL_SIZE * IMAGE_HEIGHT + IMAGE_INTERVAL) * NUM_OF_COL_DISPLAY;
	SetWindowPos(hDisplayWnd, NULL, 0, 0, uiDisplayWidth, uiDisplayHeight, 
													(SWP_SHOWWINDOW|SWP_NOZORDER));

	hBitmap = LoadBitmap(hAppInstance, MAKEINTRESOURCE(IDB_BITMAP));

	// 右クリックメニューの初期設定
	hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU3));
	hSubMenu = GetSubMenu(hMenu, 0 );
}

/******************************************************************************
/* Finalize Display
 ******************************************************************************/
VOID FinalizeDisplay(HWND hWnd)
{
	DeleteObject(hBitmap);
	delete(pUDD);
	EndDialog(hWnd, 0);
}

/******************************************************************************
/* Background Paint Procedure
 ******************************************************************************/
VOID PaintBackGround(HWND hWnd)
{
	HDC			hDC, hBuffer;
	PAINTSTRUCT ps;
	UINT		i, uiHorizontal, uiVertical, uiImageHeight, uiImageWidth;
	UINT		uiNumOfUsedDisplay;

	uiImageHeight = PIXEL_SIZE * IMAGE_HEIGHT;
	uiImageWidth = PIXEL_SIZE * IMAGE_WIDTH;
	hDC = BeginPaint(hWnd , &ps);
	hBuffer = CreateCompatibleDC(hDC);
	SelectObject(hBuffer , hBitmap);

	if(bConnecting){
		uiNumOfUsedDisplay = uiNumOfRecvImageAfterBlind + uiNumOfProcImage;
	} else if(bReplaying){
		if(bPlayWithProc){
			uiNumOfUsedDisplay = uiNumOfReplaySource + uiNumOfReplayProc;
		} else {
			uiNumOfUsedDisplay = uiNumOfReplaySource;
		}
	} else {
		uiNumOfUsedDisplay = 0;
	}

	for(i = uiNumOfUsedDisplay; i < uiNumOfDisplay; i ++){
		uiHorizontal = i % NUM_OF_ROW_DISPLAY;
		uiVertical = i / NUM_OF_ROW_DISPLAY;

		BitBlt(hDC, IMAGE_INTERVAL + uiHorizontal * (uiImageWidth + IMAGE_INTERVAL), 
					IMAGE_INTERVAL + uiVertical * (uiImageHeight + IMAGE_INTERVAL), 
					uiImageWidth, uiImageHeight, 
					hBuffer, 0, 0, SRCCOPY);
	}

	DeleteDC(hBuffer);
	EndPaint(hWnd, &ps);
}

/******************************************************************************
/* Pushing the Mouse R button excutes this procedure
 ******************************************************************************/
VOID RButtonUp(HWND hWnd, LPARAM lp)
{
	POINT	pos;
	UINT	uiHorizontal, uiVertical;

	// カーソルの位置を取得する
	pos.x = LOWORD(lp);
	pos.y = HIWORD(lp);

	// カーソル位置を、スクリーン座標に変換する
	ClientToScreen(hWnd, &pos );

	uiHorizontal = pos.x / (IMAGE_INTERVAL + PIXEL_SIZE * IMAGE_WIDTH);
	uiVertical = pos.y / (IMAGE_INTERVAL + PIXEL_SIZE * IMAGE_WIDTH);

	uiTargetDisplay = uiHorizontal + uiVertical * NUM_OF_ROW_DISPLAY;
	if(uiTargetDisplay > uiNumOfDisplay){
		uiTargetDisplay = 0;
	}

	TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, pos.x, pos.y, 0, hWnd, NULL );
}
