#include "Global.h"

CUseDirectDraw*	pUDD			= NULL;
CRecord*		pRecord			= NULL;
HINSTANCE		hAppInstance;
HWND			hCtrlWnd;
HWND			hDisplayWnd;
UINT			uiNumOfDisplay;
UINT			uiNumOfRecvImage, uiNumOfProcImage;
UINT			uiNumOfBlind, uiNumOfRecvImageAfterBlind;
UINT			uiNumOfReplaySource, uiNumOfReplayProc;
BYTE			btValuesToSend[NUM_OF_SEND_DATA];
PBYTE			pOverlayList[NUM_OF_DISPLAY];
DOUBLE			dblGpOut[NUM_OF_GP_OUT];
DOUBLE			dblGpIn[NUM_OF_GP_IN];
UINT			uiGpEditID[NUM_OF_GP_OUT + NUM_OF_GP_IN];
ULONG			ulDstIp;
USHORT			usDstPort, usSrcPort;
BYTE			btUseColor[32];
BOOL			bConnecting		= FALSE;
BOOL			bReplaying		= FALSE;
BOOL			bPlayWithProc	= FALSE;
BOOL			bRecordEnable	= FALSE;
BOOL			bWaveFormReady	= FALSE;
BOOL			bSend			= FALSE;
TCHAR			SaveDir[SAVEDIR_NAME_BUF];

//1äKëwñ⁄ïœêîêÈåæ
BYTE	btEdgeEmphasisLeft[IMAGE_DATA_LENGTH], btEdgeEmphasisRight[IMAGE_DATA_LENGTH];
BYTE	btVerticalLeft_even[IMAGE_DATA_LENGTH], btVerticalRight_even[IMAGE_DATA_LENGTH];
BYTE	btVerticalLeft_odd[IMAGE_DATA_LENGTH], btVerticalRight_odd[IMAGE_DATA_LENGTH];

BYTE	btDiagonally45Left_even[IMAGE_DATA_LENGTH], btDiagonally45Right_even[IMAGE_DATA_LENGTH];
BYTE	btDiagonally45Left_odd[IMAGE_DATA_LENGTH], btDiagonally45Right_odd[IMAGE_DATA_LENGTH];

BYTE	btDiagonally135Left_even[IMAGE_DATA_LENGTH], btDiagonally135Right_even[IMAGE_DATA_LENGTH];
BYTE	btDiagonally135Left_odd[IMAGE_DATA_LENGTH], btDiagonally135Right_odd[IMAGE_DATA_LENGTH];

BYTE	btOutputDisparity[IMAGE_DATA_LENGTH], btOutputDiagonally45Disparity[IMAGE_DATA_LENGTH], btOutputDiagonally135Disparity[IMAGE_DATA_LENGTH];

//2äKëwñ⁄ïœêîêÈåæ
BYTE	btEdgeEmphasisLeft2[IMAGE_DATA_LENGTH], btEdgeEmphasisRight2[IMAGE_DATA_LENGTH];
BYTE	btVerticalLeft_even2[IMAGE_DATA_LENGTH], btVerticalRight_even2[IMAGE_DATA_LENGTH];
BYTE	btVerticalLeft_odd2[IMAGE_DATA_LENGTH], btVerticalRight_odd2[IMAGE_DATA_LENGTH];

BYTE	btDiagonally45Left_even2[IMAGE_DATA_LENGTH], btDiagonally45Right_even2[IMAGE_DATA_LENGTH];
BYTE	btDiagonally45Left_odd2[IMAGE_DATA_LENGTH], btDiagonally45Right_odd2[IMAGE_DATA_LENGTH];

BYTE	btDiagonally135Left_even2[IMAGE_DATA_LENGTH], btDiagonally135Right_even2[IMAGE_DATA_LENGTH];
BYTE	btDiagonally135Left_odd2[IMAGE_DATA_LENGTH], btDiagonally135Right_odd2[IMAGE_DATA_LENGTH];

BYTE	btOutputDisparity2[IMAGE_DATA_LENGTH], btOutputDiagonally45Disparity2[IMAGE_DATA_LENGTH], btOutputDiagonally135Disparity2[IMAGE_DATA_LENGTH];

//3äKëwñ⁄ïœêîêÈåæ
BYTE	btEdgeEmphasisLeft3[IMAGE_DATA_LENGTH], btEdgeEmphasisRight3[IMAGE_DATA_LENGTH];
BYTE	btVerticalLeft_even3[IMAGE_DATA_LENGTH], btVerticalRight_even3[IMAGE_DATA_LENGTH];
BYTE	btVerticalLeft_odd3[IMAGE_DATA_LENGTH], btVerticalRight_odd3[IMAGE_DATA_LENGTH];

BYTE	btDiagonally45Left_even3[IMAGE_DATA_LENGTH], btDiagonally45Right_even3[IMAGE_DATA_LENGTH];
BYTE	btDiagonally45Left_odd3[IMAGE_DATA_LENGTH], btDiagonally45Right_odd3[IMAGE_DATA_LENGTH];

BYTE	btDiagonally135Left_even3[IMAGE_DATA_LENGTH], btDiagonally135Right_even3[IMAGE_DATA_LENGTH];
BYTE	btDiagonally135Left_odd3[IMAGE_DATA_LENGTH], btDiagonally135Right_odd3[IMAGE_DATA_LENGTH];

BYTE	btOutputDisparity3[IMAGE_DATA_LENGTH], btOutputDiagonally45Disparity3[IMAGE_DATA_LENGTH], btOutputDiagonally135Disparity3[IMAGE_DATA_LENGTH];

//others
BYTE    btOutPutParfectDisparity[IMAGE_DATA_LENGTH];

//For Cooperative
int				iL0[19200][17]; 
double			dblL[19200][17];
double			dblS[19200][17];
double			dblR[19200][17];
double			dblT[19200][17];

int				iL0Diagonally45[19200][17];
double			dblLDiagonally45[19200][17];
double			dblSDiagonally45[19200][17];
double			dblRDiagonally45[19200][17];
double			dblTDiagonally45[19200][17];

int				iL0Diagonally135[19200][17];
double			dblLDiagonally135[19200][17];
double			dblSDiagonally135[19200][17];
double			dblRDiagonally135[19200][17];
double			dblTDiagonally135[19200][17];