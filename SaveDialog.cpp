#include <windows.h>
#include "resource.h"
#include "HandleRecData.h"
#include "Global.h"

VOID	InitSaveDlg(HWND);
VOID	SaveBitmap(HWND);
INT_PTR CALLBACK SaveImageDlgProc(HWND hDialogWindow, UINT uiMessage, 
							WPARAM wParameter, LPARAM lParameter);

UINT	uiNumOfFrame;

/******************************************************************************
 * Open Save Dialog
 ******************************************************************************/
VOID ShowSaveDialog(UINT uiNumOfFrameToSet)
{
	if(bReplaying){
		uiNumOfFrame = uiNumOfFrameToSet;
		DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_EXPORT), NULL, SaveImageDlgProc);
	} else {
		MessageBox(NULL, "No file to export", 
			"Warning", MB_OK | MB_ICONINFORMATION);
	}
}

/******************************************************************************
 * Save Dialog Procedure
 ******************************************************************************/
INT_PTR CALLBACK SaveImageDlgProc(HWND hDlgWnd, UINT uiMessage, 
							WPARAM wParameter, LPARAM lParameter)
{
	switch(uiMessage)
	{
		case WM_INITDIALOG:
			InitSaveDlg(hDlgWnd);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParameter))
			{	
				case IDC_SAVE:
					SaveBitmap(hDlgWnd);
					EndDialog(hDlgWnd ,0);
					break;
				case IDC_CANCEL:
					EndDialog(hDlgWnd ,0);
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hDlgWnd ,0);
			break;
	}
	return 0;
}

/******************************************************************************
 * Initiate Save Image Dialog 
 ******************************************************************************/
VOID InitSaveDlg(HWND hWnd)
{
	TCHAR	strEndFrame[8], strFrameInfo[32];

	if(bReplaying){
		_itoa_s(uiNumOfFrame, strEndFrame, 8, 10);
		lstrcpy(strFrameInfo, "Total Frame Number : ");
		lstrcat(strFrameInfo, strEndFrame);
		SetDlgItemText(hWnd, IDC_STATIC_TOTAL, strFrameInfo);
	}
}

/******************************************************************************
 * Save Bitmap
 ******************************************************************************/
VOID SaveBitmap(HWND hWnd)
{
	TCHAR	strInitFrameToSave[8], strEndFrameToSave[8];
	UINT	uiInitFrameToSave, uiEndFrameToSave;

	GetDlgItemText(hWnd, IDC_EDIT_START, strInitFrameToSave, 8);
	uiInitFrameToSave = atoi(strInitFrameToSave);
	GetDlgItemText(hWnd, IDC_EDIT_END, strEndFrameToSave, 8);
	uiEndFrameToSave = atoi(strEndFrameToSave);

	ExportBitmap(uiInitFrameToSave, uiEndFrameToSave);
}
