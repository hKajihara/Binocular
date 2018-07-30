/******************************************************************************
 * Pre-processor
 ******************************************************************************/
#include "ThreadProc.h"
#include "Global.h"
#include "resource.h"
#include "CyAPI.h"
#include "ConstantValues.h"
#include "Utils.h"
#include "ImageProc.h"
#include "WaveForm.h"

#pragma comment(lib,"CyAPI.lib")

#define	EVENT_DD			TEXT("DirectDraw")
#define	EVENT_PC			TEXT("Process")
#define NUM_OF_USB_RETRY	3

/******************************************************************************
 * Procedure Declaration
 ******************************************************************************/
DWORD WINAPI ComThreadProc(HWND);
DWORD WINAPI DrawThreadProc(HWND);
DWORD WINAPI ProcThreadProc(HWND);
VOID ReconstructImage(BYTE*, BYTE*, UINT);
VOID ExtractAttachedData(BYTE*, UINT*);
UINT SynchronizeUSB(HWND);

/******************************************************************************
 * Global Variables
 ******************************************************************************/
CCyUSBDevice	*pUSBDevice;
BYTE			ImageBuff[MAX_NUM_OF_RECV_IMG][NUM_OF_TEMP_BUF][IMAGE_DATA_LENGTH];
UINT			AppendDataBuff[NUM_OF_TEMP_BUF][NUM_OF_APPEND_DATA];
BYTE			SendBuff[NUM_OF_SEND_DATA];
UINT			uiSkipFrame, uiProcFrame;
UINT			uiFrameNumber, uiFrameNumberPast;
HANDLE			hComThread, hDrawThread, hProcThread;
HANDLE			hEventDraw, hEventProc;

/******************************************************************************
 * Initialize Thread Procedures
 ******************************************************************************/
