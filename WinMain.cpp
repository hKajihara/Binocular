/******************************************************************************
 * Pre-processor
 ******************************************************************************/
#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include "resource.h"
#include "ConstantValues.h"
#include "Global.h"
#include "Display.h"
#include "ThreadProc.h"
#include "WaveForm.h"
#include "HandleRecData.h"
#include "ImageProc.h"
#include "Utils.h"

/******************************************************************************
 * Procedure Declaration
 ******************************************************************************/
void OnInitDlg(HWND);
void InitThread();
void OnClose(HWND);
void ShowDisplay(HWND);
void Connect(HWND);
void Disconnect(HWND);
void RecordButtonDown(HWND);
void SendButtonDown(HWND);
void GpSetButtonDown(HWND);
void ShowWaveButtonDown(HWND);
INT_PTR CALLBACK CtrlDlgProc(HWND hDialogWindow, UINT uiMessage, 
							WPARAM wParameter, LPARAM lParameter);

/******************************************************************************
 * Global Variables
 ******************************************************************************/
HANDLE		hDisplayThread;
UINT		uiSendEditID[NUM_OF_SEND_DATA];

/******************************************************************************
** Entry Point
 ******************************************************************************/
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, INT nShowCmd)
{
	hAppInstance = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CTRL), NULL, CtrlDlgProc);

	return 0;
}

/******************************************************************************
/* Control Dialog Procedure
 ******************************************************************************/
INT_PTR CALLBACK CtrlDlgProc(HWND hDlgWnd, UINT uiMessage, 
							WPARAM wParameter, LPARAM lParameter)
{
	switch(uiMessage)
	{
		case WM_INITDIALOG:
			OnInitDlg(hDlgWnd);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParameter))
			{	
				case IDC_CONNECT:
					if(bConnecting){
						Disconnect(hDlgWnd);
					} else {
						UnLoad(hDlgWnd);
						InvalidateRect(hDlgWnd, NULL, TRUE);
						Connect(hDlgWnd);
					}
					break;
				case IDC_LOAD:
					if(bReplaying){
						UnLoad(hDlgWnd);
					} else {
                        Disconnect(hDlgWnd);
						InvalidateRect(hDlgWnd, NULL, TRUE);
						Load(hDlgWnd);
					}
					break;
				case IDC_PLAY:
					PlayButtonDown(hDlgWnd);
					break;
				case IDC_WPROC:
					PlayWithProcCheck(hDlgWnd);
					break;
				case IDC_SLOW:
					ForwardButtonDown(hDlgWnd);
					break;
				case IDC_BACK:
					BackwardButtonDown(hDlgWnd);
					break;
				case IDC_STOP:
					StopButtonDown(hDlgWnd);
					break;
				case IDC_RWD:
					RwdButtonDown(hDlgWnd);
					break;
				case IDC_RECORD:
					RecordButtonDown(hDlgWnd);
					break;
				case IDC_SEND:
					SendButtonDown(hDlgWnd);
					break;
				case IDC_GP_SET:
					GpSetButtonDown(hDlgWnd);
					break;
				case ID_WINDOW_SHOWWAVE:
					ShowWaveButtonDown(hDlgWnd);
					break;
				case ID_FILE_EXPORT:
					ExportMenuClick();
					break;
				case ID_FILE_EXIT:
					OnClose(hDlgWnd);
					break;
			}
			break;
		case WM_CLOSE:
			OnClose(hDlgWnd);
			exit(0);
			break;
	}
	return 0;
}

/******************************************************************************
/* Start Up Procedure
 ******************************************************************************/
void InitValueSet(LPCTSTR lineBuff, LPCTSTR ValueName, LPTSTR Value)
{
	TCHAR* pStr;

	if((StrStr(lineBuff, ValueName)) != NULL){
		pStr = StrStr(lineBuff, "=");
		pStr++;
		while(*pStr != '\0') {
			*Value = *pStr;
			Value++;
			pStr++;
		}
		*Value = '\0';
	}
}

