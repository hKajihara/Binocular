/******************************************************************************
 * Pre-processor
 ******************************************************************************/
#include "HandleRecData.h"
#include "CHandleRecData.h"
#include "SaveDialog.h"
#include "Utils.h"
#include "ImageProc.h"
#include "WaveForm.h"
#include "BitmapStream.h"
#include "Global.h"
#include "ConstantValues.h"
#include "resource.h"

#define TIMER_ID1 1
		
/******************************************************************************
 * Procedure Declaration
 ******************************************************************************/
VOID CALLBACK	TimerProc(HWND, UINT, UINT, DWORD);
VOID			StopTimer(HWND);

/******************************************************************************
 * Global Variables
 ******************************************************************************/
CHandleRecData*	pRecData		= NULL;
UINT			uiTimerRate		= DEFAULT_REPLAY_RATE;
BOOL			bTimerOperating	= FALSE;

/******************************************************************************
 * Procedure for Load
 ******************************************************************************/
VOID Load(HWND hWnd)
{
	UnLoad(hWnd);

	pRecData = new CHandleRecData(SaveDir);	
	if(!pRecData->IsAvailable()){
		delete(pRecData);
		pRecData = NULL;
	} else {
		bReplaying = TRUE;

		// Setting the number of images
		uiNumOfReplaySource = pRecData->GetNumOfLoaded();
		SetNumOfProcImage();

		// Setting overlay
		SetOverlay();

		if(uiNumOfReplaySource > uiNumOfDisplay) {
			uiNumOfReplaySource = uiNumOfDisplay;
			uiNumOfReplayProc = 0;
		} else if(uiNumOfReplaySource + uiNumOfProcImage > uiNumOfDisplay){
			uiNumOfReplayProc = uiNumOfDisplay - uiNumOfReplaySource;
		} else {
			uiNumOfReplayProc = uiNumOfProcImage;
		}

		ResetWaveFormBuffer();
		DrawOneFrame(hWnd);
		SetDlgItemText(hWnd, IDC_LOAD, "File Release");
		SetDlgItemText(hWnd, IDC_STATE, "Replaying ...");
	}
}

/******************************************************************************
 * Procedure for Unload
 ******************************************************************************/
VOID UnLoad(HWND hWnd)
{
	if(bReplaying){
		bReplaying = FALSE;
		StopTimer(hWnd);
		delete(pRecData);
		pRecData = NULL;
		uiNumOfReplaySource = 0;
		if(bWaveFormReady){
			ResetWaveFormBuffer();
			DrawWaveForm();
		}
		SetDlgItemText(hWnd, IDC_LOAD, "File Read");
		SetDlgItemText(hWnd, IDC_STATE, "Command ?");
		PostMessage(hDisplayWnd, WM_MOVE, 0, 0);
	}
}

/******************************************************************************
 * Checking the "WithProc" check box excutes this procedure
 ******************************************************************************/
VOID PlayWithProcCheck(HWND hWnd)
{
	if(bPlayWithProc){
		bPlayWithProc = FALSE;
	} else {
		bPlayWithProc = TRUE;
	}
}

/******************************************************************************
 * Pushing the "Play" button excutes this procedure
 ******************************************************************************/
VOID PlayButtonDown(HWND hWnd)
{
	TCHAR	strRate[8];

	if(bReplaying){
		if(bTimerOperating == FALSE){
			GetDlgItemText(hWnd, IDC_RATE_EDIT, strRate, 8);
			uiTimerRate = atoi(strRate);
			SetTimer(hWnd, TIMER_ID1, uiTimerRate, (TIMERPROC) TimerProc);
			bTimerOperating = TRUE;
		}
	} else {
		MessageBox(NULL, TEXT("No file is selected"),
			TEXT("Read Out Error"), MB_OK | MB_ICONSTOP);
	}
}

/******************************************************************************
 * Pushing the "Slow" button excutes this procedure
 ******************************************************************************/