BOOL InitializeThread(HWND hWnd)
{
	DWORD ComThreadID, DrawThreadID, ProcThreadID;
	LONG			len = USB_PACKET_LENGTH;						// EZUSBctrl ver.3
	unsigned char	ucInitBuf[USB_PACKET_LENGTH];					// EZUSBctrl ver.3

	// Initializing communication class
	pUSBDevice = new CCyUSBDevice(NULL);
	if(!pUSBDevice->IsOpen()){
		MessageBox(NULL, TEXT("Cannot initialize USB devise class"),
			TEXT("USB error"), MB_OK | MB_ICONSTOP);
		PostQuitMessage(0);
	} else {
		pUSBDevice->BulkInEndPt->TimeOut = 1000;
		pUSBDevice->BulkOutEndPt->TimeOut = 1000;
		ucInitBuf[0] = (unsigned char)255;							// EZUSBctrl ver.3
		if(!pUSBDevice->BulkOutEndPt->XferData(ucInitBuf, len)){	// EZUSBctrl ver.3
			MessageBox(NULL, TEXT("Cannot send USB packets"),		// EZUSBctrl ver.3
				TEXT("USB error"), MB_OK | MB_ICONSTOP);			// EZUSBctrl ver.3
		}															// EZUSBctrl ver.3
	}

	// Generating an ivent handler for drawing
	hEventDraw = CreateEvent(NULL , TRUE , FALSE , EVENT_DD);
	hEventDraw = OpenEvent(EVENT_ALL_ACCESS , FALSE , EVENT_DD);

	// Generating an ivent handler for image processing
	hEventProc = CreateEvent(NULL , TRUE , FALSE , EVENT_PC);
	hEventProc = OpenEvent(EVENT_ALL_ACCESS , FALSE , EVENT_PC);

	// Setting the number of 
	SetNumOfProcImage();

	// Setting overlay
	SetOverlay();

	// Initiating a thread for communication
	bConnecting = TRUE;
	hComThread = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)ComThreadProc, (LPVOID)hWnd, 0, &ComThreadID);
	if(hComThread == NULL){
		MessageBox(hWnd, "Cannot initiate a thread for communication", 
				"Thread error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Initiating a thread for drawing
	hDrawThread = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)DrawThreadProc, (LPVOID)hWnd, 0, &DrawThreadID);
	if(hDrawThread == NULL){
		MessageBox(hWnd, "Cannot initiate a thread for drawing", 
				"Thread error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Initiating a thread for image processing
	hProcThread = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)ProcThreadProc, (LPVOID)hWnd, 0, &ProcThreadID);
	if(hProcThread == NULL){
		MessageBox(hWnd, "Cannot initiate a thread for image processing", 
				"Thread error", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	ResetWaveFormBuffer();
	return TRUE;
}

/******************************************************************************
 * Finalize Thread Procedures
 ******************************************************************************/
VOID FinalizeThread()
{
	SetEvent(hEventDraw);
	CloseHandle(hDrawThread);
	SetEvent(hEventProc);
	CloseHandle(hProcThread);
	CloseHandle(hComThread);
	CloseHandle(hEventDraw);
	CloseHandle(hEventProc);

	Sleep(200);
	delete(pUSBDevice);
}

/******************************************************************************
 * Thread Procedure for UDP Communication
 ******************************************************************************/
DWORD WINAPI ComThreadProc(HWND hWnd)
{
	UINT			i, j;
	unsigned char	ucInBuf[USB_BUF_SIZE * MAX_NUM_OF_RECV_IMG], ucOutBuf[USB_PACKET_LENGTH];
	DWORD			dwStart, dwStop, dwInterval[5];
	TCHAR			strTime[20];
	LONG			length;

	dwStart = 0;
	uiSkipFrame = 0;

	ZeroMemory(SendBuff, sizeof(BYTE) * NUM_OF_SEND_DATA);
	for(i = 0; i < USB_PACKET_LENGTH; i ++){
		ucOutBuf[i] = i % 100;
	}

	// USBとの同期
	for(i = 0; i < NUM_OF_USB_RETRY; i ++){
		if(uiNumOfRecvImage = SynchronizeUSB(hWnd)){
			break;
		} else if(i == NUM_OF_USB_RETRY - 1){
			MessageBox(hWnd, "Cannot receive USB data", 
				"USB synchronizing error", MB_OK | MB_ICONERROR);
			uiNumOfRecvImage = 0;
		}
	}

	if(uiNumOfRecvImage > 0){
		// 受信画像、処理画像の枚数設定
		if(uiNumOfRecvImage > uiNumOfDisplay) {
			uiNumOfRecvImage = uiNumOfDisplay;
			uiNumOfProcImage = 0;
		} else if(uiNumOfRecvImage + uiNumOfProcImage > uiNumOfDisplay){
			uiNumOfProcImage = uiNumOfDisplay - uiNumOfRecvImage;
		}

		if(uiNumOfRecvImage < uiNumOfBlind){
			uiNumOfRecvImageAfterBlind = 0;
		} else {
			uiNumOfRecvImageAfterBlind = uiNumOfRecvImage - uiNumOfBlind;
		}

		while(true){
			if(!bConnecting){
				break;
			}

			// 画像受信
			length = (LONG)USB_BUF_SIZE * uiNumOfRecvImage;
			if(!pUSBDevice->BulkInEndPt->XferData(ucInBuf, length)){
				MessageBox(hWnd, "Cannot receive USB data", 
							"USB communication error", MB_OK | MB_ICONERROR);
				break;
			}

			//同期ズレ対応
			if(ucInBuf[0] != 0){
				SynchronizeUSB(hWnd);
			}

			// フレームレート計算
			uiFrameNumber = ucInBuf[2];
			dwStop = timeGetTime();
			dwInterval[uiFrameNumber % 4] = dwStop - dwStart;
			dwInterval[4] = (dwInterval[0] + dwInterval[1] + dwInterval[2] + dwInterval[3]) / 4;
			_itoa_s(dwInterval[4],strTime, 20, 10);
			lstrcat(strTime, " ms");
			SetDlgItemText(hWnd, IDC_EDIT1, strTime);
			dwStart = timeGetTime();

			// フレーム飛び検出
			if((INT)(uiFrameNumber - uiFrameNumberPast) > 1){
				uiSkipFrame ++;
			}
			uiFrameNumberPast = uiFrameNumber;

			// 受信画像の再構成
			for(i = 0; i < uiNumOfRecvImage; i++){
				for(j = 0; j < NUM_OF_PACKET; j ++){
					ReconstructImage(&ucInBuf[(i * NUM_OF_PACKET + j) * USB_PACKET_LENGTH], ImageBuff[i][uiFrameNumber % NUM_OF_TEMP_BUF], j);
				}
				ExtractAttachedData(&ucInBuf[(NUM_OF_PACKET - 1) * USB_PACKET_LENGTH], AppendDataBuff[uiFrameNumber % NUM_OF_TEMP_BUF]);
			}

			// 受信画像及び波形の描画
			if(uiFrameNumber % DISPLAY_RATE == 0){
				SetEvent(hEventDraw);
				if(bWaveFormReady){
					for(i = 0; i < uiNumOfRecvImageAfterBlind; i ++){
						BufferWaveForm(ImageBuff[i][uiFrameNumber % NUM_OF_TEMP_BUF], i);
					}
					DrawWaveForm();
				}
			}

			// 受信画像の録画
			if(bRecordEnable){
				for(i = 0; i < uiNumOfRecvImage; i ++){
					pRecord->RecordImage(i, ImageBuff[i][uiFrameNumber % NUM_OF_TEMP_BUF]);
				}
				if(uiNumOfProcImage == 0){
					pRecord->RecordAppendData(AppendDataBuff[uiProcFrame % NUM_OF_TEMP_BUF]);
				}
			}

			// 画像処理
			uiProcFrame = uiFrameNumber;
			if(uiNumOfProcImage > 0){
				SetEvent(hEventProc);
			}

			// FPGAへのデータ送信
			if(bSend){
				SetDlgItemText(hWnd, IDC_EDIT1, TEXT("Wait"));
				for(i = 0; i < NUM_OF_SEND_DATA; i ++){
					ucOutBuf[i] = btValuesToSend[i];
				}
				length = (LONG)USB_PACKET_LENGTH;
				if(pUSBDevice->BulkOutEndPt->XferData(ucOutBuf, length)){
					SetDlgItemText(hWnd, IDC_EDIT1, TEXT("Sent"));
				} else {
					SetDlgItemText(hWnd, IDC_EDIT1, TEXT("Not Sent"));
				}
				bSend = FALSE;
			}
		}
	} 

	return 0;
}


/******************************************************************************
 * Thread Procedure for DirectDraw
 ******************************************************************************/
DWORD WINAPI DrawThreadProc(HWND hWnd)
{
	UINT	i;

	while(TRUE){
		if(WaitForSingleObject(hEventDraw, INFINITE) == WAIT_OBJECT_0){
			ResetEvent(hEventDraw);
			if(!bConnecting){
				break;
			}
			for(i = 0; i < uiNumOfRecvImageAfterBlind; i ++){
				if(btUseColor[i]){
					pUDD->DrawColorImage(i, ImageBuff[i][uiFrameNumber % NUM_OF_TEMP_BUF]);
				} else {
					pUDD->DrawGrayImage(i, ImageBuff[i][uiFrameNumber % NUM_OF_TEMP_BUF]);
				}
			}
		}
	}
	return 0;
}

/******************************************************************************
 * Thread Procedure for Image Processing
 ******************************************************************************/
DWORD WINAPI ProcThreadProc(HWND hWnd)
{
	UINT	i, j, nFrameBuf[FRAMES_TO_PROC];
	BYTE*	pImage[MAX_NUM_OF_RECV_IMG * FRAMES_TO_PROC];

	while(TRUE){
		if(WaitForSingleObject(hEventProc, INFINITE) == WAIT_OBJECT_0)
		{
			ResetEvent(hEventProc);
			if(!bConnecting){
				break;
			}
			nFrameBuf[0] = uiProcFrame % NUM_OF_TEMP_BUF;
			for(i = 1; i < FRAMES_TO_PROC; i ++){
				if(nFrameBuf[0] < i){
					nFrameBuf[i] = NUM_OF_TEMP_BUF - i + nFrameBuf[0];
				} else {
					nFrameBuf[i] = nFrameBuf[0] - i;
				}
			}

			for(i = 0; i < MAX_NUM_OF_RECV_IMG; i ++){
				for(j = 0; j < FRAMES_TO_PROC; j ++){
					pImage[i * FRAMES_TO_PROC + j] = ImageBuff[i][nFrameBuf[j]];
				}
			}

			ImageProc(pImage, AppendDataBuff[uiProcFrame % NUM_OF_TEMP_BUF]);
			PostImageProc(uiNumOfRecvImage, uiNumOfRecvImageAfterBlind, 
							uiNumOfProcImage, bRecordEnable);

			if(bRecordEnable){
				pRecord->RecordAppendData(AppendDataBuff[uiProcFrame % NUM_OF_TEMP_BUF]);
			}
			
		}
	}
	return 0;
}

/******************************************************************************
 * Reconstruct Image from USB Packet
 ******************************************************************************/
VOID ReconstructImage(BYTE* RecvBuff, BYTE* ImageBuff, UINT uiNumOfPacket){
	UINT	uiDataLength;

	if(uiNumOfPacket == NUM_OF_PACKET - 1){
		uiDataLength = USB_DATA_LENGTH - ATTACHED_DATA_LENGTH;
	} else {
		uiDataLength = USB_DATA_LENGTH;
	}
	memcpy(&(ImageBuff[uiNumOfPacket * USB_DATA_LENGTH]), 
			&(RecvBuff[USB_HEADER_LENGTH]), uiDataLength);
}

/******************************************************************************
 * Extract Attached Data from UDP Packet
 ******************************************************************************/
VOID ExtractAttachedData(BYTE* RecvBuff, UINT* AppendDataBuff){
	UINT	i;

	for(i = 0; i < NUM_OF_ATTACHED_DATA; i++){
		AppendDataBuff[i]
				= RecvBuff[USB_PACKET_LENGTH - ATTACHED_DATA_LENGTH + i * 2]
				+ (RecvBuff[USB_PACKET_LENGTH - ATTACHED_DATA_LENGTH + i * 2+ 1] << 8);
	}
}

/******************************************************************************
 * Synchronize USB
 ******************************************************************************/
UINT SynchronizeUSB(HWND hWnd){
	LONG			len = USB_PACKET_LENGTH;
	INT				uiNumOfReceivedImage;
	unsigned char	ucTempBuf[USB_PACKET_LENGTH];

	for(;;){
		if(pUSBDevice->BulkInEndPt->XferData(ucTempBuf, len)){
			if(ucTempBuf[1] == ucTempBuf[3] - 1 && ucTempBuf[0] == NUM_OF_PACKET - 1){
				uiNumOfReceivedImage = ucTempBuf[3];
				break;
			}
		} else {
			uiNumOfReceivedImage = 0;
			break;
		}
	}
	return uiNumOfReceivedImage;

}