void OnInitDlg(HWND hWnd)
{
	HANDLE	hInitFile;
	DWORD	dwRead;
	TCHAR	readBuff[INIT_FILE_BUF_LENGTH], lineBuff[INIT_FILE_LINE_LENGTH];
	TCHAR	InitColor[3][10];
	TCHAR	strInitGpValue[NUM_OF_GP_IN][32], strInitSendValue[NUM_OF_SEND_DATA][8];
//	TCHAR	strNumOfBlindImage[8];
	TCHAR	strBuff[32], strTemp[8];
	INT		i, j, k;

	hCtrlWnd = hWnd;

	// Importing initial settings
	hInitFile = CreateFile("InSight.ini", GENERIC_READ, 0, 
			   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hInitFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, "Cannot open initial file", 
			"Initialization Error", MB_OK | MB_ICONERROR);
	} else {
		ReadFile(hInitFile, (LPVOID*)readBuff, INIT_FILE_BUF_LENGTH, &dwRead, NULL);
		i = 0;
		j = 0;
		while(i < (INT) dwRead){
			lineBuff[i - j] = readBuff[i];
			if(readBuff[i] == '\012'){
				lineBuff[i - j - 1] = '\0';
				InitValueSet(lineBuff, "SAVE_DIR", SaveDir);
				InitValueSet(lineBuff, "INIT_COLOR_0", InitColor[0]);
				InitValueSet(lineBuff, "INIT_COLOR_1", InitColor[1]);
				InitValueSet(lineBuff, "INIT_COLOR_2", InitColor[2]);
				//InitValueSet(lineBuff, "NUM_OF_BLIND", strNumOfBlindImage);
				//uiNumOfBlind = (UINT) atoi(strNumOfBlindImage);
				uiNumOfBlind = 0;
				for(k = 0; k < NUM_OF_SEND_DATA; k ++){
					lstrcpy(strBuff, "SEND_DATA_");
					_itoa_s(k, strTemp, 8, 10);
					if (k < 10) lstrcat(strBuff, "0");
					lstrcat(strBuff, strTemp);
					InitValueSet(lineBuff, strBuff, strInitSendValue[k]);
				}
				for(k = 0; k < NUM_OF_GP_IN; k ++){
					lstrcpy(strBuff, "GP_IN_");
					_itoa_s(k, strTemp, 8, 10);
					lstrcat(strBuff, strTemp);
					InitValueSet(lineBuff, strBuff, strInitGpValue[k]);
				}
				j = i + 1;
			}
			i ++;
		}
		CloseHandle(hInitFile);
	}

	// Initializing valiables for FPGA
	uiSendEditID[0] = IDC_SEND_EDIT0;
	for(i = 1; i < NUM_OF_SEND_DATA; i ++){
		uiSendEditID[i] = uiSendEditID[i-1] + 1; 
	}

	for(i = 0; i < NUM_OF_SEND_DATA; i ++){
		j = atoi(strInitSendValue[i]);
		if(j > 0 && j <= 255){
			SetDlgItemText(hWnd, uiSendEditID[i], strInitSendValue[i]);
			btValuesToSend[i] = j;
		} else {
			SetDlgItemText(hWnd, uiSendEditID[i], "0");
			btValuesToSend[i] = 0;
		}
	}

	// Initializing general purpose valiables
	uiGpEditID[0] = IDC_GP_EDIT0;
	for(i = 1; i < NUM_OF_GP_OUT + NUM_OF_GP_IN; i ++){
		uiGpEditID[i] = uiGpEditID[i-1] + 1;
	}

	ZeroMemory(dblGpOut, sizeof(DOUBLE) * NUM_OF_GP_OUT);
	for(i = 0; i < NUM_OF_GP_OUT; i ++){
		sprintf_s(strBuff, 32, "%.3f", dblGpOut[i]);
		SetDlgItemText(hWnd, uiGpEditID[i], strBuff);
	}

	for(i = 0; i < NUM_OF_GP_IN; i ++){
		dblGpIn[i] = atof(strInitGpValue[i]);
		SetDlgItemText(hWnd, uiGpEditID[NUM_OF_GP_OUT + i], strInitGpValue[i]);
	}

	// Initializing color of display
	for(i = 0; i < 3; i ++){
		for(j = 0; j < 8; j ++){
			if(InitColor[i][j] == '1'){
				btUseColor[i * 8 + j] = 1;
			}
			else if (InitColor[i][j] == '2') {
				btUseColor[i * 8 + j] = 2;
			}
			else {
				btUseColor[i * 8 + j] = 0;
			}
		}
	}

	// Initializing overlay
	for(i = 0; i < NUM_OF_DISPLAY; i++){
		pOverlayList[i] = NULL;
	}

	// Initializing replay rate
	_itoa_s(DEFAULT_REPLAY_RATE, strBuff, 32, 10);
	SetDlgItemText(hWnd, IDC_RATE_EDIT, strBuff);

	// Initializing display
	uiNumOfDisplay = NUM_OF_ROW_DISPLAY * NUM_OF_COL_DISPLAY;
	ShowDisplay(hWnd);
}