VOID ForwardButtonDown(HWND hWnd)
{
	if(bReplaying){
		StopTimer(hWnd);
		pRecData->IncrementFrame();
		DrawOneFrame(hWnd);
	} else {
		MessageBox(NULL, TEXT("No file is selected"),
			TEXT("Read Out Error"), MB_OK | MB_ICONSTOP);
	}
}

/******************************************************************************
 * Pushing the "Back" button excutes this procedure
 ******************************************************************************/
VOID BackwardButtonDown(HWND hWnd)
{
	if(bReplaying){
		StopTimer(hWnd);
		pRecData->DecrementFrame();
		DrawOneFrame(hWnd);
	} else {
		MessageBox(NULL, TEXT("No file is selected"),
			TEXT("Read Out Error"), MB_OK | MB_ICONSTOP);
	}
}

/******************************************************************************
 * Pushing the "Stop" button excutes this procedure
 ******************************************************************************/
VOID StopButtonDown(HWND hWnd)
{
	if(bReplaying){
		StopTimer(hWnd);
	} else {
		MessageBox(NULL, TEXT("No file is selected"),
			TEXT("Read Out Error"), MB_OK | MB_ICONSTOP);
	}
}

/******************************************************************************
 * Pushing the "Rewind" button excutes this procedure
 ******************************************************************************/
VOID RwdButtonDown(HWND hWnd)
{
	if(bReplaying){
		StopTimer(hWnd);
		pRecData->SetPresentFrame(0);
		DrawOneFrame(hWnd);
	} else {
		MessageBox(NULL, TEXT("No file is selected"),
			TEXT("Read Out Error"), MB_OK | MB_ICONSTOP);
	}
}

/******************************************************************************
 * Draw Procedure
 ******************************************************************************/
VOID DrawOneFrame(HWND hWnd)
{
	UINT	i, j;
	UINT	uiFrameToProc, uiTempFrame;
	UINT	AppendDataBuff[NUM_OF_TEMP_BUF][NUM_OF_APPEND_DATA];
	BYTE*	pImage[MAX_NUM_OF_RECV_IMG * FRAMES_TO_PROC];
	TCHAR	strFrameNumber[32];

	if(bPlayWithProc){
		uiTempFrame = pRecData->GetPresentFrame();
		for(i = 0; i < uiNumOfReplaySource; i ++){
			for(j = 0; j < FRAMES_TO_PROC; j ++){
				if(uiTempFrame - j < 0){
					uiFrameToProc = 0;
				} else {
					uiFrameToProc = uiTempFrame - j;
				}
				pRecData->SetPresentFrame(uiFrameToProc);
				pImage[i * FRAMES_TO_PROC + j] = pRecData->GetImage(i);
			}
		}
		pRecData->SetPresentFrame(uiTempFrame);

		// Drawing images
		for(i = 0; i < uiNumOfReplaySource; i ++){
			if(btUseColor[i]){
				pUDD->DrawColorImage(i, pRecData->GetImage(i));
			} else {
				pUDD->DrawGrayImage(i, pRecData->GetImage(i));
			}
		}

		// Re-processing
		ImageProc(pImage, AppendDataBuff[0]);
		PostImageProc(uiNumOfReplaySource, uiNumOfReplaySource, 
						uiNumOfReplayProc, bRecordEnable);
	} else {
		for(i = 0; i < pRecData->GetNumOfLoaded(); i ++){
			if(btUseColor[i]){
				pUDD->DrawColorImage(i, pRecData->GetImage(i));
			} else {
				pUDD->DrawGrayImage(i, pRecData->GetImage(i));
			}
		}
	}

	// Drawing waveform
	if(bWaveFormReady){
		for(i = 0; i < pRecData->GetNumOfLoaded(); i ++){
			BufferWaveForm(pRecData->GetImage(i), i);
		}
		DrawWaveForm();
	}

	// Re-recording
	if(bRecordEnable){
		for(i = 0; i < uiNumOfReplaySource; i ++){
			pRecord->RecordImage(i, pRecData->GetImage(i));
		}
		pRecord->RecordAppendData(AppendDataBuff[0]);
	}

	_itoa_s(pRecData->GetPresentFrame(), strFrameNumber, 32, 10);
	lstrcat(strFrameNumber, " fr");
	SetDlgItemText(hWnd, IDC_EDIT1, strFrameNumber);
}

