/******************************************************************************
 * Pre-processor
 ******************************************************************************/
#include "WaveForm.h"
#include "ConstantValues.h"
#include "Utils.h"
#include "Global.h"
#include "resource.h"

#define NUM_OF_CHANNEL 4
#define BLANK_H 50
#define BLANK_V 5

/******************************************************************************
 * Global Variables
 ******************************************************************************/
BYTE	btLineBuff[NUM_OF_CHANNEL][IMAGE_WIDTH];
BYTE*	pImage[NUM_OF_CHANNEL];
HWND	hWaveFormWnd;
UINT	uiLineNumberToDraw = 50;
bool	bUpdateValid;
bool	bDrawCh[NUM_OF_CHANNEL] = {true, true, true, true};

/******************************************************************************
 * Procedure Declaration
 ******************************************************************************/
VOID OnInitWaveFormDlg(HWND);
VOID ReBufferWaveForm();
VOID SaveWaveForm(VOID);
INT_PTR CALLBACK WaveFormDlgProc(HWND hDialogWindow, UINT uiMessage, 
							WPARAM wParameter, LPARAM lParameter);

/******************************************************************************
 * Open Dialog for Wave Form Viewer
 ******************************************************************************/
VOID ShowWaveForm()
{
	UINT	i;

	for(i = 0; i < NUM_OF_CHANNEL; i ++){
		pImage[i] = NULL;
	}
	ZeroMemory(btLineBuff, sizeof(BYTE) * NUM_OF_CHANNEL * IMAGE_WIDTH);
	DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_WAVE), NULL, WaveFormDlgProc);
}

/******************************************************************************
/* Dialog Procedure
 ******************************************************************************/
INT_PTR CALLBACK WaveFormDlgProc(HWND hDlgWnd, UINT uiMessage, 
							WPARAM wParameter, LPARAM lParameter)
{
	TCHAR	strNumberToDraw[8];


	switch(uiMessage)
	{
		case WM_INITDIALOG:
			OnInitWaveFormDlg(hDlgWnd);
			break;
		case WM_PAINT:
			DrawWaveForm();
			break;
		case WM_COMMAND:
			switch(LOWORD(wParameter))
			{	
				case ID_MENU_SAVE:
					SaveWaveForm();
					break;
				case ID_MENU_STOP:
					if(bUpdateValid){
						bUpdateValid = FALSE;
					} else{
						bUpdateValid = TRUE;
					}
					break;
				case IDC_MN_TEN:
					if(uiLineNumberToDraw >= 10){
						uiLineNumberToDraw -= 10;
					} else {
						uiLineNumberToDraw = 0;
					}
					_itoa_s(uiLineNumberToDraw, strNumberToDraw, 8, 10);
					SetDlgItemText(hDlgWnd, IDC_LINE_EDIT, strNumberToDraw);
					ReBufferWaveForm();
					DrawWaveForm();
					break;
				case IDC_MN_ONE:
					if(uiLineNumberToDraw != 0){
						uiLineNumberToDraw -= 1;
					} 
					_itoa_s(uiLineNumberToDraw, strNumberToDraw, 8, 10);
					SetDlgItemText(hDlgWnd, IDC_LINE_EDIT, strNumberToDraw);
					ReBufferWaveForm();
					DrawWaveForm();
					break;
				case IDC_PL_ONE:
					if(uiLineNumberToDraw != IMAGE_HEIGHT - 1){
						uiLineNumberToDraw += 1;
					} 
					_itoa_s(uiLineNumberToDraw, strNumberToDraw, 8, 10);
					SetDlgItemText(hDlgWnd, IDC_LINE_EDIT, strNumberToDraw);
					ReBufferWaveForm();
					DrawWaveForm();
					break;
				case IDC_PL_TEN:
					if(uiLineNumberToDraw < IMAGE_HEIGHT - 10){
						uiLineNumberToDraw += 10;
					} else {
						uiLineNumberToDraw = IMAGE_HEIGHT - 1;
					}
					_itoa_s(uiLineNumberToDraw, strNumberToDraw, 8, 10);
					SetDlgItemText(hDlgWnd, IDC_LINE_EDIT, strNumberToDraw);
					ReBufferWaveForm();
					DrawWaveForm();
					break;
				case IDC_CH1:
					if(bDrawCh[0] == false){
						bDrawCh[0] = true;
						SetDlgItemText(hDlgWnd, IDC_CH1, "Ch1");
					} else {
						bDrawCh[0] = false;
						SetDlgItemText(hDlgWnd, IDC_CH1, "OFF");
					}
					break;
				case IDC_CH2:
					if(bDrawCh[1] == false){
						bDrawCh[1] = true;
						SetDlgItemText(hDlgWnd, IDC_CH2, "Ch2");
					} else {
						bDrawCh[1] = false;
						SetDlgItemText(hDlgWnd, IDC_CH2, "OFF");
					}
					break;
				case IDC_CH3:
					if(bDrawCh[2] == false){
						bDrawCh[2] = true;
						SetDlgItemText(hDlgWnd, IDC_CH3, "Ch3");
					} else {
						bDrawCh[2] = false;
						SetDlgItemText(hDlgWnd, IDC_CH3, "OFF");
					}
					break;
				case IDC_CH4:
					if(bDrawCh[3] == false){
						bDrawCh[3] = true;
						SetDlgItemText(hDlgWnd, IDC_CH4, "Ch4");
					} else {
						bDrawCh[3] = false;
						SetDlgItemText(hDlgWnd, IDC_CH4, "OFF");
					}
					break;
			}
			break;
		case WM_CLOSE:
		bWaveFormReady = FALSE;
		CloseWaveForm();
		break;
	}
	return 0;
}