/******************************************************************************
/* Shutdown Procedure
 ******************************************************************************/
void OnClose(HWND hWnd)
{
	UnLoad(hWnd);
	Disconnect(hWnd);
	FinalizeDisplay(hDisplayWnd);
//	Sleep(200);
	CloseHandle(hDisplayThread);

	if(bWaveFormReady){
		CloseWaveForm();
	}

	EndDialog(hWnd, 0);
}

/******************************************************************************
/* Show Display Dialog
 ******************************************************************************/
DWORD WINAPI DisplayThreadProc(HWND hWnd)
{
	DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DISP), NULL, DisplayDlgProc);
	return 0;
}

void ShowDisplay(HWND hWnd)
{
	DWORD	DisplayThreadID;

	hDisplayThread = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)DisplayThreadProc, (LPVOID)hWnd, 0, &DisplayThreadID);

	if(hDisplayThread == NULL){
		MessageBox(hWnd, "Cannot create a thread", 
				"Thread Create Error", MB_OK | MB_ICONERROR);
	}
}


/******************************************************************************
/* Procedure for Connect
 ******************************************************************************/
void Connect(HWND hWnd)
{
	if(InitializeThread(hWnd)){
		SetDlgItemText(hWnd, IDC_CONNECT, "Disconnect");
		SetDlgItemText(hWnd, IDC_STATE, "Online ...");
	}
}

/******************************************************************************
/* Procedure for Disconnect
 ******************************************************************************/
void Disconnect(HWND hWnd)
{
	if(bConnecting){
		bConnecting = FALSE;
		if(bRecordEnable){
			bRecordEnable = FALSE;
			SetDlgItemText(hWnd, IDC_RECORD, "Record");
			delete(pRecord);
		}
		FinalizeThread();
		SetDlgItemText(hWnd, IDC_CONNECT, "Connect");
		SetDlgItemText(hWnd, IDC_STATE, "Command ?");
		PostMessage(hDisplayWnd, WM_MOVE, 0, 0);
	}
}


/******************************************************************************
/* Pushing the "Record" button excutes this procedure
 ******************************************************************************/
void RecordButtonDown(HWND hWnd)
{
	if(bRecordEnable){
		bRecordEnable = FALSE;
		SetDlgItemText(hWnd, IDC_RECORD, "Record");
		delete(pRecord);
	} else {
		if(bConnecting){
			pRecord = new CRecord(SaveDir, uiNumOfRecvImage + uiNumOfProcImage);
			SetDlgItemText(hWnd, IDC_RECORD, "Recording");
			bRecordEnable = TRUE;
		} else if(bReplaying){
			pRecord = new CRecord(SaveDir, uiNumOfReplaySource + uiNumOfReplayProc);
			SetDlgItemText(hWnd, IDC_RECORD, "Recording");
			bRecordEnable = TRUE;
		}
	}
}

/******************************************************************************
/* Pushing the "Send" button excutes this procedure
 ******************************************************************************/
void SendButtonDown(HWND hWnd)
{
	UINT	i;
	TCHAR	strBuff[8];

	for(i = 0; i < 14; i ++){
		GetDlgItemText(hWnd, uiSendEditID[i], strBuff, 8);
		btValuesToSend[i] = atoi(strBuff);
		if(btValuesToSend[i] > SEND_DATA_LIMIT)
			btValuesToSend[i] = SEND_DATA_LIMIT;
	}
	bSend = TRUE;
}

/******************************************************************************
/* Pushing the "Set" button excutes this procedure
 ******************************************************************************/
void GpSetButtonDown(HWND hWnd)
{
	UINT	i;
	TCHAR	strBuff[12];

	for(i = 0; i < NUM_OF_GP_IN; i ++){
		GetDlgItemText(hWnd, uiGpEditID[NUM_OF_GP_OUT + i], strBuff, 12);
		dblGpIn[i] = atof(strBuff);
	}
}

/******************************************************************************
/* Pushing the "Start WaveForm" button excutes this procedure
 ******************************************************************************/
void ShowWaveButtonDown(HWND hWnd)
{
	if(!bWaveFormReady){
		ShowWaveForm();
	}
}
