#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h>
#include "ConstantValues.h"
#include "CUseDirectDraw.h"
#include "CRecord.h"

extern CUseDirectDraw*	pUDD;
extern CRecord*			pRecord;
extern HINSTANCE		hAppInstance;
extern HWND				hCtrlWnd;
extern HWND				hDisplayWnd;
extern UINT				uiNumOfDisplay;
extern UINT				uiNumOfRecvImage, uiNumOfProcImage;
extern UINT				uiNumOfBlind, uiNumOfRecvImageAfterBlind;
extern UINT				uiNumOfReplaySource, uiNumOfReplayProc;
extern BYTE				btValuesToSend[NUM_OF_SEND_DATA];
extern PBYTE			pOverlayList[NUM_OF_DISPLAY];
extern DOUBLE			dblGpOut[NUM_OF_GP_OUT];
extern DOUBLE			dblGpIn[NUM_OF_GP_IN];
extern UINT				uiGpEditID[NUM_OF_GP_OUT + NUM_OF_GP_IN];
extern ULONG			ulDstIp;
extern USHORT			usDstPort, usSrcPort;
extern BYTE				btUseColor[32];							// カラー描画時にTRUE
extern BOOL				bConnecting;							// 通信中にTRUE
extern BOOL				bReplaying;								// 録画データ再生中にTRUE
extern BOOL				bPlayWithProc;
extern BOOL				bRecordEnable;							// 録画機能利用時にTRUE
extern BOOL				bWaveFormReady;							// WaveFormウィンド表示時にTRUE
extern BOOL				bSend;									// FPGAへのデータ転送準備時にTRUE
extern TCHAR			SaveDir[SAVEDIR_NAME_BUF];

//1階層目変数宣言
extern BYTE	btEdgeEmphasisLeft[IMAGE_DATA_LENGTH], btEdgeEmphasisRight[IMAGE_DATA_LENGTH];
extern BYTE	btVerticalLeft_even[IMAGE_DATA_LENGTH], btVerticalRight_even[IMAGE_DATA_LENGTH];
extern BYTE	btVerticalLeft_odd[IMAGE_DATA_LENGTH], btVerticalRight_odd[IMAGE_DATA_LENGTH];

extern BYTE	btDiagonally45Left_even[IMAGE_DATA_LENGTH], btDiagonally45Right_even[IMAGE_DATA_LENGTH];
extern BYTE	btDiagonally45Left_odd[IMAGE_DATA_LENGTH], btDiagonally45Right_odd[IMAGE_DATA_LENGTH];

extern BYTE	btDiagonally135Left_even[IMAGE_DATA_LENGTH], btDiagonally135Right_even[IMAGE_DATA_LENGTH];
extern BYTE	btDiagonally135Left_odd[IMAGE_DATA_LENGTH], btDiagonally135Right_odd[IMAGE_DATA_LENGTH];

extern BYTE	btOutputDisparity[IMAGE_DATA_LENGTH], btOutputDiagonally45Disparity[IMAGE_DATA_LENGTH], btOutputDiagonally135Disparity[IMAGE_DATA_LENGTH];
extern BYTE    btOutPutParfectDisparity[IMAGE_DATA_LENGTH];

//2階層目変数宣言
extern BYTE	btEdgeEmphasisLeft2[IMAGE_DATA_LENGTH], btEdgeEmphasisRight2[IMAGE_DATA_LENGTH];
extern BYTE	btVerticalLeft_even2[IMAGE_DATA_LENGTH], btVerticalRight_even2[IMAGE_DATA_LENGTH];
extern BYTE	btVerticalLeft_odd2[IMAGE_DATA_LENGTH], btVerticalRight_odd2[IMAGE_DATA_LENGTH];

extern BYTE	btDiagonally45Left_even2[IMAGE_DATA_LENGTH], btDiagonally45Right_even2[IMAGE_DATA_LENGTH];
extern BYTE	btDiagonally45Left_odd2[IMAGE_DATA_LENGTH], btDiagonally45Right_odd2[IMAGE_DATA_LENGTH];

extern BYTE	btDiagonally135Left_even2[IMAGE_DATA_LENGTH], btDiagonally135Right_even2[IMAGE_DATA_LENGTH];
extern BYTE	btDiagonally135Left_odd2[IMAGE_DATA_LENGTH], btDiagonally135Right_odd2[IMAGE_DATA_LENGTH];

extern BYTE	btOutputDisparity2[IMAGE_DATA_LENGTH], btOutputDiagonally45Disparity2[IMAGE_DATA_LENGTH], btOutputDiagonally135Disparity2[IMAGE_DATA_LENGTH];
extern BYTE    btOutPutParfectDisparity2[IMAGE_DATA_LENGTH];

//3階層目変数宣言
extern BYTE	btEdgeEmphasisLeft3[IMAGE_DATA_LENGTH], btEdgeEmphasisRight3[IMAGE_DATA_LENGTH];
extern BYTE	btVerticalLeft_even3[IMAGE_DATA_LENGTH], btVerticalRight_even3[IMAGE_DATA_LENGTH];
extern BYTE	btVerticalLeft_odd3[IMAGE_DATA_LENGTH], btVerticalRight_odd3[IMAGE_DATA_LENGTH];

extern BYTE	btDiagonally45Left_even3[IMAGE_DATA_LENGTH], btDiagonally45Right_even3[IMAGE_DATA_LENGTH];
extern BYTE	btDiagonally45Left_odd3[IMAGE_DATA_LENGTH], btDiagonally45Right_odd3[IMAGE_DATA_LENGTH];

extern BYTE	btDiagonally135Left_even3[IMAGE_DATA_LENGTH], btDiagonally135Right_even3[IMAGE_DATA_LENGTH];
extern BYTE	btDiagonally135Left_odd3[IMAGE_DATA_LENGTH], btDiagonally135Right_odd3[IMAGE_DATA_LENGTH];

extern BYTE	btOutputDisparity3[IMAGE_DATA_LENGTH], btOutputDiagonally45Disparity3[IMAGE_DATA_LENGTH], btOutputDiagonally135Disparity3[IMAGE_DATA_LENGTH];
extern BYTE    btOutPutParfectDisparity3[IMAGE_DATA_LENGTH];

extern int				iL0[19200][17]; 
extern double			dblL[19200][17];
extern double			dblS[19200][17];
extern double			dblR[19200][17];
extern double			dblT[19200][17];

extern int				iL0Diagonally45[19200][17];
extern double			dblLDiagonally45[19200][17];
extern double			dblSDiagonally45[19200][17];
extern double			dblRDiagonally45[19200][17];
extern double			dblTDiagonally45[19200][17];

extern int				iL0Diagonally135[19200][17];
extern double			dblLDiagonally135[19200][17];
extern double			dblSDiagonally135[19200][17];
extern double			dblRDiagonally135[19200][17];
extern double			dblTDiagonally135[19200][17];

#endif