/******************************************************************************
 * Pushing the "Export" button excutes this procedure
 ******************************************************************************/
VOID ExportMenuClick()
{
	if(bReplaying){
		ShowSaveDialog(pRecData->GetEndFrame() + 1);
	} else {
		MessageBox(NULL, TEXT("No file to export"),
			TEXT("Read Out Error"), MB_OK | MB_ICONSTOP);
	}
}

/******************************************************************************
 * Export Bitmap File
 ******************************************************************************/
INT ExportBitmap(UINT uiInitFrameToSave, UINT uiEndFrameToSave)
{
	TCHAR	strFilePathTemp[SAVEDIR_NAME_BUF], strFilePath[SAVEDIR_NAME_BUF];
	TCHAR	strNumOfImage[8], strFrameNumber[8];
	UINT	uiNumOfFrameToSave, uiFrame, uiTempFrame;
	UINT	i, j;

	if(uiInitFrameToSave < 0 || uiInitFrameToSave > uiEndFrameToSave || uiInitFrameToSave > pRecData->GetEndFrame()){
		MessageBox(NULL, TEXT("Invalid input"),
			TEXT("Input Error"), MB_OK | MB_ICONSTOP);
		return -1;
	} else if(uiEndFrameToSave > pRecData->GetEndFrame()){
		uiEndFrameToSave = pRecData->GetEndFrame();
	}

	uiNumOfFrameToSave = uiEndFrameToSave - uiInitFrameToSave + 1;
	lstrcpy(strFilePathTemp, SaveDir);
	GetTimeString(strFilePathTemp);
	uiTempFrame = pRecData->GetPresentFrame();
	pRecData->SetPresentFrame(0);

	for(i = 0; i < pRecData->GetNumOfLoaded(); i ++){
		for(j = 0; j < uiNumOfFrameToSave; j ++){
			_itoa_s(i, strNumOfImage, 8, 10);
			lstrcpy(strFilePath, strFilePathTemp);
			lstrcat(strFilePath, "-");
			lstrcat(strFilePath, strNumOfImage);
			uiFrame = j + uiInitFrameToSave;
			_itoa_s(j, strFrameNumber, 8, 10);
			if(j > 99){
				lstrcat(strFilePath, "-");
			} else if(j > 9){
				lstrcat(strFilePath, "-0");
			} else {
				lstrcat(strFilePath, "-00");
			}
			lstrcat(strFilePath, strFrameNumber);
			lstrcat(strFilePath, ".bmp");
			pRecData->SetPresentFrame(uiFrame);
			if(bPlayWithProc){
				SaveBitmap(strFilePath, pRecData->GetImage(i), pOverlayList[i], btUseColor[i], 
				IMAGE_WIDTH, IMAGE_HEIGHT);
			} else {
				SaveBitmap(strFilePath, pRecData->GetImage(i), NULL, btUseColor[i], 
				IMAGE_WIDTH, IMAGE_HEIGHT);
			}
		}
	}
	pRecData->SetPresentFrame(uiTempFrame);
	return 1;
}

/******************************************************************************
 * Timer Procedure
 ******************************************************************************/
VOID CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTimer)
{
	if(pRecData->IncrementFrame()){
		DrawOneFrame(hWnd);
	}
}

/******************************************************************************
 * Stop Timer Procedure
 ******************************************************************************/
VOID StopTimer(HWND hWnd)
{
	if(bTimerOperating){
		KillTimer(hWnd, TIMER_ID1);
		bTimerOperating = FALSE;
	}
}
