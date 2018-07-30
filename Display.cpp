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

	// �E�B���h�n���h�����O���[�o����
	hDisplayWnd = hWnd;

	// ��ʃJ���[���[�h�̎擾
	hDC = GetDC(hWnd);
	uiBitsPix = GetDeviceCaps(hDC, BITSPIXEL);

	// �`��N���X�̏�����
	if(uiBitsPix == 32){
		pUDD = new CUseDirectDraw(hWnd, uiNumOfDisplay, TRUE);
	} else {
		pUDD = new CUseDirectDraw(hWnd, uiNumOfDisplay, FALSE);
	}
	if(!pUDD->IsAvailable()){
		MessageBox(NULL, TEXT("DirectDraw���������ł��܂���"),
			TEXT("�N���G���["), MB_OK | MB_ICONSTOP);
		PostQuitMessage(0);
	}

	// �f�B�X�v���C�T�C�Y�̐ݒ�
	uiDisplayWidth = 15 + IMAGE_INTERVAL + (PIXEL_SIZE * IMAGE_WIDTH + IMAGE_INTERVAL) * NUM_OF_ROW_DISPLAY;
	uiDisplayHeight = 38 + IMAGE_INTERVAL + (PIXEL_SIZE * IMAGE_HEIGHT + IMAGE_INTERVAL) * NUM_OF_COL_DISPLAY;
	SetWindowPos(hDisplayWnd, NULL, 0, 0, uiDisplayWidth, uiDisplayHeight, 
													(SWP_SHOWWINDOW|SWP_NOZORDER));

	hBitmap = LoadBitmap(hAppInstance, MAKEINTRESOURCE(IDB_BITMAP));

	// �E�N���b�N���j���[�̏����ݒ�
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

	// �J�[�\���̈ʒu���擾����
	pos.x = LOWORD(lp);
	pos.y = HIWORD(lp);

	// �J�[�\���ʒu���A�X�N���[�����W�ɕϊ�����
	ClientToScreen(hWnd, &pos );

	uiHorizontal = pos.x / (IMAGE_INTERVAL + PIXEL_SIZE * IMAGE_WIDTH);
	uiVertical = pos.y / (IMAGE_INTERVAL + PIXEL_SIZE * IMAGE_WIDTH);

	uiTargetDisplay = uiHorizontal + uiVertical * NUM_OF_ROW_DISPLAY;
	if(uiTargetDisplay > uiNumOfDisplay){
		uiTargetDisplay = 0;
	}

	TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, pos.x, pos.y, 0, hWnd, NULL );
}