/******************************************************************************
 * Show Wave Form
 ******************************************************************************/
VOID OnInitWaveFormDlg(HWND hWnd)
{
	TCHAR	strTemp[8];
	UINT	uiDisplayWidth, uiDisplayHeight;

	hWaveFormWnd = hWnd;
	bWaveFormReady = TRUE;
	bUpdateValid = TRUE;
	_itoa_s(uiLineNumberToDraw, strTemp, 8, 10);
	SetDlgItemText(hWnd, IDC_LINE_EDIT, strTemp);

	// Setting display size
	uiDisplayWidth = 70 + IMAGE_WIDTH * 3;
	uiDisplayHeight = 65 + 256;
	SetWindowPos(hWnd, NULL, 800, 0, uiDisplayWidth, uiDisplayHeight, 
													(SWP_SHOWWINDOW|SWP_NOZORDER));

}

/******************************************************************************
 * Buffer Wave Form
 ******************************************************************************/
VOID BufferWaveForm(BYTE* btImageData, UINT uiChannel)
{
	UINT	i;

	if(bUpdateValid && uiChannel < NUM_OF_CHANNEL){
		pImage[uiChannel] = btImageData;
		for(i = 0; i < IMAGE_WIDTH; i++){
			btLineBuff[uiChannel][i] = btImageData[IMAGE_WIDTH * uiLineNumberToDraw + i];
		}
	}
}

/******************************************************************************
 * ReBuffer Wave Form
 ******************************************************************************/
VOID ReBufferWaveForm()
{
	UINT	i, j;

	for(i = 0; i < NUM_OF_CHANNEL; i ++){
		for(j = 0; j < IMAGE_WIDTH; j++){
			if(pImage[i] != NULL){
				btLineBuff[i][j] = pImage[i][IMAGE_WIDTH * uiLineNumberToDraw + j];
			}
		}
	}
}


/******************************************************************************
 * Reset Buffer
 ******************************************************************************/
VOID ResetWaveFormBuffer()
{
	UINT	i;

	for(i = 0; i < NUM_OF_CHANNEL; i ++){
		pImage[i] = NULL;
	}
	ZeroMemory(btLineBuff, sizeof(BYTE) * NUM_OF_CHANNEL * IMAGE_WIDTH);
}

/******************************************************************************
 * Draw Wave Form
 ******************************************************************************/
VOID DrawWaveForm()
{
	HDC				hDC;
	HPEN			hPen[NUM_OF_CHANNEL], hOldPen;
	HBRUSH			hBrush, hOldBrush;
	UINT			i, j;

	hDC = GetDC(hWaveFormWnd);

	// Draw Background
	hPen[0] = CreatePen(PS_SOLID, 1, RGB(0,0,0) );
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	Rectangle(hDC, BLANK_H, BLANK_V, BLANK_H + 3 * IMAGE_WIDTH - 1, BLANK_V + 256);
	DeleteObject(hPen[0]);
	DeleteObject(hBrush);

	// Create pens to draw
	hPen[0] = CreatePen(PS_SOLID, 1, RGB(255,0,0));
	hPen[1] = CreatePen(PS_SOLID, 1, RGB(0,255,255));
	hPen[2] = CreatePen(PS_SOLID, 1, RGB(0,255,0));
	hPen[3] = CreatePen(PS_SOLID, 1, RGB(255,255,255));

	// Draw Lines
	for(j = 0; j < NUM_OF_CHANNEL; j++){
		SelectObject(hDC, hPen[j]);
		MoveToEx(hDC, BLANK_H + 1, BLANK_V + 255 - btLineBuff[j][0], NULL);
		if(bDrawCh[j]){
			for(i = 1; i < IMAGE_WIDTH; i++){
				LineTo(hDC, BLANK_H + i * 3 + 1, BLANK_V + 255 - btLineBuff[j][i]);
			}
		}
	}

	// Delete pens
	for(j = 0; j < NUM_OF_CHANNEL; j++){
		DeleteObject(hPen[j]);
	}

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);

	ReleaseDC(hWaveFormWnd, hDC);
}

/******************************************************************************
 * Save Wave Form
 ******************************************************************************/
VOID SaveWaveForm()
{
	HANDLE	hFile;
	TCHAR	strFilePath[128], strWaveForm[6];
	DWORD	dwWrite;
	UINT	i, j;

	lstrcpy(strFilePath, SaveDir);
	GetTimeString(strFilePath);
	lstrcat(strFilePath, "-line.csv");

	hFile = CreateFile(strFilePath, GENERIC_WRITE, 0, 
						NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, "Cannot open file", 
				"File Error", MB_OK | MB_ICONERROR);
	} else {
		for(i = 0; i < IMAGE_WIDTH; i ++){
			for(j = 0; j < NUM_OF_CHANNEL; j ++){
				ZeroMemory(strWaveForm, sizeof(strWaveForm));
				_itoa_s(btLineBuff[j][i], strWaveForm, 6, 10);
				if(j == NUM_OF_CHANNEL - 1){
					lstrcat(strWaveForm, "\n");
				} else {
					lstrcat(strWaveForm, ",");
				}
				WriteFile(hFile, (LPVOID*)strWaveForm, lstrlen(strWaveForm), &dwWrite, NULL);
			}
		}
		CloseHandle(hFile);
	}
}

/******************************************************************************
 * Close Wave Form Window
 ******************************************************************************/
VOID CloseWaveForm()
{
	EndDialog(hWaveFormWnd ,0);
}
