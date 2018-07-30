/******************************************************************************
 * Pre-processor
 ******************************************************************************/
#include "ImageProc.h"
#include "ConstantValues.h"
#include "Global.h"
#include <math.h>
#include <stdio.h>

#include"ClassicDisparity.h"
#include"MakeBandPassFilter.h"
#include"GetBinoEnergy.h"

/******************************************************************************
 * Procedure Declaration
 ******************************************************************************/
UINT ComputeAverage(BYTE* pImage);
VOID MyImageProc(BYTE** ppImage, UINT* pAppendData);
VOID ColorCheck(BYTE** ppImage, UINT* pAppendData);

/******************************************************************************
 * Global Variables
 ******************************************************************************/
BYTE  computedImage[NUM_OF_PROCESS_BUF][IMAGE_DATA_LENGTH];
UINT  uiCounter;
INT	  iComputedImage[NUM_OF_PROCESS_BUF][IMAGE_DATA_LENGTH];

UINT  uiPresentFr = 0;
BYTE    btOutPutParfectDisparityM[IMAGE_DATA_LENGTH];

/******************************************************************************
 * Image Processing
 ******************************************************************************/
VOID ImageProc(BYTE** ppImage, UINT* pAppendData)
{
	MyImageProc(ppImage, pAppendData);
//	ColorCheck(ppImage, pAppendData);
}

/******************************************************************************
 * Set Number of Process Image
 ******************************************************************************/
VOID SetNumOfProcImage()
{
	uiNumOfProcImage = 40;
}

/******************************************************************************
 * Set Overlay
 ******************************************************************************/
VOID SetOverlay()
{
//	pOverlayList[0] = computedImage[0];
}

/******************************************************************************
 * Post Procedure
 ******************************************************************************/
VOID PostImageProc(UINT uiNumOfSrcImage, UINT uiNumOfSrcImageAfterBlind,
				   UINT uiNumOfResultImage, BOOL bRecEnable)
{
	UINT i;
	TCHAR strGpOut[16];

	// Drawing processed images
	for(i = 0; i < uiNumOfResultImage; i ++){
		if(btUseColor[uiNumOfSrcImageAfterBlind + i] == 2){
			pUDD->DrawFullColorImage(uiNumOfSrcImageAfterBlind + i, iComputedImage[i]);
		} else if(btUseColor[uiNumOfSrcImageAfterBlind + i] == 1){
			pUDD->DrawColorImage(uiNumOfSrcImageAfterBlind + i, computedImage[i]);
		} else {
			pUDD->DrawGrayImage(uiNumOfSrcImageAfterBlind + i, computedImage[i]);
		}
	}

	// Drawing overlay images
	for(i = 0; i < uiNumOfDisplay; i ++){
		if(pOverlayList[i] != NULL){
			pUDD->DrawOverlayImage(i, pOverlayList[i]);
		}
	}

	// Updating general purpose valiables
	for(i = 0; i < NUM_OF_GP_OUT; i ++){
		sprintf_s(strGpOut, 16, "%.3f", dblGpOut[i]);
		SetDlgItemText(hCtrlWnd, uiGpEditID[i], strGpOut);
	}

	// Recording processed images
	if(bRecEnable){
		for(i = 0; i < uiNumOfResultImage; i ++){
			pRecord->RecordImage(uiNumOfSrcImage + i, computedImage[i]);
		}
	}
}

/******************************************************************************
 * Template Procedure for Image Processing 
 ******************************************************************************/
VOID MyImageProc(BYTE** ppImage, UINT* pAppendData)
{
	UINT	i ,j, k;
	INT		l;
	BYTE*	pImage[MAX_NUM_OF_RECV_IMG][FRAMES_TO_PROC];

	for(i = 0; i < MAX_NUM_OF_RECV_IMG; i ++){
		for(j = 0; j < FRAMES_TO_PROC; j ++ ){
			pImage[i][j] = *ppImage;
			ppImage ++;
		}
	}

	/* ------------------------------------------------------------------
	/	pImage[i][j]	: Image of image number [i], [j] frames in the past.
	/  ------------------------------------------------------------------ */

	// Write your program here ------------------------------------------------

	//----------ここから-------------
	DWORD	dwStart, dwStop, dwInterval;
	dwStart = timeGetTime();	//時間計測
	ZeroMemory(computedImage[1], IMAGE_DATA_LENGTH * sizeof(UINT));

	INT  thresh2 = dblGpIn[2], thresh3 = dblGpIn[3], thresh4 = dblGpIn[4], thresh5 = dblGpIn[5], thresh6 = dblGpIn[6];

	INT	y, x;
	INT	iRight_SearchRange;

	INT iMax, iMax_index;
	DOUBLE dblMax;
	

	UINT	uiCount;

	////1階層目変数宣言
	//BYTE	btEdgeEmphasisLeft[IMAGE_DATA_LENGTH], btEdgeEmphasisRight[IMAGE_DATA_LENGTH];
	//BYTE	btVerticalLeft_even[IMAGE_DATA_LENGTH], btVerticalRight_even[IMAGE_DATA_LENGTH];
	//BYTE	btVerticalLeft_odd[IMAGE_DATA_LENGTH], btVerticalRight_odd[IMAGE_DATA_LENGTH];

	//BYTE	btDiagonally45Left_even[IMAGE_DATA_LENGTH], btDiagonally45Right_even[IMAGE_DATA_LENGTH];
	//BYTE	btDiagonally45Left_odd[IMAGE_DATA_LENGTH], btDiagonally45Right_odd[IMAGE_DATA_LENGTH];

	//BYTE	btDiagonally135Left_even[IMAGE_DATA_LENGTH], btDiagonally135Right_even[IMAGE_DATA_LENGTH];
	//BYTE	btDiagonally135Left_odd[IMAGE_DATA_LENGTH], btDiagonally135Right_odd[IMAGE_DATA_LENGTH];

	//BYTE	btOutputDisparity[IMAGE_DATA_LENGTH], btOutputDiagonally45Disparity[IMAGE_DATA_LENGTH], btOutputDiagonally135Disparity[IMAGE_DATA_LENGTH];
	//BYTE    btOutPutParfectDisparity[IMAGE_DATA_LENGTH];

	INT	iNumOfVertical = 7;		//足しこむ範囲
	INT iDistanceAway = 1;		//離す距離

#define NUM_OF_PIXEL 17			//検出範囲
	
	INT iDisparityEnergy[NUM_OF_PIXEL];
	INT iDisparityEnergyOfDiagonally45[NUM_OF_PIXEL];
	INT iDisparityEnergyOfDiagonally135[NUM_OF_PIXEL];

	INT iSumOfEven[NUM_OF_PIXEL], iSumPowOfEven[NUM_OF_PIXEL];
	INT iSumOfOdd[NUM_OF_PIXEL], iSumPowOfOdd[NUM_OF_PIXEL];
	

	
	////2階層目変数宣言
	//BYTE	btEdgeEmphasisLeft2[IMAGE_DATA_LENGTH], btEdgeEmphasisRight2[IMAGE_DATA_LENGTH];
	//BYTE	btVerticalLeft_even2[IMAGE_DATA_LENGTH], btVerticalRight_even2[IMAGE_DATA_LENGTH];
	//BYTE	btVerticalLeft_odd2[IMAGE_DATA_LENGTH], btVerticalRight_odd2[IMAGE_DATA_LENGTH];

	//BYTE	btDiagonally45Left_even2[IMAGE_DATA_LENGTH], btDiagonally45Right_even2[IMAGE_DATA_LENGTH];
	//BYTE	btDiagonally45Left_odd2[IMAGE_DATA_LENGTH], btDiagonally45Right_odd2[IMAGE_DATA_LENGTH];

	//BYTE	btDiagonally135Left_even2[IMAGE_DATA_LENGTH], btDiagonally135Right_even2[IMAGE_DATA_LENGTH];
	//BYTE	btDiagonally135Left_odd2[IMAGE_DATA_LENGTH], btDiagonally135Right_odd2[IMAGE_DATA_LENGTH];

	//BYTE	btOutputDisparity2[IMAGE_DATA_LENGTH], btOutputDiagonally45Disparity2[IMAGE_DATA_LENGTH], btOutputDiagonally135Disparity2[IMAGE_DATA_LENGTH];
	//BYTE    btOutPutParfectDisparity2[IMAGE_DATA_LENGTH];

	INT	iNumOfVertical2 = 17;	//足しこむ範囲
	INT iDistanceAway2 = 2;		//離す距離

#define NUM_OF_PIXEL2 17		//検出範囲

	INT iDisparityEnergy2[NUM_OF_PIXEL2];
	INT iDisparityEnergyOfDiagonally45_2[NUM_OF_PIXEL];
	INT iDisparityEnergyOfDiagonally135_2[NUM_OF_PIXEL];

	INT iSumOfEven2[NUM_OF_PIXEL2], iSumPowOfEven2[NUM_OF_PIXEL2];
	INT iSumOfOdd2[NUM_OF_PIXEL2], iSumPowOfOdd2[NUM_OF_PIXEL2];

	////3階層目変数宣言
	//BYTE	btEdgeEmphasisLeft3[IMAGE_DATA_LENGTH], btEdgeEmphasisRight3[IMAGE_DATA_LENGTH];
	//BYTE	btVerticalLeft_even3[IMAGE_DATA_LENGTH], btVerticalRight_even3[IMAGE_DATA_LENGTH];
	//BYTE	btVerticalLeft_odd3[IMAGE_DATA_LENGTH], btVerticalRight_odd3[IMAGE_DATA_LENGTH];

	//BYTE	btDiagonally45Left_even3[IMAGE_DATA_LENGTH], btDiagonally45Right_even3[IMAGE_DATA_LENGTH];
	//BYTE	btDiagonally45Left_odd3[IMAGE_DATA_LENGTH], btDiagonally45Right_odd3[IMAGE_DATA_LENGTH];

	//BYTE	btDiagonally135Left_even3[IMAGE_DATA_LENGTH], btDiagonally135Right_even3[IMAGE_DATA_LENGTH];
	//BYTE	btDiagonally135Left_odd3[IMAGE_DATA_LENGTH], btDiagonally135Right_odd3[IMAGE_DATA_LENGTH];

	//BYTE	btOutputDisparity3[IMAGE_DATA_LENGTH], btOutputDiagonally45Disparity3[IMAGE_DATA_LENGTH], btOutputDiagonally135Disparity3[IMAGE_DATA_LENGTH];
	//BYTE    btOutPutParfectDisparity3[IMAGE_DATA_LENGTH];

	INT	iNumOfVertical3 = 33;	//足しこむ範囲
	INT iDistanceAway3 = 4;		//離す距離

#define NUM_OF_PIXEL3 17		//検出範囲

	INT iDisparityEnergy3[NUM_OF_PIXEL3];
	INT iDisparityEnergyOfDiagonally45_3[NUM_OF_PIXEL];
	INT iDisparityEnergyOfDiagonally135_3[NUM_OF_PIXEL];

	INT iSumOfEven3[NUM_OF_PIXEL3], iSumPowOfEven3[NUM_OF_PIXEL3];
	INT iSumOfOdd3[NUM_OF_PIXEL3], iSumPowOfOdd3[NUM_OF_PIXEL3];

	DOUBLE k1 = 0.5, k2 = 0.25;
	INT iMargeDisparityEnergy[NUM_OF_PIXEL];
	INT iMargeDisparityEnergyOfDiagonally45[NUM_OF_PIXEL];
	INT iMargeDisparityEnergyOfDiagonally135[NUM_OF_PIXEL];
	BYTE	btOutputDisparityM[IMAGE_DATA_LENGTH];
	BYTE	btOutputDisparityOfDiagonally45M[IMAGE_DATA_LENGTH];
	BYTE	btOutputDisparityOfDiagonally135M[IMAGE_DATA_LENGTH];

	//cooperativeAlgorithm----------------------------------
	INT		iColum, iDisparity, iRow, iColum2, iDisparity2, iRow2;
	DOUBLE	dblSumOfWithinPSI, dblSumOfWithinPSIDiagonally45, dblSumOfWithinPSIDiagonally135;
	INT		PHI = 3, PSI = 17;
	DOUBLE	ALPHA = dblGpIn[0];


	//初期化
	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
		btOutputDisparity[i] = 0;
	}

	//左右輪郭強調---------------------------------------
	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
		btEdgeEmphasisLeft[i] = max(min(pImage[0][0][i] - pImage[2][0][i] + 128, 255), 0);
		btEdgeEmphasisRight[i] = max(min(pImage[1][0][i] - pImage[3][0][i] + 128, 255), 0);
	}

	//左右縦強調(cos)----------------------------------------
	MakeBandPassFilter(btEdgeEmphasisLeft, btVerticalLeft_even, iNumOfVertical);
	MakeBandPassFilter(btEdgeEmphasisRight, btVerticalRight_even, iNumOfVertical);
	
	//左右縦強調(sin)----------------------------------------
	MakeVerticalOdd(btVerticalLeft_even, btVerticalLeft_odd, iDistanceAway);
	MakeVerticalOdd(btVerticalRight_even, btVerticalRight_odd, iDistanceAway);
	
	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	for (x = iDistanceAway; x < IMAGE_WIDTH - iDistanceAway; x++) {
	//		//左眼縦要素強調
	//		btVerticalLeft_odd[x + IMAGE_WIDTH*y] = max(min(btVerticalLeft_even[(x + IMAGE_WIDTH*y) - iDistanceAway] - btVerticalLeft_even[(x + IMAGE_WIDTH*y) + iDistanceAway] + 128, 255), 0);
	//		//右眼縦要素強調
	//		btVerticalRight_odd[x + IMAGE_WIDTH*y] = max(min(btVerticalRight_even[(x + IMAGE_WIDTH*y) - iDistanceAway] - btVerticalRight_even[(x + IMAGE_WIDTH*y) + iDistanceAway] + 128, 255), 0);
	//	}
	//}

	//左右ななめ45強調(cos)----------------------------------------
	MakeDiagonallyEdgeEmphasis45(btEdgeEmphasisLeft, btDiagonally45Left_even, iNumOfVertical);
	MakeDiagonallyEdgeEmphasis45(btEdgeEmphasisRight, btDiagonally45Right_even, iNumOfVertical);

	//左右ななめ45強調(sin)----------------------------------------
	MakeDiagonallyOdd45(btDiagonally45Left_even, btDiagonally45Left_odd, iDistanceAway);
	MakeDiagonallyOdd45(btDiagonally45Right_even, btDiagonally45Right_odd, iDistanceAway);
	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	for (x = iDistanceAway; x < IMAGE_WIDTH - iDistanceAway; x++) {
	//		//左眼ななめ45要素強調
	//		btDiagonally45Left_odd[x + IMAGE_WIDTH*y] = max(min(btDiagonally45Left_even[x + (IMAGE_WIDTH*(y - iDistanceAway)) - iDistanceAway] - btDiagonally45Left_even[x + (IMAGE_WIDTH*(y + iDistanceAway)) + iDistanceAway] + 128, 255), 0);
	//		//右眼ななめ45要素強調
	//		btDiagonally45Right_odd[x + IMAGE_WIDTH*y] = max(min(btDiagonally45Right_even[x + (IMAGE_WIDTH*(y - iDistanceAway)) - iDistanceAway] - btDiagonally45Right_even[x + (IMAGE_WIDTH*(y - iDistanceAway)) + iDistanceAway] + 128, 255), 0);
	//	}
	//}

	//左右ななめ135強調(cos)----------------------------------------
	MakeDiagonallyEdgeEmphasis135(btEdgeEmphasisLeft, btDiagonally135Left_even, iNumOfVertical);
	MakeDiagonallyEdgeEmphasis135(btEdgeEmphasisRight, btDiagonally135Right_even, iNumOfVertical);

	//左右ななめ135強調(sin)----------------------------------------
	MakeDiagonallyOdd135(btDiagonally135Left_even, btDiagonally135Left_odd, iDistanceAway);
	MakeDiagonallyOdd135(btDiagonally135Right_even, btDiagonally135Right_odd, iDistanceAway);
	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	for (x = iDistanceAway; x < IMAGE_WIDTH - iDistanceAway; x++) {
	//		//左眼ななめ135要素強調
	//		btDiagonally135Left_odd[x + IMAGE_WIDTH*y] = max(min(btDiagonally135Left_even[x + (IMAGE_WIDTH*(y + iDistanceAway)) - iDistanceAway] - btDiagonally135Left_even[x + (IMAGE_WIDTH*(y - iDistanceAway)) + iDistanceAway] + 128, 255), 0);
	//		//右眼ななめ45要素強調
	//		btDiagonally135Right_odd[x + IMAGE_WIDTH*y] = max(min(btDiagonally135Right_even[x + (IMAGE_WIDTH*(y + iDistanceAway)) - iDistanceAway] - btDiagonally135Right_even[x + (IMAGE_WIDTH*(y - iDistanceAway)) + iDistanceAway] + 128, 255), 0);
	//	}
	//}

	//2階層目作成-------------------------------------------

	//初期化
	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
		btOutputDisparity2[i] = 0;
	}

	//左右輪郭強調---------------------------------------
	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
		btEdgeEmphasisLeft2[i] = max(min(pImage[2][0][i] - pImage[4][0][i] + 128, 255), 0);
		btEdgeEmphasisRight2[i] = max(min(pImage[3][0][i] - pImage[5][0][i] + 128, 255), 0);
	}

	//左右縦強調(cos)----------------------------------------
	MakeBandPassFilter(btEdgeEmphasisLeft2, btVerticalLeft_even2, iNumOfVertical2);
	MakeBandPassFilter(btEdgeEmphasisRight2, btVerticalRight_even2, iNumOfVertical2);

	//左右縦強調(sin)----------------------------------------
	MakeVerticalOdd(btVerticalLeft_even2, btVerticalLeft_odd2, iDistanceAway2);
	MakeVerticalOdd(btVerticalRight_even2, btVerticalRight_odd2, iDistanceAway2);
	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	for (x = iDistanceAway2; x < IMAGE_WIDTH - iDistanceAway2; x++) {
	//		//左眼縦要素強調
	//		btVerticalLeft_odd2[x + IMAGE_WIDTH*y] = max(min(btVerticalLeft_even2[(x + IMAGE_WIDTH*y) - iDistanceAway2] - btVerticalLeft_even2[(x + IMAGE_WIDTH*y) + iDistanceAway2] + 128, 255), 0);
	//		//右眼縦要素強調
	//		btVerticalRight_odd2[x + IMAGE_WIDTH*y] = max(min(btVerticalRight_even2[(x + IMAGE_WIDTH*y) - iDistanceAway2] - btVerticalRight_even2[(x + IMAGE_WIDTH*y) + iDistanceAway2] + 128, 255), 0);
	//	}
	//}

	//左右ななめ45強調(cos)----------------------------------------
	MakeDiagonallyEdgeEmphasis45(btEdgeEmphasisLeft2, btDiagonally45Left_even2, iNumOfVertical2);
	MakeDiagonallyEdgeEmphasis45(btEdgeEmphasisRight2, btDiagonally45Right_even2, iNumOfVertical2);

	//左右ななめ45強調(sin)----------------------------------------
	MakeDiagonallyOdd45(btDiagonally45Left_even2, btDiagonally45Left_odd2, iDistanceAway2);
	MakeDiagonallyOdd45(btDiagonally45Right_even2, btDiagonally45Right_odd2, iDistanceAway2);
	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	for (x = iDistanceAway2; x < IMAGE_WIDTH - iDistanceAway2; x++) {
	//		//左眼ななめ45要素強調
	//		btDiagonally45Left_odd2[x + IMAGE_WIDTH*y] = max(min(btDiagonally45Left_even2[x + (IMAGE_WIDTH*(y - iDistanceAway2)) - iDistanceAway2] - btDiagonally45Left_even2[x + (IMAGE_WIDTH*(y + iDistanceAway2)) + iDistanceAway2] + 128, 255), 0);
	//		//右眼ななめ45要素強調
	//		btDiagonally45Right_odd2[x + IMAGE_WIDTH*y] = max(min(btDiagonally45Right_even2[x + (IMAGE_WIDTH*(y - iDistanceAway2)) - iDistanceAway2] - btDiagonally45Right_even2[x + (IMAGE_WIDTH*(y - iDistanceAway2)) + iDistanceAway2] + 128, 255), 0);
	//	}
	//}

	//左右ななめ135強調(cos)----------------------------------------
	MakeDiagonallyEdgeEmphasis135(btEdgeEmphasisLeft2, btDiagonally135Left_even2, iNumOfVertical2);
	MakeDiagonallyEdgeEmphasis135(btEdgeEmphasisRight2, btDiagonally135Right_even2, iNumOfVertical2);

	//左右ななめ135強調(sin)----------------------------------------
	MakeDiagonallyOdd135(btDiagonally135Left_even2, btDiagonally135Left_odd2, iDistanceAway2);
	MakeDiagonallyOdd135(btDiagonally135Right_even2, btDiagonally135Right_odd2, iDistanceAway2);
	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	for (x = iDistanceAway2; x < IMAGE_WIDTH - iDistanceAway2; x++) {
	//		//左眼ななめ135要素強調
	//		btDiagonally135Left_odd2[x + IMAGE_WIDTH*y] = max(min(btDiagonally135Left_even2[x + (IMAGE_WIDTH*(y + iDistanceAway2)) - iDistanceAway2] - btDiagonally135Left_even2[x + (IMAGE_WIDTH*(y - iDistanceAway2)) + iDistanceAway2] + 128, 255), 0);
	//		//右眼ななめ45要素強調
	//		btDiagonally135Right_odd2[x + IMAGE_WIDTH*y] = max(min(btDiagonally135Right_even2[x + (IMAGE_WIDTH*(y + iDistanceAway2)) - iDistanceAway2] - btDiagonally135Right_even2[x + (IMAGE_WIDTH*(y - iDistanceAway2)) + iDistanceAway2] + 128, 255), 0);
	//	}
	//}

	//3階層目作成-------------------------------------------

	//初期化
	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
		btOutputDisparity3[i] = 0;
	}

	//左右輪郭強調---------------------------------------
	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
		btEdgeEmphasisLeft3[i] = max(min(pImage[4][0][i] - pImage[6][0][i] + 128, 255), 0);
		btEdgeEmphasisRight3[i] = max(min(pImage[5][0][i] - pImage[7][0][i] + 128, 255), 0);
	}

	//左右縦強調(cos)----------------------------------------
	MakeBandPassFilter(btEdgeEmphasisLeft3, btVerticalLeft_even3, iNumOfVertical3);
	MakeBandPassFilter(btEdgeEmphasisRight3, btVerticalRight_even3, iNumOfVertical3);

	//左右縦強調(sin)----------------------------------------
	MakeVerticalOdd(btVerticalLeft_even3, btVerticalLeft_odd3, iDistanceAway3);
	MakeVerticalOdd(btVerticalRight_even3, btVerticalRight_odd3, iDistanceAway3);
	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	for (x = iDistanceAway3; x < IMAGE_WIDTH - iDistanceAway3; x++) {
	//		//左眼縦要素強調
	//		btVerticalLeft_odd3[x + IMAGE_WIDTH*y] = max(min(btVerticalLeft_even3[(x + IMAGE_WIDTH*y) - iDistanceAway3] - btVerticalLeft_even3[(x + IMAGE_WIDTH*y) + iDistanceAway3] + 128, 255), 0);
	//		//右眼縦要素強調
	//		btVerticalRight_odd3[x + IMAGE_WIDTH*y] = max(min(btVerticalRight_even3[(x + IMAGE_WIDTH*y) - iDistanceAway3] - btVerticalRight_even3[(x + IMAGE_WIDTH*y) + iDistanceAway3] + 128, 255), 0);
	//	}
	//}

	//左右ななめ45強調(cos)----------------------------------------
	MakeDiagonallyEdgeEmphasis45(btEdgeEmphasisLeft3, btDiagonally45Left_even3, iNumOfVertical3);
	MakeDiagonallyEdgeEmphasis45(btEdgeEmphasisRight3, btDiagonally45Right_even3, iNumOfVertical3);

	//左右ななめ45強調(sin)----------------------------------------
	MakeDiagonallyOdd45(btDiagonally45Left_even3, btDiagonally45Left_odd3, iDistanceAway3);
	MakeDiagonallyOdd45(btDiagonally45Right_even3, btDiagonally45Right_odd3, iDistanceAway3);
	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	for (x = iDistanceAway3; x < IMAGE_WIDTH - iDistanceAway3; x++) {
	//		//左眼ななめ45要素強調
	//		btDiagonally45Left_odd3[x + IMAGE_WIDTH*y] = max(min(btDiagonally45Left_even3[x + (IMAGE_WIDTH*(y - iDistanceAway3)) - iDistanceAway3] - btDiagonally45Left_even3[x + (IMAGE_WIDTH*(y + iDistanceAway3)) + iDistanceAway3] + 128, 255), 0);
	//		//右眼ななめ45要素強調
	//		btDiagonally45Right_odd3[x + IMAGE_WIDTH*y] = max(min(btDiagonally45Right_even3[x + (IMAGE_WIDTH*(y - iDistanceAway3)) - iDistanceAway3] - btDiagonally45Right_even3[x + (IMAGE_WIDTH*(y - iDistanceAway3)) + iDistanceAway3] + 128, 255), 0);
	//	}
	//}

	//左右ななめ135強調(cos)----------------------------------------
	MakeDiagonallyEdgeEmphasis135(btEdgeEmphasisLeft3, btDiagonally135Left_even3, iNumOfVertical3);
	MakeDiagonallyEdgeEmphasis135(btEdgeEmphasisRight3, btDiagonally135Right_even3, iNumOfVertical3);

	//左右ななめ135強調(sin)----------------------------------------
	MakeDiagonallyOdd135(btDiagonally135Left_even3, btDiagonally135Left_odd3, iDistanceAway3);
	MakeDiagonallyOdd135(btDiagonally135Right_even3, btDiagonally135Right_odd3, iDistanceAway3);
	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	for (x = iDistanceAway3; x < IMAGE_WIDTH - iDistanceAway3; x++) {
	//		//左眼ななめ135要素強調
	//		btDiagonally135Left_odd3[x + IMAGE_WIDTH*y] = max(min(btDiagonally135Left_even3[x + (IMAGE_WIDTH*(y + iDistanceAway3)) - iDistanceAway3] - btDiagonally135Left_even3[x + (IMAGE_WIDTH*(y - iDistanceAway3)) + iDistanceAway3] + 128, 255), 0);
	//		//右眼ななめ45要素強調
	//		btDiagonally135Right_odd3[x + IMAGE_WIDTH*y] = max(min(btDiagonally135Right_even3[x + (IMAGE_WIDTH*(y + iDistanceAway3)) - iDistanceAway3] - btDiagonally135Right_even3[x + (IMAGE_WIDTH*(y - iDistanceAway3)) + iDistanceAway3] + 128, 255), 0);
	//	}
	//}

	//初期化
	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
		btOutputDisparityM[i] = 0;
	}

	//視差エネルギーの計算------------------------------------

	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = NUM_OF_PIXEL / 2 ; x < IMAGE_WIDTH - (NUM_OF_PIXEL / 2); x++) {

			//1階層目縦

			//初期化
			for (i = 0; i < NUM_OF_PIXEL; i++) {
				iDisparityEnergy[i] = 0;
			}
			
			if ((pow(double(btVerticalLeft_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btVerticalLeft_odd[x + IMAGE_WIDTH * y] - 128), 2))>thresh4) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -NUM_OF_PIXEL / 2; iRight_SearchRange <= NUM_OF_PIXEL / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)

																															//Even Part----------------
					iSumOfEven[iRight_SearchRange + NUM_OF_PIXEL / 2] = (btVerticalLeft_even[(x + IMAGE_WIDTH*y)] - 128)
						+ (btVerticalRight_even[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfEven[iRight_SearchRange + NUM_OF_PIXEL / 2] = (int)pow((double)iSumOfEven[iRight_SearchRange + NUM_OF_PIXEL / 2], 2);//2乗して負の値を除去

																																				  //Odd Part----------------
					iSumOfOdd[iRight_SearchRange + NUM_OF_PIXEL / 2] = (btVerticalLeft_odd[(x + IMAGE_WIDTH*y)] - 128)
						+ (btVerticalRight_odd[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfOdd[iRight_SearchRange + NUM_OF_PIXEL / 2] = (int)pow((double)iSumOfOdd[iRight_SearchRange + NUM_OF_PIXEL / 2], 2);	//2乗して負の値を除去

				}
				for (i = 0; i < NUM_OF_PIXEL; i++) {
					iDisparityEnergy[i] = iSumPowOfEven[i] + iSumPowOfOdd[i];	//EvenとOddの加算
				}

				//dblGpOut[5] = iDisparityEnergy[8];//デバッグ用

				//最大値の更新----------------
				iMax = iDisparityEnergy[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL; i++) {
					if (iMax < iDisparityEnergy[i]) {
						iMax = iDisparityEnergy[i];
						iMax_index = i;

					}
				}

				//出力------------------------
				btOutputDisparity[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDisparity[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}

			//視差別出力
			/*for (i = 0; i < 17; i = i + 4) {
				computedImage[4 + i / 4][x + IMAGE_WIDTH*y] = min(255, (double)iDisparityEnergy[i] / 4096 * 255);
			}*/

			//1階層目ななめ45

			//初期化
			for (i = 0; i < NUM_OF_PIXEL; i++) {
				iDisparityEnergyOfDiagonally45[i] = 0;
			}

			if ((pow(double(btDiagonally45Left_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btDiagonally45Left_odd[x + IMAGE_WIDTH * y] - 128), 2))>thresh4) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -NUM_OF_PIXEL / 2; iRight_SearchRange <= NUM_OF_PIXEL / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)

																															//Even Part----------------
					iSumOfEven[iRight_SearchRange + NUM_OF_PIXEL / 2] = (btDiagonally45Left_even[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally45Right_even[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfEven[iRight_SearchRange + NUM_OF_PIXEL / 2] = (int)pow((double)iSumOfEven[iRight_SearchRange + NUM_OF_PIXEL / 2], 2);//2乗して負の値を除去

																																				  //Odd Part----------------
					iSumOfOdd[iRight_SearchRange + NUM_OF_PIXEL / 2] = (btDiagonally45Left_odd[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally45Right_odd[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfOdd[iRight_SearchRange + NUM_OF_PIXEL / 2] = (int)pow((double)iSumOfOdd[iRight_SearchRange + NUM_OF_PIXEL / 2], 2);	//2乗して負の値を除去

				}
				for (i = 0; i < NUM_OF_PIXEL; i++) {
					iDisparityEnergyOfDiagonally45[i] = iSumPowOfEven[i] + iSumPowOfOdd[i];	//EvenとOddの加算
				}

				//dblGpOut[5] = iDisparityEnergyOfDiagonally45[8];//デバッグ用

												  //最大値の更新----------------
				iMax = iDisparityEnergyOfDiagonally45[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL; i++) {
					if (iMax < iDisparityEnergyOfDiagonally45[i]) {
						iMax = iDisparityEnergyOfDiagonally45[i];
						iMax_index = i;

					}
				}

				//出力------------------------
				btOutputDiagonally45Disparity[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDiagonally45Disparity[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}

			//視差別出力
			/*for (i = 0; i < 17; i = i + 4) {
				computedImage[28 + i / 4][x + IMAGE_WIDTH*y] = min(255, (double)iDisparityEnergyOfDiagonally45[i] / 4096 * 255);
			}*/

			//1階層目ななめ135

			//初期化
			for (i = 0; i < NUM_OF_PIXEL; i++) {
				iDisparityEnergyOfDiagonally135[i] = 0;
			}
			
			if ((pow(double(btDiagonally135Left_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btDiagonally135Left_odd[x + IMAGE_WIDTH * y] - 128), 2))>thresh4) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -NUM_OF_PIXEL / 2; iRight_SearchRange <= NUM_OF_PIXEL / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)

																															//Even Part----------------
					iSumOfEven[iRight_SearchRange + NUM_OF_PIXEL / 2] = (btDiagonally135Left_even[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally135Right_even[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfEven[iRight_SearchRange + NUM_OF_PIXEL / 2] = (int)pow((double)iSumOfEven[iRight_SearchRange + NUM_OF_PIXEL / 2], 2);//2乗して負の値を除去

																																				  //Odd Part----------------
					iSumOfOdd[iRight_SearchRange + NUM_OF_PIXEL / 2] = (btDiagonally135Left_odd[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally135Right_odd[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfOdd[iRight_SearchRange + NUM_OF_PIXEL / 2] = (int)pow((double)iSumOfOdd[iRight_SearchRange + NUM_OF_PIXEL / 2], 2);	//2乗して負の値を除去

				}
				
				for (i = 0; i < NUM_OF_PIXEL; i++) {
					iDisparityEnergyOfDiagonally135[i] = iSumPowOfEven[i] + iSumPowOfOdd[i];	//EvenとOddの加算
				}
				
				//dblGpOut[5] = iDisparityEnergyOfDiagonally135[8];//デバッグ用

												  //最大値の更新----------------
				iMax = iDisparityEnergyOfDiagonally135[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL; i++) {
					if (iMax < iDisparityEnergyOfDiagonally135[i]) {
						iMax = iDisparityEnergyOfDiagonally135[i];
						iMax_index = i;

					}
				}

				//出力------------------------
				btOutputDiagonally135Disparity[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDiagonally135Disparity[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}

			

			//2階層目縦

			//初期化
			for (i = 0; i < NUM_OF_PIXEL2; i++) {
				iDisparityEnergy2[i] = 0;
			}

			if ((pow(double(btVerticalLeft_even2[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btVerticalLeft_odd2[x + IMAGE_WIDTH * y] - 128), 2))>thresh5) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -NUM_OF_PIXEL2 / 2; iRight_SearchRange <= NUM_OF_PIXEL2 / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)

																															//Even Part----------------
					iSumOfEven2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (btVerticalLeft_even2[(x + IMAGE_WIDTH*y)] - 128)
						+ (btVerticalRight_even2[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfEven2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (int)pow((double)iSumOfEven2[iRight_SearchRange + NUM_OF_PIXEL2 / 2], 2);//2乗して負の値を除去

																																				  //Odd Part----------------
					iSumOfOdd2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (btVerticalLeft_odd2[(x + IMAGE_WIDTH*y)] - 128)
						+ (btVerticalRight_odd2[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfOdd2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (int)pow((double)iSumOfOdd2[iRight_SearchRange + NUM_OF_PIXEL2 / 2], 2);	//2乗して負の値を除去

				}
				for (i = 0; i < NUM_OF_PIXEL2; i++) {
					iDisparityEnergy2[i] = iSumPowOfEven2[i] + iSumPowOfOdd2[i];	//EvenとOddの加算
				}

				//dblGpOut[6] = iDisparityEnergy2[9];//デバッグ用

				//最大値の更新----------------
				iMax = iDisparityEnergy2[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL2; i++) {
					if (iMax < iDisparityEnergy2[i]) {
						iMax = iDisparityEnergy2[i];
						iMax_index = i;

					}
				}
			
				//出力------------------------
				btOutputDisparity2[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDisparity2[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}

			//視差別出力
			/*for (i = 0; i < 17; i = i + 4) {
				computedImage[10 + i / 4][x + IMAGE_WIDTH*y] = min(255, (double)iDisparityEnergy2[i] / 4096 * 255);
			}*/


			//2階層目ななめ45

			//初期化
			for (i = 0; i < NUM_OF_PIXEL2; i++) {
				iDisparityEnergyOfDiagonally45_2[i] = 0;
			}

			if ((pow(double(btDiagonally45Left_even2[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btDiagonally45Left_odd2[x + IMAGE_WIDTH * y] - 128), 2))>thresh5) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -NUM_OF_PIXEL2 / 2; iRight_SearchRange <= NUM_OF_PIXEL2 / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)

																															//Even Part----------------
					iSumOfEven2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (btDiagonally45Left_even2[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally45Right_even2[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfEven2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (int)pow((double)iSumOfEven2[iRight_SearchRange + NUM_OF_PIXEL2 / 2], 2);//2乗して負の値を除去

																																				  //Odd Part----------------
					iSumOfOdd2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (btDiagonally45Left_odd2[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally45Right_odd2[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfOdd2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (int)pow((double)iSumOfOdd2[iRight_SearchRange + NUM_OF_PIXEL2 / 2], 2);	//2乗して負の値を除去

				}
				for (i = 0; i < NUM_OF_PIXEL2; i++) {
					iDisparityEnergyOfDiagonally45_2[i] = iSumPowOfEven2[i] + iSumPowOfOdd2[i];	//EvenとOddの加算
				}

				//dblGpOut[5] = iDisparityEnergyOfDiagonally45_2[8];//デバッグ用

												  //最大値の更新----------------
				iMax = iDisparityEnergyOfDiagonally45_2[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL2; i++) {
					if (iMax < iDisparityEnergyOfDiagonally45_2[i]) {
						iMax = iDisparityEnergyOfDiagonally45_2[i];
						iMax_index = i;

					}
				}

				//出力------------------------
				btOutputDiagonally45Disparity2[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDiagonally45Disparity2[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}
		
			//2階層目ななめ135

			//初期化
			for (i = 0; i < NUM_OF_PIXEL2; i++) {
				iDisparityEnergyOfDiagonally135_2[i] = 0;
			}

			if ((pow(double(btDiagonally135Left_even2[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btDiagonally135Left_odd2[x + IMAGE_WIDTH * y] - 128), 2))>thresh5) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -NUM_OF_PIXEL2 / 2; iRight_SearchRange <= NUM_OF_PIXEL2 / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)

																															//Even Part----------------
					iSumOfEven2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (btDiagonally135Left_even2[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally135Right_even2[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfEven2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (int)pow((double)iSumOfEven2[iRight_SearchRange + NUM_OF_PIXEL2 / 2], 2);//2乗して負の値を除去

																																				  //Odd Part----------------
					iSumOfOdd2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (btDiagonally135Left_odd2[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally135Right_odd2[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfOdd2[iRight_SearchRange + NUM_OF_PIXEL2 / 2] = (int)pow((double)iSumOfOdd2[iRight_SearchRange + NUM_OF_PIXEL2 / 2], 2);	//2乗して負の値を除去

				}
				for (i = 0; i < NUM_OF_PIXEL2; i++) {
					iDisparityEnergyOfDiagonally135_2[i] = iSumPowOfEven2[i] + iSumPowOfOdd2[i];	//EvenとOddの加算
				}

				//dblGpOut[5] = iDisparityEnergyOfDiagonally135_2[8];//デバッグ用

												  //最大値の更新----------------
				iMax = iDisparityEnergyOfDiagonally135_2[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL2; i++) {
					if (iMax < iDisparityEnergyOfDiagonally135_2[i]) {
						iMax = iDisparityEnergyOfDiagonally135_2[i];
						iMax_index = i;

					}
				}

				//出力------------------------
				btOutputDiagonally135Disparity2[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDiagonally135Disparity2[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}

			//3階層目縦

			//初期化
			for (i = 0; i < NUM_OF_PIXEL3; i++) {
				iDisparityEnergy3[i] = 0;
			}

			if ((pow(double(btVerticalLeft_even3[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btVerticalLeft_odd3[x + IMAGE_WIDTH * y] - 128), 2))>thresh6) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -NUM_OF_PIXEL3 / 2; iRight_SearchRange <= NUM_OF_PIXEL3 / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)

																															//Even Part----------------
					iSumOfEven3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (btVerticalLeft_even3[(x + IMAGE_WIDTH*y)] - 128)
						+ (btVerticalRight_even3[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfEven3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (int)pow((double)iSumOfEven3[iRight_SearchRange + NUM_OF_PIXEL3 / 2], 2);//2乗して負の値を除去

																																				  //Odd Part----------------
					iSumOfOdd3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (btVerticalLeft_odd3[(x + IMAGE_WIDTH*y)] - 128)
						+ (btVerticalRight_odd3[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfOdd3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (int)pow((double)iSumOfOdd3[iRight_SearchRange + NUM_OF_PIXEL3 / 2], 2);	//2乗して負の値を除去

				}
				for (i = 0; i < NUM_OF_PIXEL3; i++) {
					iDisparityEnergy3[i] = iSumPowOfEven3[i] + iSumPowOfOdd3[i];	//EvenとOddの加算
				}

				//dblGpOut[7] = iDisparityEnergy3[9];//デバッグ用

				//最大値の更新----------------
				iMax = iDisparityEnergy3[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL3; i++) {
					if (iMax < iDisparityEnergy3[i]) {
						iMax = iDisparityEnergy3[i];
						iMax_index = i;

					}
				}

				//出力------------------------
				btOutputDisparity3[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDisparity3[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}

			//for (i = 0; i < 17; i = i + 4) {
			//	computedImage[16 + i / 4][x + IMAGE_WIDTH*y] = min(255, (double)iDisparityEnergy3[i] / 4096 * 255);
			//}

			//3階層目ななめ45

			//初期化
			for (i = 0; i < NUM_OF_PIXEL3; i++) {
				iDisparityEnergyOfDiagonally45_3[i] = 0;
			}

			if ((pow(double(btDiagonally45Left_even3[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btDiagonally45Left_odd3[x + IMAGE_WIDTH * y] - 128), 2))>thresh6) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -NUM_OF_PIXEL3 / 2; iRight_SearchRange <= NUM_OF_PIXEL3 / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)

																															//Even Part----------------
					iSumOfEven3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (btDiagonally45Left_even3[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally45Right_even3[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfEven3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (int)pow((double)iSumOfEven3[iRight_SearchRange + NUM_OF_PIXEL3 / 2], 2);//2乗して負の値を除去

																																				  //Odd Part----------------
					iSumOfOdd3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (btDiagonally45Left_odd3[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally45Right_odd3[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfOdd3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (int)pow((double)iSumOfOdd3[iRight_SearchRange + NUM_OF_PIXEL3 / 2], 2);	//2乗して負の値を除去

				}
				for (i = 0; i < NUM_OF_PIXEL3; i++) {
					iDisparityEnergyOfDiagonally45_3[i] = iSumPowOfEven3[i] + iSumPowOfOdd3[i];	//EvenとOddの加算
				}

				//dblGpOut[5] = iDisparityEnergyOfDiagonally45_3[8];//デバッグ用

												  //最大値の更新----------------
				iMax = iDisparityEnergyOfDiagonally45_3[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL3; i++) {
					if (iMax < iDisparityEnergyOfDiagonally45_3[i]) {
						iMax = iDisparityEnergyOfDiagonally45_3[i];
						iMax_index = i;

					}
				}

				//出力------------------------
				btOutputDiagonally45Disparity3[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDiagonally45Disparity3[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}


			//3階層目ななめ135

			//初期化
			for (i = 0; i < NUM_OF_PIXEL3; i++) {
				iDisparityEnergyOfDiagonally135_3[i] = 0;
			}

			if ((pow(double(btDiagonally135Left_even3[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btDiagonally135Left_odd3[x + IMAGE_WIDTH * y] - 128), 2))>thresh6) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -NUM_OF_PIXEL3 / 2; iRight_SearchRange <= NUM_OF_PIXEL3 / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)

																															//Even Part----------------
					iSumOfEven3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (btDiagonally135Left_even3[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally135Right_even3[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfEven3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (int)pow((double)iSumOfEven3[iRight_SearchRange + NUM_OF_PIXEL3 / 2], 2);//2乗して負の値を除去

																																				  //Odd Part----------------
					iSumOfOdd3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (btDiagonally135Left_odd3[(x + IMAGE_WIDTH*y)] - 128)
						+ (btDiagonally135Right_odd3[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
					iSumPowOfOdd3[iRight_SearchRange + NUM_OF_PIXEL3 / 2] = (int)pow((double)iSumOfOdd3[iRight_SearchRange + NUM_OF_PIXEL3 / 2], 2);	//2乗して負の値を除去

				}
				for (i = 0; i < NUM_OF_PIXEL3; i++) {
					iDisparityEnergyOfDiagonally135_3[i] = iSumPowOfEven3[i] + iSumPowOfOdd3[i];	//EvenとOddの加算
				}

				//dblGpOut[5] = iDisparityEnergyOfDiagonally135_3[8];//デバッグ用

												  //最大値の更新----------------
				iMax = iDisparityEnergyOfDiagonally135_3[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL3; i++) {
					if (iMax < iDisparityEnergyOfDiagonally135_3[i]) {
						iMax = iDisparityEnergyOfDiagonally135_3[i];
						iMax_index = i;

					}
				}

				//出力------------------------
				btOutputDiagonally135Disparity3[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDiagonally135Disparity3[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}

			//統合エネルギー算出　縦
			for (i = 0; i < NUM_OF_PIXEL; i++) {
				iMargeDisparityEnergy[i] = 0;
			}
			iMargeDisparityEnergy[0] = k1*iDisparityEnergy3[0] + k1*iDisparityEnergy2[0] + iDisparityEnergy[0];
			iMargeDisparityEnergy[1] = k1*iDisparityEnergy3[0] + k2*iDisparityEnergy3[0] + k2*iDisparityEnergy3[4] + k2*iDisparityEnergy2[0] + k2*iDisparityEnergy2[2] + iDisparityEnergy[1];
			iMargeDisparityEnergy[2] = k2*iDisparityEnergy3[0] + k2*iDisparityEnergy3[4] + k1*iDisparityEnergy2[2] + iDisparityEnergy[2];
			iMargeDisparityEnergy[3] = k2*iDisparityEnergy3[0] + k2*iDisparityEnergy3[4] + k2*iDisparityEnergy3[4] + k2*iDisparityEnergy2[2] + k2*iDisparityEnergy2[4] + iDisparityEnergy[3];
			
			iMargeDisparityEnergy[4] = k1*iDisparityEnergy3[4] + k1*iDisparityEnergy2[4] + iDisparityEnergy[4];
			iMargeDisparityEnergy[5] = k1*iDisparityEnergy3[4] + k2*iDisparityEnergy3[4] + k2*iDisparityEnergy3[8] + k2*iDisparityEnergy2[4] + k2*iDisparityEnergy2[6] + iDisparityEnergy[5];
			iMargeDisparityEnergy[6] = k2*iDisparityEnergy3[4] + k2*iDisparityEnergy3[8] + k1*iDisparityEnergy2[6] + iDisparityEnergy[6];
			iMargeDisparityEnergy[7] = k2*iDisparityEnergy3[4] + k2*iDisparityEnergy3[8] + k1*iDisparityEnergy3[8] + k2*iDisparityEnergy2[6] + k2*iDisparityEnergy2[8] + iDisparityEnergy[7];

			iMargeDisparityEnergy[8] = k1*iDisparityEnergy3[8] + k1*iDisparityEnergy2[8] + iDisparityEnergy[8];
			iMargeDisparityEnergy[9] = k1*iDisparityEnergy3[8] + k2*iDisparityEnergy3[8] + k2*iDisparityEnergy3[12] + k2*iDisparityEnergy2[8] + k2*iDisparityEnergy2[10] + iDisparityEnergy[9];
			iMargeDisparityEnergy[10] = k2*iDisparityEnergy3[8] + k2*iDisparityEnergy3[12] + k1*iDisparityEnergy2[10] + iDisparityEnergy[10];
			iMargeDisparityEnergy[11] = k2*iDisparityEnergy3[8] + k2*iDisparityEnergy3[12] + k1*iDisparityEnergy3[12] + k2*iDisparityEnergy2[10] + k2*iDisparityEnergy2[12] + iDisparityEnergy[11];

			iMargeDisparityEnergy[12] = k1*iDisparityEnergy3[12] + k1*iDisparityEnergy2[12] + iDisparityEnergy[12];
			iMargeDisparityEnergy[13] = k1*iDisparityEnergy3[12] + k2*iDisparityEnergy3[12] + k2*iDisparityEnergy3[16] + k2*iDisparityEnergy2[12] + k2*iDisparityEnergy2[14] + iDisparityEnergy[13];
			iMargeDisparityEnergy[14] = k2*iDisparityEnergy3[12] + k2*iDisparityEnergy3[16] + k1*iDisparityEnergy2[14] + iDisparityEnergy[14];
			iMargeDisparityEnergy[15] = k2*iDisparityEnergy3[12] + k2*iDisparityEnergy3[16] + k1*iDisparityEnergy3[16] + k2*iDisparityEnergy2[14] + k2*iDisparityEnergy2[16] + iDisparityEnergy[15];

			iMargeDisparityEnergy[16] = k1*iDisparityEnergy3[16] + k1*iDisparityEnergy2[16] + iDisparityEnergy[16];


			if ((pow(double(btVerticalLeft_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btVerticalLeft_odd[x + IMAGE_WIDTH * y] - 128), 2))>thresh4) {//物体の特徴が認識されるとき
				//最大値の更新----------------
				iMax = iMargeDisparityEnergy[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL; i++) {
					if (iMax < iMargeDisparityEnergy[i]) {
						iMax = iMargeDisparityEnergy[i];
						iMax_index = i;

					}
				}
				
				//出力------------------------
				btOutputDisparityM[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
				}
			else {
				btOutputDisparityM[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}

			//縦統合エネ視差別出力
			for (i = 0; i < 17; i = i + 8) {
				computedImage[0 + i / 8][x + IMAGE_WIDTH*y] = min(255, (double)iMargeDisparityEnergy[i] / 4096 * 255);
			}

			//統合エネルギー算出　ななめ45
			for (i = 0; i < NUM_OF_PIXEL; i++) {
				iMargeDisparityEnergyOfDiagonally45[i] = 0;
			}
			iMargeDisparityEnergyOfDiagonally45[0] = k1*iDisparityEnergyOfDiagonally45_3[0] + k1*iDisparityEnergyOfDiagonally45_2[0] + iDisparityEnergyOfDiagonally45[0];
			iMargeDisparityEnergyOfDiagonally45[1] = k1*iDisparityEnergyOfDiagonally45_3[0] + k2*iDisparityEnergyOfDiagonally45_3[0] + k2*iDisparityEnergyOfDiagonally45_3[4] + k2*iDisparityEnergyOfDiagonally45_2[0] + k2*iDisparityEnergyOfDiagonally45_2[2] + iDisparityEnergyOfDiagonally45[1];
			iMargeDisparityEnergyOfDiagonally45[2] = k2*iDisparityEnergyOfDiagonally45_3[0] + k2*iDisparityEnergyOfDiagonally45_3[4] + k1*iDisparityEnergyOfDiagonally45_2[2] + iDisparityEnergyOfDiagonally45[2];
			iMargeDisparityEnergyOfDiagonally45[3] = k2*iDisparityEnergyOfDiagonally45_3[0] + k2*iDisparityEnergyOfDiagonally45_3[4] + k2*iDisparityEnergyOfDiagonally45_3[4] + k2*iDisparityEnergyOfDiagonally45_2[2] + k2*iDisparityEnergyOfDiagonally45_2[4] + iDisparityEnergyOfDiagonally45[3];

			iMargeDisparityEnergyOfDiagonally45[4] = k1*iDisparityEnergyOfDiagonally45_3[4] + k1*iDisparityEnergyOfDiagonally45_2[4] + iDisparityEnergyOfDiagonally45[4];
			iMargeDisparityEnergyOfDiagonally45[5] = k1*iDisparityEnergyOfDiagonally45_3[4] + k2*iDisparityEnergyOfDiagonally45_3[4] + k2*iDisparityEnergyOfDiagonally45_3[8] + k2*iDisparityEnergyOfDiagonally45_2[4] + k2*iDisparityEnergyOfDiagonally45_2[6] + iDisparityEnergyOfDiagonally45[5];
			iMargeDisparityEnergyOfDiagonally45[6] = k2*iDisparityEnergyOfDiagonally45_3[4] + k2*iDisparityEnergyOfDiagonally45_3[8] + k1*iDisparityEnergyOfDiagonally45_2[6] + iDisparityEnergyOfDiagonally45[6];
			iMargeDisparityEnergyOfDiagonally45[7] = k2*iDisparityEnergyOfDiagonally45_3[4] + k2*iDisparityEnergyOfDiagonally45_3[8] + k1*iDisparityEnergyOfDiagonally45_3[8] + k2*iDisparityEnergyOfDiagonally45_2[6] + k2*iDisparityEnergyOfDiagonally45_2[8] + iDisparityEnergyOfDiagonally45[7];

			iMargeDisparityEnergyOfDiagonally45[8] = k1*iDisparityEnergyOfDiagonally45_3[8] + k1*iDisparityEnergyOfDiagonally45_2[8] + iDisparityEnergyOfDiagonally45[8];
			iMargeDisparityEnergyOfDiagonally45[9] = k1*iDisparityEnergyOfDiagonally45_3[8] + k2*iDisparityEnergyOfDiagonally45_3[8] + k2*iDisparityEnergyOfDiagonally45_3[12] + k2*iDisparityEnergyOfDiagonally45_2[8] + k2*iDisparityEnergyOfDiagonally45_2[10] + iDisparityEnergyOfDiagonally45[9];
			iMargeDisparityEnergyOfDiagonally45[10] = k2*iDisparityEnergyOfDiagonally45_3[8] + k2*iDisparityEnergyOfDiagonally45_3[12] + k1*iDisparityEnergyOfDiagonally45_2[10] + iDisparityEnergyOfDiagonally45[10];
			iMargeDisparityEnergyOfDiagonally45[11] = k2*iDisparityEnergyOfDiagonally45_3[8] + k2*iDisparityEnergyOfDiagonally45_3[12] + k1*iDisparityEnergyOfDiagonally45_3[12] + k2*iDisparityEnergyOfDiagonally45_2[10] + k2*iDisparityEnergyOfDiagonally45_2[12] + iDisparityEnergyOfDiagonally45[11];

			iMargeDisparityEnergyOfDiagonally45[12] = k1*iDisparityEnergyOfDiagonally45_3[12] + k1*iDisparityEnergyOfDiagonally45_2[12] + iDisparityEnergyOfDiagonally45[12];
			iMargeDisparityEnergyOfDiagonally45[13] = k1*iDisparityEnergyOfDiagonally45_3[12] + k2*iDisparityEnergyOfDiagonally45_3[12] + k2*iDisparityEnergyOfDiagonally45_3[16] + k2*iDisparityEnergyOfDiagonally45_2[12] + k2*iDisparityEnergyOfDiagonally45_2[14] + iDisparityEnergyOfDiagonally45[13];
			iMargeDisparityEnergyOfDiagonally45[14] = k2*iDisparityEnergyOfDiagonally45_3[12] + k2*iDisparityEnergyOfDiagonally45_3[16] + k1*iDisparityEnergyOfDiagonally45_2[14] + iDisparityEnergyOfDiagonally45[14];
			iMargeDisparityEnergyOfDiagonally45[15] = k2*iDisparityEnergyOfDiagonally45_3[12] + k2*iDisparityEnergyOfDiagonally45_3[16] + k1*iDisparityEnergyOfDiagonally45_3[16] + k2*iDisparityEnergyOfDiagonally45_2[14] + k2*iDisparityEnergyOfDiagonally45_2[16] + iDisparityEnergyOfDiagonally45[15];

			iMargeDisparityEnergyOfDiagonally45[16] = k1*iDisparityEnergyOfDiagonally45_3[16] + k1*iDisparityEnergyOfDiagonally45_2[16] + iDisparityEnergyOfDiagonally45[16];


			if ((pow(double(btDiagonally45Left_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btDiagonally45Left_odd[x + IMAGE_WIDTH * y] - 128), 2))>thresh4) {//物体の特徴が認識されるとき
																																						   //最大値の更新----------------
				iMax = iMargeDisparityEnergyOfDiagonally45[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL; i++) {
					if (iMax < iMargeDisparityEnergyOfDiagonally45[i]) {
						iMax = iMargeDisparityEnergyOfDiagonally45[i];
						iMax_index = i;

					}
				}

				//出力------------------------
				btOutputDisparityOfDiagonally45M[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDisparityOfDiagonally45M[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}

			//統合エネルギー算出　ななめ135
			for (i = 0; i < NUM_OF_PIXEL; i++) {
				iMargeDisparityEnergyOfDiagonally135[i] = 0;
			}
			iMargeDisparityEnergyOfDiagonally135[0] = k1*iDisparityEnergyOfDiagonally135_3[0] + k1*iDisparityEnergyOfDiagonally135_2[0] + iDisparityEnergyOfDiagonally135[0];
			iMargeDisparityEnergyOfDiagonally135[1] = k1*iDisparityEnergyOfDiagonally135_3[0] + k2*iDisparityEnergyOfDiagonally135_3[0] + k2*iDisparityEnergyOfDiagonally135_3[4] + k2*iDisparityEnergyOfDiagonally135_2[0] + k2*iDisparityEnergyOfDiagonally135_2[2] + iDisparityEnergyOfDiagonally135[1];
			iMargeDisparityEnergyOfDiagonally135[2] = k2*iDisparityEnergyOfDiagonally135_3[0] + k2*iDisparityEnergyOfDiagonally135_3[4] + k1*iDisparityEnergyOfDiagonally135_2[2] + iDisparityEnergyOfDiagonally135[2];
			iMargeDisparityEnergyOfDiagonally135[3] = k2*iDisparityEnergyOfDiagonally135_3[0] + k2*iDisparityEnergyOfDiagonally135_3[4] + k2*iDisparityEnergyOfDiagonally135_3[4] + k2*iDisparityEnergyOfDiagonally135_2[2] + k2*iDisparityEnergyOfDiagonally135_2[4] + iDisparityEnergyOfDiagonally135[3];

			iMargeDisparityEnergyOfDiagonally135[4] = k1*iDisparityEnergyOfDiagonally135_3[4] + k1*iDisparityEnergyOfDiagonally135_2[4] + iDisparityEnergyOfDiagonally135[4];
			iMargeDisparityEnergyOfDiagonally135[5] = k1*iDisparityEnergyOfDiagonally135_3[4] + k2*iDisparityEnergyOfDiagonally135_3[4] + k2*iDisparityEnergyOfDiagonally135_3[8] + k2*iDisparityEnergyOfDiagonally135_2[4] + k2*iDisparityEnergyOfDiagonally135_2[6] + iDisparityEnergyOfDiagonally135[5];
			iMargeDisparityEnergyOfDiagonally135[6] = k2*iDisparityEnergyOfDiagonally135_3[4] + k2*iDisparityEnergyOfDiagonally135_3[8] + k1*iDisparityEnergyOfDiagonally135_2[6] + iDisparityEnergyOfDiagonally135[6];
			iMargeDisparityEnergyOfDiagonally135[7] = k2*iDisparityEnergyOfDiagonally135_3[4] + k2*iDisparityEnergyOfDiagonally135_3[8] + k1*iDisparityEnergyOfDiagonally135_3[8] + k2*iDisparityEnergyOfDiagonally135_2[6] + k2*iDisparityEnergyOfDiagonally135_2[8] + iDisparityEnergyOfDiagonally135[7];

			iMargeDisparityEnergyOfDiagonally135[8] = k1*iDisparityEnergyOfDiagonally135_3[8] + k1*iDisparityEnergyOfDiagonally135_2[8] + iDisparityEnergyOfDiagonally135[8];
			iMargeDisparityEnergyOfDiagonally135[9] = k1*iDisparityEnergyOfDiagonally135_3[8] + k2*iDisparityEnergyOfDiagonally135_3[8] + k2*iDisparityEnergyOfDiagonally135_3[12] + k2*iDisparityEnergyOfDiagonally135_2[8] + k2*iDisparityEnergyOfDiagonally135_2[10] + iDisparityEnergyOfDiagonally135[9];
			iMargeDisparityEnergyOfDiagonally135[10] = k2*iDisparityEnergyOfDiagonally135_3[8] + k2*iDisparityEnergyOfDiagonally135_3[12] + k1*iDisparityEnergyOfDiagonally135_2[10] + iDisparityEnergyOfDiagonally135[10];
			iMargeDisparityEnergyOfDiagonally135[11] = k2*iDisparityEnergyOfDiagonally135_3[8] + k2*iDisparityEnergyOfDiagonally135_3[12] + k1*iDisparityEnergyOfDiagonally135_3[12] + k2*iDisparityEnergyOfDiagonally135_2[10] + k2*iDisparityEnergyOfDiagonally135_2[12] + iDisparityEnergyOfDiagonally135[11];

			iMargeDisparityEnergyOfDiagonally135[12] = k1*iDisparityEnergyOfDiagonally135_3[12] + k1*iDisparityEnergyOfDiagonally135_2[12] + iDisparityEnergyOfDiagonally135[12];
			iMargeDisparityEnergyOfDiagonally135[13] = k1*iDisparityEnergyOfDiagonally135_3[12] + k2*iDisparityEnergyOfDiagonally135_3[12] + k2*iDisparityEnergyOfDiagonally135_3[16] + k2*iDisparityEnergyOfDiagonally135_2[12] + k2*iDisparityEnergyOfDiagonally135_2[14] + iDisparityEnergyOfDiagonally135[13];
			iMargeDisparityEnergyOfDiagonally135[14] = k2*iDisparityEnergyOfDiagonally135_3[12] + k2*iDisparityEnergyOfDiagonally135_3[16] + k1*iDisparityEnergyOfDiagonally135_2[14] + iDisparityEnergyOfDiagonally135[14];
			iMargeDisparityEnergyOfDiagonally135[15] = k2*iDisparityEnergyOfDiagonally135_3[12] + k2*iDisparityEnergyOfDiagonally135_3[16] + k1*iDisparityEnergyOfDiagonally135_3[16] + k2*iDisparityEnergyOfDiagonally135_2[14] + k2*iDisparityEnergyOfDiagonally135_2[16] + iDisparityEnergyOfDiagonally135[15];

			iMargeDisparityEnergyOfDiagonally135[16] = k1*iDisparityEnergyOfDiagonally135_3[16] + k1*iDisparityEnergyOfDiagonally135_2[16] + iDisparityEnergyOfDiagonally135[16];
			

			if ((pow(double(btDiagonally135Left_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btDiagonally135Left_odd[x + IMAGE_WIDTH * y] - 128), 2))>thresh4) {//物体の特徴が認識されるとき
																																						   //最大値の更新----------------
				iMax = iMargeDisparityEnergyOfDiagonally135[0];
				iMax_index = 0;

				for (i = 1; i < NUM_OF_PIXEL; i++) {
					if (iMax < iMargeDisparityEnergyOfDiagonally135[i]) {
						iMax = iMargeDisparityEnergyOfDiagonally135[i];
						iMax_index = i;

					}
				}

				//出力------------------------
				btOutputDisparityOfDiagonally135M[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
			}
			else {
				btOutputDisparityOfDiagonally135M[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
			}

			//視差マップを結合(ななめと縦の結合)
			if (btOutputDisparity[x + IMAGE_WIDTH*y] != 0) {
				btOutPutParfectDisparity[x + IMAGE_WIDTH*y] = btOutputDisparity[x + IMAGE_WIDTH*y];
			}
			else if (btOutputDiagonally45Disparity[x + IMAGE_WIDTH*y] != 0) {
				btOutPutParfectDisparity[x + IMAGE_WIDTH*y] = btOutputDiagonally45Disparity[x + IMAGE_WIDTH*y];
			}
			else if (btOutputDiagonally135Disparity[x + IMAGE_WIDTH*y] != 0) {
				btOutPutParfectDisparity[x + IMAGE_WIDTH*y] = btOutputDiagonally135Disparity[x + IMAGE_WIDTH*y];
			}
			else {
				btOutPutParfectDisparity[x + IMAGE_WIDTH*y] = 0;
			}

			//統合視差マップを結合(ななめと縦の結合)
			if (btOutputDisparityM[x + IMAGE_WIDTH*y] != 0) {
				btOutPutParfectDisparityM[x + IMAGE_WIDTH*y] = btOutputDisparityM[x + IMAGE_WIDTH*y];
			}
			else if (btOutputDisparityOfDiagonally45M[x + IMAGE_WIDTH*y] != 0) {
				btOutPutParfectDisparityM[x + IMAGE_WIDTH*y] = btOutputDisparityOfDiagonally45M[x + IMAGE_WIDTH*y];
			}
			else if (btOutputDisparityOfDiagonally135M[x + IMAGE_WIDTH*y] != 0) {
				btOutPutParfectDisparityM[x + IMAGE_WIDTH*y] = btOutputDisparityOfDiagonally135M[x + IMAGE_WIDTH*y];
			}
			else {
				btOutPutParfectDisparityM[x + IMAGE_WIDTH*y] = 0;
			}

			for (i = 0; i < NUM_OF_PIXEL; i++) {
				iL0[x + IMAGE_WIDTH*y][i] = iMargeDisparityEnergy[i]; //cooperative
				//dblL[x + IMAGE_WIDTH*y][i] = (double)iMargeDisparityEnergy[i];	//cooperative
				dblL[x + IMAGE_WIDTH*y][i] = (double)iDisparityEnergy3[i];	//cooperative

				iL0Diagonally45[x + IMAGE_WIDTH*y][i] = iMargeDisparityEnergyOfDiagonally45[i]; //cooperative
				//dblLDiagonally45[x + IMAGE_WIDTH*y][i] = (double)iMargeDisparityEnergyOfDiagonally45[i];	//cooperative
				dblLDiagonally45[x + IMAGE_WIDTH*y][i] = (double)iDisparityEnergyOfDiagonally45_3[i];	//cooperative

				iL0Diagonally135[x + IMAGE_WIDTH*y][i] = iMargeDisparityEnergyOfDiagonally135[i]; //cooperative
				//dblLDiagonally135[x + IMAGE_WIDTH*y][i] = (double)iMargeDisparityEnergyOfDiagonally135[i];	//cooperative
				dblLDiagonally135[x + IMAGE_WIDTH*y][i] = (double)iDisparityEnergyOfDiagonally135_3[i];	//cooperative
			}

		}
	}

	//int uiIteration = 0;
	//while (uiIteration < dblGpIn[1]) {
	//
	//	for (y = 1; y < IMAGE_HEIGHT - 1; y++) {
	//		for (x = 9; x < IMAGE_WIDTH - 9; x++) {
	//			for (i = 1; i < 16; i++) {
	//				for (iRow = -PHI / 2; iRow <= PHI / 2; iRow++) {
	//					for (iColum = -PHI / 2; iColum <= PHI / 2; iColum++) {
	//						for (iDisparity = -PHI / 2; iDisparity <= PHI / 2; iDisparity++) {
	//							dblS[x + IMAGE_WIDTH*y][i] += dblL[x + iColum + IMAGE_WIDTH * (y + iRow)][i + iDisparity];

	//							dblSDiagonally45[x + IMAGE_WIDTH*y][i] += dblLDiagonally45[x + iColum + IMAGE_WIDTH * (y + iRow)][i + iDisparity];

	//							dblSDiagonally135[x + IMAGE_WIDTH*y][i] += dblLDiagonally135[x + iColum + IMAGE_WIDTH * (y + iRow)][i + iDisparity];
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//
	//	for (y = 1; y < IMAGE_HEIGHT-1; y++) {
	//		for (x = 16; x < IMAGE_WIDTH - 16; x++) {
	//			for (i = 0; i < 17; i++) {
	//
	//				dblSumOfWithinPSI = 0;
	//				dblSumOfWithinPSIDiagonally45 = 0;
	//				dblSumOfWithinPSIDiagonally135 = 0;
	//
	//				for (iDisparity2 = -PSI / 2; iDisparity2 <= PSI / 2; iDisparity2++) {
	//					for (iColum2 = -PSI / 2; iColum2 <= PSI / 2; iColum2++) {
	//						if (iColum2 == -iDisparity2 || iColum2 == 0) {
	//							if (iDisparity2 != 0) {
	//								dblSumOfWithinPSI += dblS[x + iColum2 + IMAGE_WIDTH * y][iDisparity2 + PSI / 2];

	//								dblSumOfWithinPSIDiagonally45 += dblSDiagonally45[x + iColum2 + IMAGE_WIDTH * y][iDisparity2 + PSI / 2];

	//								dblSumOfWithinPSIDiagonally135 += dblSDiagonally135[x + iColum2 + IMAGE_WIDTH * y][iDisparity2 + PSI / 2];
	//							}
	//						}
	//					}
	//				}
	//
	//				if (dblSumOfWithinPSI != 0) {
	//					dblR[x + IMAGE_WIDTH*y][i] = pow(dblS[x + IMAGE_WIDTH*y][i] / dblSumOfWithinPSI, ALPHA);
	//				}
	//				if (dblSumOfWithinPSIDiagonally45 != 0) {
	//					dblRDiagonally45[x + IMAGE_WIDTH*y][i] = pow(dblSDiagonally45[x + IMAGE_WIDTH*y][i] / dblSumOfWithinPSIDiagonally45, ALPHA);
	//				}
	//				if (dblSumOfWithinPSIDiagonally135 != 0) {
	//					dblRDiagonally135[x + IMAGE_WIDTH*y][i] = pow(dblSDiagonally135[x + IMAGE_WIDTH*y][i] / dblSumOfWithinPSIDiagonally135, ALPHA);
	//				}
	//
	//			}
	//
	//		}
	//	}
	//
	//	for (y = 0; y < IMAGE_HEIGHT; y++) {
	//		for (x = 8; x < IMAGE_WIDTH - 8; x++) {
	//			for (i = 0; i < 17; i++) {
	//				dblT[x + IMAGE_WIDTH*y][i] = iL0[x + IMAGE_WIDTH*y][i] * dblR[x + IMAGE_WIDTH*y][i];

	//				dblTDiagonally45[x + IMAGE_WIDTH*y][i] = iL0Diagonally45[x + IMAGE_WIDTH*y][i] * dblRDiagonally45[x + IMAGE_WIDTH*y][i];

	//				dblTDiagonally135[x + IMAGE_WIDTH*y][i] = iL0Diagonally135[x + IMAGE_WIDTH*y][i] * dblRDiagonally135[x + IMAGE_WIDTH*y][i];
	//			}
	//		}
	//	}
	//
	//	for (y = 0; y < IMAGE_HEIGHT; y++) {
	//		for (x = 8; x < IMAGE_WIDTH - 8; x++) {
	//			for (i = 0; i < 17; i++) {
	//				dblL[x + IMAGE_WIDTH*y][i] = dblT[x + IMAGE_WIDTH*y][i];

	//				dblLDiagonally45[x + IMAGE_WIDTH*y][i] = dblTDiagonally45[x + IMAGE_WIDTH*y][i];

	//				dblLDiagonally135[x + IMAGE_WIDTH*y][i] = dblTDiagonally135[x + IMAGE_WIDTH*y][i];
	//					
	//			}
	//		}
	//	}
	//	uiIteration++;
	//}

	//
	////after cooperative 視差別出力
	//for (y = 1; y < IMAGE_HEIGHT-1; y++) {
	//	for (x = NUM_OF_PIXEL; x < IMAGE_WIDTH - (NUM_OF_PIXEL); x++) {
	//		for (i = 0; i < 17; i++) {
	//			if (i == 1 || i == 4 || i == 8 || i == 12 || i == 15) {
	//				computedImage[22 + (i + 1) / 4][x + IMAGE_WIDTH*y] = min(255, dblL[x + IMAGE_WIDTH*y][i] / 1024 * 255);
	//			}
	//		}
	//	}
	//}

	////協調アルゴリズム　視差マップ作成　縦
	//for (y = 1; y < IMAGE_HEIGHT - 1; y++) {
	//	for (x = 16; x < IMAGE_WIDTH - 16; x++) {
	//
	//		if ((pow(double(btVerticalLeft_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btVerticalLeft_odd[x + IMAGE_WIDTH * y] - 128), 2)) > thresh5) {
	//			//最大値の更新----------------
	//			dblMax = dblL[x + IMAGE_WIDTH*y][0];
	//			iMax_index = 0;
	//
	//			for (i = 1; i < 17; i++) {
	//				if (dblMax < dblL[x + IMAGE_WIDTH*y][i]) {
	//					dblMax = dblL[x + IMAGE_WIDTH*y][i];
	//					iMax_index = i;
	//				}
	//			}
	//
	//			//出力------------------------
	//			computedImage[13][x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
	//		}
	//		else {
	//			computedImage[13][x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
	//		}
	//	}
	//}
	//
	////協調アルゴリズム　視差マップ作成　ななめ45
	//for (y = 1; y < IMAGE_HEIGHT - 1; y++) {
	//	for (x = 16; x < IMAGE_WIDTH - 16; x++) {

	//		if ((pow(double(btDiagonally45Left_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btDiagonally45Left_odd[x + IMAGE_WIDTH * y] - 128), 2)) > thresh5) {
	//			//最大値の更新----------------
	//			dblMax = dblLDiagonally45[x + IMAGE_WIDTH*y][0];
	//			iMax_index = 0;

	//			for (i = 1; i < 17; i++) {
	//				if (dblMax < dblLDiagonally45[x + IMAGE_WIDTH*y][i]) {
	//					dblMax = dblLDiagonally45[x + IMAGE_WIDTH*y][i];
	//					iMax_index = i;
	//				}
	//			}

	//			//出力------------------------
	//			computedImage[14][x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
	//		}
	//		else {
	//			computedImage[14][x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
	//		}
	//	}
	//}

	////協調アルゴリズム　視差マップ作成　ななめ135
	//for (y = 1; y < IMAGE_HEIGHT - 1; y++) {
	//	for (x = 16; x < IMAGE_WIDTH - 16; x++) {

	//		if ((pow(double(btDiagonally135Left_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btDiagonally135Left_odd[x + IMAGE_WIDTH * y] - 128), 2)) > thresh5) {
	//			//最大値の更新----------------
	//			dblMax = dblLDiagonally135[x + IMAGE_WIDTH*y][0];
	//			iMax_index = 0;

	//			for (i = 1; i < 17; i++) {
	//				if (dblMax < dblLDiagonally135[x + IMAGE_WIDTH*y][i]) {
	//					dblMax = dblLDiagonally135[x + IMAGE_WIDTH*y][i];
	//					iMax_index = i;
	//				}
	//			}

	//			//出力------------------------
	//			computedImage[15][x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);		//-8から8までの17点のときの出力式
	//		}
	//		else {
	//			computedImage[15][x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
	//		}
	//	}
	//}

	//

	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	for (x = NUM_OF_PIXEL / 2; x < IMAGE_WIDTH - (NUM_OF_PIXEL / 2); x++) {
	//		//視差マップを統合(ななめと縦の統合)
	//		if (computedImage[13][x + IMAGE_WIDTH*y] != 0) {
	//			computedImage[10][x + IMAGE_WIDTH*y] = computedImage[13][x + IMAGE_WIDTH*y];
	//		}
	//		else if (computedImage[14][x + IMAGE_WIDTH*y] != 0) {
	//			computedImage[10][x + IMAGE_WIDTH*y] = computedImage[14][x + IMAGE_WIDTH*y];
	//		}
	//		else if (computedImage[15][x + IMAGE_WIDTH*y] != 0) {
	//			computedImage[10][x + IMAGE_WIDTH*y] = computedImage[15][x + IMAGE_WIDTH*y];
	//		}
	//		else {
	//			computedImage[10][x + IMAGE_WIDTH*y] = 0;
	//		}
	//	}
	//}



	//memcpy(computedImage[4], btEdgeEmphasisLeft, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[5], btEdgeEmphasisRight, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[6], btVerticalLeft_even, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[7], btVerticalRight_even, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[8], btVerticalLeft_odd, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[9], btVerticalRight_odd, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[10], btEdgeEmphasisLeft2, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[11], btEdgeEmphasisRight2, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[12], btVerticalLeft_even2, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[13], btVerticalRight_even2, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[14], btVerticalLeft_odd2, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[15], btVerticalRight_odd2, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[16], btEdgeEmphasisLeft3, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[17], btEdgeEmphasisRight3, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[18], btVerticalLeft_even3, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[19], btVerticalRight_even3, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[20], btVerticalLeft_odd3, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[21], btVerticalRight_odd3, IMAGE_DATA_LENGTH);

	//memcpy(computedImage[0], btOutputDisparity, IMAGE_DATA_LENGTH);
	////memcpy(computedImage[1], btOutputDisparity2, IMAGE_DATA_LENGTH);
	////memcpy(computedImage[2], btOutputDisparity3, IMAGE_DATA_LENGTH);
	////memcpy(computedImage[3], btOutputDisparityM, IMAGE_DATA_LENGTH);

	///*memcpy(computedImage[4], btDiagonally45Left_odd, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[5], btDiagonally45Right_odd, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[6], btDiagonally135Left_odd, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[7], btDiagonally135Right_odd, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[10], btDiagonally45Left_odd2, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[11], btDiagonally45Right_odd2, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[12], btDiagonally135Left_odd2, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[13], btDiagonally135Right_odd2, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[16], btDiagonally45Left_odd3, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[17], btDiagonally45Right_odd3, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[18], btDiagonally135Left_odd3, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[19], btDiagonally135Right_odd3, IMAGE_DATA_LENGTH);*/

	//memcpy(computedImage[1], btOutputDisparityOfDiagonally45M, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[2], btOutputDisparityOfDiagonally135M, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[2], btOutPutParfectDisparity, IMAGE_DATA_LENGTH);
	memcpy(computedImage[3], btOutPutParfectDisparityM, IMAGE_DATA_LENGTH);
	//memcpy(computedImage[1], btOutputDisparityM, IMAGE_DATA_LENGTH);




	/*FILE *fp;
	UINT OUTLINE = 72;
	INT	iArray[5] = { 1,4,8,12,15};
	if (uiPresentFr == 1) {
		fp = fopen("CheckAmountEner.csv", "w");
		if (fp == NULL) {
			printf("error\n");
			exit(1);
		}
		fprintf(fp, "1階層目\n");
		fprintf(fp, "-7,-4,0,+4,+7\n\n");
		

		for (x = 0; x < IMAGE_WIDTH; x++) {
			for (i = 0; i < 5; i++) {
				fprintf(fp, "%d,", iDisparityEnergy[x + IMAGE_WIDTH*OUTLINE][iArray[i]]);
			}
			fprintf(fp, "\t,");
			for (i = 0; i < 5; i++) {
				fprintf(fp, "%f,", dblL[x + IMAGE_WIDTH*OUTLINE][iArray[i]]);
			}
			fprintf(fp, "\t,");
			for (i = 0; i < 5; i++) {
				if (iL0[x + IMAGE_WIDTH*OUTLINE][iArray[i]] != 0) {
					fprintf(fp, "%f,", dblL[x + IMAGE_WIDTH*OUTLINE][iArray[i]] / iL0[x + IMAGE_WIDTH*OUTLINE][iArray[i]]);
				}
				else {
					fprintf(fp, "-,");
				}
			}
			fprintf(fp, "\t,");
			for (i = 0; i < 5; i++) {
				fprintf(fp, "%f,", dblS[x + IMAGE_WIDTH*OUTLINE][iArray[i]]);
			}
			fprintf(fp, "\t,");
			for (i = 0; i < 5; i++) {
				fprintf(fp, "%f,", dblR[x + IMAGE_WIDTH*OUTLINE][iArray[i]]);
			}
			fprintf(fp, "\t,");
			for (i = 0; i < 5; i++) {
				fprintf(fp, "%f,", dblT[x + IMAGE_WIDTH*OUTLINE][iArray[i]]);
			}
			fprintf(fp, "\n");
		}

		fclose(fp);

	}

	uiPresentFr++;*/


	//	INT		iLeft_SearchRange, iSub[17], iSubPow[17], iSubPowSum[17], iMin = 256 * 17, iMin_index;
	//
	//
	//
	//	int	   iSum[17], iSumPow[17], iSumPowSum[17], iMaxOfAdditionEvenAndOdd, iMaxOfAdditionEvenAndOdd_index;
	//
	//
	//
	//
	//
	//	
	//
	//	
	//
	//	
	//
	//	//cooperativeAlgorithm----------------------------------
	//
	//	INT		iColum, iDisparity, iRow, iColum2, iDisparity2, iRow2;
	//	DOUBLE	dblSumOfWithinPSI, dblSumOfWithinPSIComp;
	//	INT		PHI = 3, PSI = 17;
	//	DOUBLE	ALPHA = dblGpIn[0];
	//
	//	double	dblMaxOfAdditionEvenAndOdd;
	//
	//	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
	//		for (j = 0; j < 17; j++) {
	//			dblL[i][j] = 0;
	//			iL0[i][j] = 0;
	//			dblS[i][j] = 0;
	//			dblR[i][j] = 0;
	//			dblT[i][j] = 0;
	//
	//			dblLComp[i][j] = 0;
	//			dblSComp[i][j] = 0;
	//			dblRComp[i][j] = 0;
	//			dblTComp[i][j] = 0;
	//		}
	//	}
	//
	//	for (y = 0; y < IMAGE_HEIGHT; y++) {
	//		for (x = 8; x < IMAGE_WIDTH - 8; x++) {
	//
	//			//初期化
	//			for (i = 0; i <17; i++) {
	//				iDisparityEnergy[i] = 0;
	//			}
	//
	//			if ((pow(double(btVerticalLeft_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btVerticalLeft_odd[x + IMAGE_WIDTH * y] - 128), 2))>thresh5) {//物体の特徴が認識されるとき
	//
	//				for (iRight_SearchRange = -8; iRight_SearchRange <= 8; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)
	//
	//					//Even Part----------------
	//					iSumOfEven[iRight_SearchRange + 8] = (btVerticalLeft_even[(x + IMAGE_WIDTH*y)] - 128)
	//						+ (btVerticalRight_even[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);	//和を求める
	//					iSumPowOfEven[iRight_SearchRange + 8] = (int)pow((double)iSumOfEven[iRight_SearchRange + 8], 2);//2乗して負の値を除去
	//
	//					//Odd Part----------------
	//					iSumOfOdd[iRight_SearchRange + 8] = (btVerticalLeft_odd[(x + IMAGE_WIDTH*y)] - 128)
	//						+ (btVerticalRight_odd[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);	//和を求める
	//					iSumPowOfOdd[iRight_SearchRange + 8] = (int)pow((double)iSumOfOdd[iRight_SearchRange + 8], 2);	//2乗して負の値を除去
	//
	//				}
	//				for (i = 0; i < 17; i++) {
	//					iDisparityEnergy[i] = iSumPowOfEven[i] + iSumPowOfOdd[i];	//EvenとOddの加算
	//					
	//					iL0[x + IMAGE_WIDTH*y][i] = iDisparityEnergy[i]; //cooperative
	//					
	//					dblLComp[x + IMAGE_WIDTH*y][i] = (double)iDisparityEnergy[i]; //cooperative
	//				}
	//
	//				
	//			
	//			}
	//			else {
	//				btOutputDisparity[x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
	//			}
	//
	//		}
	//	}
	//
	//	BYTE	btEdgeEmphasisLeft2[IMAGE_DATA_LENGTH], btEdgeEmphasisRight2[IMAGE_DATA_LENGTH];
	//	BYTE	btVerticalLeft_even2[IMAGE_DATA_LENGTH], btVerticalRight_even2[IMAGE_DATA_LENGTH];
	//	BYTE	btVerticalLeft_odd2[IMAGE_DATA_LENGTH], btVerticalRight_odd2[IMAGE_DATA_LENGTH];
	//	BYTE	btOutputDisparity2[IMAGE_DATA_LENGTH], btOutputDisparityOfMulti[IMAGE_DATA_LENGTH];
	//
	//	//平滑化広げたバージョン
	//	UINT	Sy, Sx;
	//
	//	INT		iS_SummingOfNear, iS_NumOfData = 21;
	//	UINT	uiS_Sum = 0, uiS_Count = 0;
	//
	//	//左右輪郭強調---------------------------------------
	//	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
	//		btEdgeEmphasisLeft2[i] = max(min(pImage[2][0][i] - pImage[4][0][i] + 128, 255), 0);//左目輪郭強調
	//		btEdgeEmphasisRight2[i] = max(min(pImage[3][0][i] - pImage[5][0][i] + 128, 255), 0);//右目輪郭強調
	//	}
	//
	//	//左右縦強調(cos)----------------------------------------
	//	MakeBandPassFilter(btEdgeEmphasisLeft2, btVerticalLeft_even2, iS_NumOfData);
	//	MakeBandPassFilter(btEdgeEmphasisRight2, btVerticalRight_even2, iS_NumOfData);
	//
	//	//computedImageへ出力--------------------------------
	//	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
	//		/*computedImage[6][i] = btEdgeEmphasisLeft2[i];
	//		computedImage[7][i] = btEdgeEmphasisRight2[i];
	//		computedImage[8][i] = btVerticalLeft_even2[i];
	//		computedImage[9][i] = btVerticalRight_even2[i];*/
	//	}
	//
	//	//sin型縦要素強調画像作成
	//	for (Sy = 0; Sy < IMAGE_HEIGHT; Sy++) {
	//		for (Sx = 3; Sx < IMAGE_WIDTH - 3; Sx++) {
	//			btVerticalLeft_odd2[Sx + IMAGE_WIDTH*Sy] = max(min(btVerticalLeft_even2[(Sx + IMAGE_WIDTH*Sy) - 2] - btVerticalLeft_even2[(Sx + IMAGE_WIDTH*Sy) + 2] + 128, 255), 0);
	//			btVerticalRight_odd2[Sx + IMAGE_WIDTH*Sy] = max(min(btVerticalRight_even2[(Sx + IMAGE_WIDTH*Sy) - 2] - btVerticalRight_even2[(Sx + IMAGE_WIDTH*Sy) + 2] + 128, 255), 0);
	//		}
	//	}
	//
	//	//computedImageへ出力--------------------------------
	//	for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
	//		computedImage[10][i] = btVerticalLeft_odd2[i];
	//		computedImage[11][i] = btVerticalRight_odd2[i];
	//	}
	//
	//	//広い平滑化組み合わせ型単独点
	//
	//	//----------------------対応点の探索-------------------------------
	//
	//	for (y = 0; y < IMAGE_HEIGHT; y++) {
	//		for (x = 8+2; x < IMAGE_WIDTH -8 - 2; x++) {
	//
	//			for (i = 0; i <17; i++) {
	//				iDisparityEnergy[i] = 0;
	//			}
	//
	//			if ((pow(double(btVerticalLeft_even2[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btVerticalLeft_odd2[x + IMAGE_WIDTH * y] - 128), 2))>thresh6) {//物体の特徴が認識されるとき
	//
	//				for (iRight_SearchRange = -8; iRight_SearchRange <= 8; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)
	//
	//					//Even Part----------------
	//					iSumOfEven[iRight_SearchRange + 8] = (btVerticalLeft_even2[(x + IMAGE_WIDTH*y)] - 128)
	//							+ (btVerticalRight_even2[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
	//					iSumPowOfEven[iRight_SearchRange + 8] = (int)pow((double)iSumOfEven[iRight_SearchRange + 8], 2);	//2乗して負の値を除去
	//
	//					//Odd Part----------------
	//					iSumOfOdd[iRight_SearchRange + 8] = (btVerticalLeft_odd2[(x + IMAGE_WIDTH*y)] - 128)
	//						+ (btVerticalRight_odd2[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
	//					iSumPowOfOdd[iRight_SearchRange + 8] = (int)pow((double)iSumOfOdd[iRight_SearchRange + 8], 2);  //2乗して負の値を除去
	//					
	//				}
	//				for (i = 0; i < 17; i++) {
	//					iDisparityEnergy[i] = iSumPowOfEven[i] + iSumPowOfOdd[i];//EvenとOddの加算
	//					dblL[x + IMAGE_WIDTH*y][i] = (double)iDisparityEnergy[i];	//cooperative
	//				}
	//
	//				//最大値の更新
	//				iMaxOfAdditionEvenAndOdd = iDisparityEnergy[0];
	//				iMaxOfAdditionEvenAndOdd_index = 0;
	//
	//				for (i = 1; i < 17; i++) {
	//					if (iMaxOfAdditionEvenAndOdd < iDisparityEnergy[i]) {
	//						iMaxOfAdditionEvenAndOdd = iDisparityEnergy[i];
	//						iMaxOfAdditionEvenAndOdd_index = i;
	//					}
	//				}
	//
	//				btOutputDisparity2[x + IMAGE_WIDTH*y] = 256 - (8 * iMaxOfAdditionEvenAndOdd_index + 64);		//-8から8までの17点のときの出力式
	//
	//			}
	//			else {
	//				btOutputDisparity2[x + IMAGE_WIDTH*y] = 0;
	//			}
	//			//computedImageへ出力--------------------------------
	//			computedImage[9][x + IMAGE_WIDTH*y] = btOutputDisparity2[x + IMAGE_WIDTH*y];
	//
	//			//視差別出力
	//			/*for (i = 0; i < 17; i = i + 4) {
	//				computedImage[22 + i / 4][x + IMAGE_WIDTH*y] = min(255, (double)iDisparityEnergy[i] / 16384 * 255);
	//			}*/
	//		}
	//	}
	//	int uiIteration = 0;
	//	while (uiIteration < dblGpIn[1]) {
	//
	//		for (y = 1; y < IMAGE_HEIGHT - 1; y++) {
	//			for (x = 9; x < IMAGE_WIDTH - 9; x++) {
	//				for (i = 1; i < 16; i++) {
	//					for (iRow = -PHI / 2; iRow <= PHI / 2; iRow++) {
	//						for (iColum = -PHI / 2; iColum <= PHI / 2; iColum++) {
	//							for (iDisparity = -PHI / 2; iDisparity <= PHI / 2; iDisparity++) {
	//								dblS[x + IMAGE_WIDTH*y][i] += dblL[x + iColum + IMAGE_WIDTH * (y + iRow)][i + iDisparity];
	//								dblSComp[x + IMAGE_WIDTH*y][i] += dblLComp[x + iColum + IMAGE_WIDTH * (y + iRow)][i + iDisparity];
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//
	//		for (y = 1; y < IMAGE_HEIGHT-1; y++) {
	//			for (x = 16; x < IMAGE_WIDTH - 16; x++) {
	//				for (i = 0; i < 17; i++) {
	//
	//					dblSumOfWithinPSI = 0;
	//					dblSumOfWithinPSIComp = 0;
	//
	//					for (iDisparity2 = -PSI / 2; iDisparity2 <= PSI / 2; iDisparity2++) {
	//						for (iColum2 = -PSI / 2; iColum2 <= PSI / 2; iColum2++) {
	//							if (iColum2 == -iDisparity2 || iColum2 == 0) {
	//								if (iDisparity2 != 0) {
	//									dblSumOfWithinPSI += dblS[x + iColum2 + IMAGE_WIDTH * y][iDisparity2 + PSI / 2];
	//									dblSumOfWithinPSIComp += dblSComp[x + iColum2 + IMAGE_WIDTH * y][iDisparity2 + PSI / 2];
	//								}
	//							}
	//						}
	//					}
	//
	//					if (dblSumOfWithinPSI != 0) {
	//						dblR[x + IMAGE_WIDTH*y][i] = pow(dblS[x + IMAGE_WIDTH*y][i] / dblSumOfWithinPSI, ALPHA);
	//					}
	//					if (dblSumOfWithinPSIComp != 0) {
	//						dblRComp[x + IMAGE_WIDTH*y][i] = pow(dblSComp[x + IMAGE_WIDTH*y][i] / dblSumOfWithinPSIComp, ALPHA);
	//					}
	//
	//				}
	//
	//			}
	//		}
	//
	//		for (y = 0; y < IMAGE_HEIGHT; y++) {
	//			for (x = 8; x < IMAGE_WIDTH - 8; x++) {
	//				for (i = 0; i < 17; i++) {
	//					dblT[x + IMAGE_WIDTH*y][i] = iL0[x + IMAGE_WIDTH*y][i] * dblR[x + IMAGE_WIDTH*y][i];
	//					dblTComp[x + IMAGE_WIDTH*y][i] = iL0[x + IMAGE_WIDTH*y][i] * dblRComp[x + IMAGE_WIDTH*y][i];
	//				}
	//			}
	//		}
	//
	//		for (y = 0; y < IMAGE_HEIGHT; y++) {
	//			for (x = 8; x < IMAGE_WIDTH - 8; x++) {
	//				for (i = 0; i < 17; i++) {
	//					dblL[x + IMAGE_WIDTH*y][i] = dblT[x + IMAGE_WIDTH*y][i];
	//					dblLComp[x + IMAGE_WIDTH*y][i] = dblTComp[x + IMAGE_WIDTH*y][i];
	//					
	//				}
	//			}
	//		}
	//		uiIteration++;
	//	}
	//	
	//	//視差別出力
	//	for (y = 0; y < IMAGE_HEIGHT; y++) {
	//		for (x = 8; x < IMAGE_WIDTH - 8; x++) {
	//			uiCount = 0;
	//			for (i = 0; i < 17; i++) {
	//				if (i == 1 || i == 4 || i == 8 || i == 12 || i == 15) {
	//					computedImage[22 + uiCount][x + IMAGE_WIDTH*y] = (int)min(255, dblL[x + IMAGE_WIDTH*y][i] / 2048 * 255);
	//					computedImage[16 + uiCount][x + IMAGE_WIDTH*y] = (int)min(255, dblLComp[x + IMAGE_WIDTH*y][i] / 2048 * 255);
	//					uiCount++;
	//				}
	//			}
	//		}
	//	}
	//
	//	for (y = 0; y < IMAGE_HEIGHT; y++) {
	//		for (x = 16; x < IMAGE_WIDTH - 16; x++) {
	//
	//			if ((pow(double(btVerticalLeft_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btVerticalLeft_odd[x + IMAGE_WIDTH * y] - 128), 2)) > thresh5) {
	//				//最大値の更新----------------
	//				dblMaxOfAdditionEvenAndOdd = dblL[x + IMAGE_WIDTH*y][0];
	//				iMaxOfAdditionEvenAndOdd_index = 0;
	//
	//				for (i = 1; i < 17; i++) {
	//					if (dblMaxOfAdditionEvenAndOdd < dblL[x + IMAGE_WIDTH*y][i]) {
	//						dblMaxOfAdditionEvenAndOdd = dblL[x + IMAGE_WIDTH*y][i];
	//						iMaxOfAdditionEvenAndOdd_index = i;
	//					}
	//				}
	//
	//				//出力------------------------
	//				computedImage[14][x + IMAGE_WIDTH*y] = 256 - (8 * iMaxOfAdditionEvenAndOdd_index + 64);		//-8から8までの17点のときの出力式
	//			}
	//			else {
	//				computedImage[14][x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
	//			}
	//		}
	//	}
	//
	//
	//
	//	//for (y = 0; y < IMAGE_HEIGHT; y++) {
	//	//	for (x = 16; x < IMAGE_WIDTH - 16; x++) {
	//
	//	//		if ((pow(double(btVerticalLeft_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btVerticalLeft_odd[x + IMAGE_WIDTH * y] - 128), 2)) > thresh5) {
	//	//			//最大値の更新----------------
	//	//			dblMaxOfAdditionEvenAndOdd = dblLComp[x + IMAGE_WIDTH*y][0];
	//	//			iMaxOfAdditionEvenAndOdd_index = 0;
	//
	//	//			for (i = 1; i < 17; i++) {
	//	//				if (dblMaxOfAdditionEvenAndOdd < dblLComp[x + IMAGE_WIDTH*y][i]) {
	//	//					dblMaxOfAdditionEvenAndOdd = dblLComp[x + IMAGE_WIDTH*y][i];
	//	//					iMaxOfAdditionEvenAndOdd_index = i;
	//	//				}
	//	//			}
	//
	//	//			//出力------------------------
	//	//			computedImage[15][x + IMAGE_WIDTH*y] = 256 - (8 * iMaxOfAdditionEvenAndOdd_index + 64);		//-8から8までの17点のときの出力式
	//	//		}
	//	//		else {
	//	//			computedImage[15][x + IMAGE_WIDTH*y] = 0;	//背景のとき画素値0
	//	//		}
	//	//	}
	//	//}
	//
	//
	
//	
//
////まず普通に視差エネルギー求めてから対応点探索のときに範囲を限定する方法
//
//	for (y = 0; y < IMAGE_HEIGHT; y++) {
//		for (x = 8 + 2; x < IMAGE_WIDTH - 2 - 8; x++) {
//
//			for (i = 0; i < 17; i++) {
//				iSumPowOfEven[i] = 0;
//				iSumPowOfOdd[i] = 0;
//				iDisparityEnergy[i] = 0;
//			}
//
//			if ((pow(double(btVerticalLeft_even[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btVerticalLeft_odd[x + IMAGE_WIDTH * y] - 128), 2))>thresh5) {//物体の特徴が認識されるとき
//
//				for (iRight_SearchRange = -8; iRight_SearchRange <= 8; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)
//
//																							  //Even Part----------------
//					iSumOfEven[iRight_SearchRange + 8] = (btVerticalLeft_even[(x + IMAGE_WIDTH*y)] - 128)
//						+ (btVerticalRight_even[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
//					iSumPowOfEven[iRight_SearchRange + 8] = (int)pow((double)iSumOfEven[iRight_SearchRange + 8], 2);//2乗して負の値を除去
//
//																																				//Odd Part----------------
//					iSumOfOdd[iRight_SearchRange + 8] = (btVerticalLeft_odd[(x + IMAGE_WIDTH*y)] - 128)
//						+ (btVerticalRight_odd[(x + IMAGE_WIDTH*y) + iRight_SearchRange] - 128);												//和を求める
//					iSumPowOfOdd[iRight_SearchRange + 8] = (int)pow((double)iSumOfOdd[iRight_SearchRange + 8], 2);	//2乗して負の値を除去
//
//				}
//				for (i = 0; i < 17; i++) {
//					iDisparityEnergy[i] = iSumPowOfEven[i] + iSumPowOfOdd[i];	//EvenとOddの加算
//				}
//
//
//				if (btOutputDisparity2[x + IMAGE_WIDTH*y] != 0) {
//
//					if ((-(btOutputDisparity2[x + IMAGE_WIDTH*y] - 256) / 8 - 16) <= -3) {
//						//最大値の探索
//						iMaxOfAdditionEvenAndOdd = iDisparityEnergy[0];
//						iMaxOfAdditionEvenAndOdd_index = 0;
//
//						for (i = 1; i < 6; i++) {
//							if (iMaxOfAdditionEvenAndOdd < iDisparityEnergy[i]) {
//								iMaxOfAdditionEvenAndOdd = iDisparityEnergy[i];
//								iMaxOfAdditionEvenAndOdd_index = i;
//							}
//						}
//					}
//					else if ((-(btOutputDisparity2[x + IMAGE_WIDTH*y] - 256) / 8 - 16) <= 2) {
//						//最大値の探索
//						iMaxOfAdditionEvenAndOdd = iDisparityEnergy[6];
//						iMaxOfAdditionEvenAndOdd_index = 6;
//
//						for (i = 7; i < 11; i++) {
//							if (iMaxOfAdditionEvenAndOdd < iDisparityEnergy[i]) {
//								iMaxOfAdditionEvenAndOdd = iDisparityEnergy[i];
//								iMaxOfAdditionEvenAndOdd_index = i;
//							}
//						}
//					}
//					else if ((-(btOutputDisparity2[x + IMAGE_WIDTH*y] - 256) / 8 - 16) <= 8) {
//						//最大値の探索
//						iMaxOfAdditionEvenAndOdd = iDisparityEnergy[11];
//						iMaxOfAdditionEvenAndOdd_index = 11;
//
//						for (i = 12; i < 17; i++) {
//							if (iMaxOfAdditionEvenAndOdd < iDisparityEnergy[i]) {
//								iMaxOfAdditionEvenAndOdd = iDisparityEnergy[i];
//								iMaxOfAdditionEvenAndOdd_index = i;
//							}
//						}
//					}
//				}
//				else {
//					//最大値の更新
//					iMaxOfAdditionEvenAndOdd = iDisparityEnergy[0];
//					iMaxOfAdditionEvenAndOdd_index = 0;
//
//					for (i = 1; i < 17; i++) {
//						if (iMaxOfAdditionEvenAndOdd < iDisparityEnergy[i]) {
//							iMaxOfAdditionEvenAndOdd = iDisparityEnergy[i];
//							iMaxOfAdditionEvenAndOdd_index = i;
//						}
//					}
//
//				}
//
//				btOutputDisparityOfMulti[x + IMAGE_WIDTH*y] = 256 - (8 * iMaxOfAdditionEvenAndOdd_index + 64);
//
//			}
//			else {
//				btOutputDisparityOfMulti[x + IMAGE_WIDTH*y] = 0;
//
//			}
//			//computedImageへ出力--------------------------------
//			computedImage[12][x + IMAGE_WIDTH*y] = btOutputDisparityOfMulti[x + IMAGE_WIDTH*y];
//
//		}
//	}
//

//カラーバー表示
/*for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
computedImage[1][i] = 0;
}*/


//int y, x, iMaxOfAdditionEvenAndOdd_index;
/*iMaxOfAdditionEvenAndOdd_index = 8;
for (y = 0; y < IMAGE_HEIGHT; y++) {
if (y != 0 && y % 7 == 0) {
iMaxOfAdditionEvenAndOdd_index--;
}
for (x = 140; x < IMAGE_WIDTH; x++) {
computedImage[1][x + IMAGE_WIDTH*y] = 12 * iMaxOfAdditionEvenAndOdd_index + 8;
}
}*/

//for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
//computedImage[1][i] = 0;
//}
//
//int iColor = 240;
//for (y = 0; y < IMAGE_HEIGHT; y++) {
//	for (x = 140; x < IMAGE_WIDTH; x++) {
//		
//		computedImage[2][x + IMAGE_WIDTH*y] = iColor;
//		
//	}
//	iColor -= 2;
//}

////差の2乗の総和アルゴリズム用
//for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
//	computedImage[16][i] = pImage[0][0][i];
//	computedImage[17][i] = pImage[1][0][i];
//}
////ClassicDisparity(btEdgeEmphasisLeft, btEdgeEmphasisRight, computedImage[7], 17);
////ClassicDisparity(btVerticalLeft_even, btVerticalRight_even, computedImage[8], 17);
//////MaxOfSumDisparity(btVerticalLeft_even, btVerticalRight_even, computedImage[15], 17);
//NormalClassicDisparity(computedImage[16], computedImage[17], computedImage[1], 17);
//
//int iDifference, iSum = 0, iCount = 0;
//double dblEvaluationValue;
//
//for (y = 1; y < IMAGE_HEIGHT - 1; y++) {
//	for (x = NUM_OF_PIXEL; x < IMAGE_WIDTH - (NUM_OF_PIXEL); x++) {
//		if (computedImage[1][x + IMAGE_WIDTH * y] != 0) {
//			iDifference = 8 - ((256 - computedImage[1][x + IMAGE_WIDTH * y] - 64) / 8);
//			iSum += pow(iDifference, 2);
//			iCount++;
//		}
//	}
//}
//
//dblGpOut[4] = iCount;//(double)iSum / iCount;
//
//iSum = 0;
//iCount = 0;
//
//for (y = 1; y < IMAGE_HEIGHT - 1; y++) {
//	for (x = NUM_OF_PIXEL; x < IMAGE_WIDTH - (NUM_OF_PIXEL); x++) {
//		if (computedImage[0][x + IMAGE_WIDTH * y] != 0) {
//			iDifference = 8 - ((256 - computedImage[0][x + IMAGE_WIDTH * y] - 64) / 8);
//			iSum += pow(iDifference, 2);
//			iCount++;
//		}
//	}
//}
//
//dblGpOut[5] = iCount;//(double)iSum/ iCount;
//
//iSum = 0;
//iCount = 0;
//
//for (y = 1; y < IMAGE_HEIGHT - 1; y++) {
//	for (x = NUM_OF_PIXEL; x < IMAGE_WIDTH - (NUM_OF_PIXEL); x++) {
//		if (computedImage[3][x + IMAGE_WIDTH * y] != 0) {
//			iDifference = 8 - ((256 - computedImage[3][x + IMAGE_WIDTH * y] - 64) / 8);
//			iSum += pow(iDifference, 2);
//			iCount++;
//		}
//	}
//}
//
//dblGpOut[6] = iCount;// (double)iSum / iCount;
//
//iSum = 0;
//iCount = 0;
//
//for (y = 1; y < IMAGE_HEIGHT - 1; y++) {
//	for (x = NUM_OF_PIXEL; x < IMAGE_WIDTH - (NUM_OF_PIXEL); x++) {
//		if (computedImage[10][x + IMAGE_WIDTH * y] != 0) {
//			iDifference = 8 - ((256 - computedImage[10][x + IMAGE_WIDTH * y] - 64) / 8);
//			iSum += pow(iDifference, 2);
//			iCount++;
//		}
//	}
//}
//
//dblGpOut[7] = iCount;//(double)iSum / iCount;

//ここまで--------------------------------------------
	//ここまで--------------------------------------------


	//BYTE* btNarrowImage;
	//BYTE* btWideImage;
	//BYTE btVerticalLeft[19200], btVerticalRight[19200];

	//btNarrowImage = pImage[6][0];
	//btWideImage = pImage[2][0];
	//for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
	//	computedImage[0][i] = max(0, min(255, btNarrowImage[i] - btWideImage[i] + 128));
	//}

	//btNarrowImage = pImage[7][0];
	//btWideImage = pImage[3][0];
	//for (i = 0; i < IMAGE_DATA_LENGTH; i++) {
	//	computedImage[1][i] = max(0, min(255, btNarrowImage[i] - btWideImage[i] + 128));
	//}

	//UINT uiHalfOfVertPix = 7;
	//UINT uiRefPixel;
	//UINT uiTempLeft, uiTempRight;
	//
	//ZeroMemory(btVerticalLeft, IMAGE_DATA_LENGTH);
	//ZeroMemory(btVerticalRight, IMAGE_DATA_LENGTH);

	//for (i = uiHalfOfVertPix; i < IMAGE_HEIGHT - uiHalfOfVertPix; i++) {
	//	for (j = 0; j < IMAGE_WIDTH; j++) {
	//		uiRefPixel = j + i * IMAGE_WIDTH;
	//		uiTempLeft = 0;
	//		uiTempRight = 0;
	//		for (k = 0; k < uiHalfOfVertPix * 2 + 1; k++) {
	//			uiTempLeft += computedImage[0][uiRefPixel + (k - uiHalfOfVertPix) * IMAGE_WIDTH];
	//			uiTempRight += computedImage[1][uiRefPixel + (k - uiHalfOfVertPix) * IMAGE_WIDTH];
	//		}
	//		btVerticalLeft[uiRefPixel] = uiTempLeft / (uiHalfOfVertPix * 2 + 1);
	//		btVerticalRight[uiRefPixel] = uiTempRight / (uiHalfOfVertPix * 2 + 1);
	//	}
	//}
	////memcpy(computedImage[2], btVerticalLeft, IMAGE_DATA_LENGTH);
	////memcpy(computedImage[3], btVerticalRight, IMAGE_DATA_LENGTH);

	//UINT uiDetectThresh = 12;
	//double dblSqDifference, dblMin;

	//UINT uiDetectRange = 7;

	//BYTE* btLeftInput = btVerticalLeft;
	//BYTE* btRightInput = btVerticalRight;

	//ZeroMemory(computedImage[2], IMAGE_DATA_LENGTH);

	//for (i = uiHalfOfVertPix; i < IMAGE_HEIGHT - uiHalfOfVertPix; i++) {
	//	for (j = 12; j < IMAGE_WIDTH - 12; j++) {
	//		uiRefPixel = j + i * IMAGE_WIDTH;
	//		if (btLeftInput[uiRefPixel] > 128 + uiDetectThresh
	//			|| btLeftInput[uiRefPixel] < 128 - uiDetectThresh) {
	//			dblMin = 300;
	//			for (k = 0; k <= uiDetectRange * 2; k++) {
	//				dblSqDifference = 0;
	//				for(l = -1; l < 2; l++){
	//					dblSqDifference += pow(btLeftInput[uiRefPixel + l * 5] - btRightInput[uiRefPixel + k - uiDetectRange + l * 5], 2);
	//				}
	//				if (dblSqDifference < dblMin) {
	//					dblMin = dblSqDifference;
	//					computedImage[2][uiRefPixel] = 16 * (k + 8 - uiDetectRange);
	//				}
	//			}
	//		}
	//	}
	//}

	int iAngle[4];
	
	for (i = 0; i < 4; i++) {
		if (pAppendData[8 + i] > 32767) {
			iAngle[i] = pAppendData[8 + i] - 65536;
		} else {
			iAngle[i] = pAppendData[8 + i];
		}
		dblGpOut[i] = (double)iAngle[i]/10;
	}
	dwStop = timeGetTime();	//時間計測
	dwInterval = dwStop - dwStart;	//1Frにかかる時間
	/*dblGpOut[5] = dwStart;
	dblGpOut[6] = dwStop;
	dblGpOut[7] = dwInterval;*/
}

/******************************************************************************
 * Procedure for Color Check
 ******************************************************************************/
VOID ColorCheck(BYTE** ppImage, UINT* pAppendData)
{
	UINT i ,j;
	BYTE* pImage[MAX_NUM_OF_RECV_IMG][FRAMES_TO_PROC];

	for(i = 0; i < MAX_NUM_OF_RECV_IMG; i ++){
		for(j = 0; j < FRAMES_TO_PROC; j ++ ){
			pImage[i][j] = *ppImage;
			ppImage ++;
		}
	}

	/* ------------------------------------------------------------------
	/	pImage[i][j]	: Image of image number [i], [j] frames in the past.
	/  ------------------------------------------------------------------ */

	// Write your program here ------------------------------------------------

	for(i = 0; i < IMAGE_DATA_LENGTH; i ++){
		if(i % 128 < 13){
			computedImage[0][i] = 0;
		} else if(i %128 < 26){
			computedImage[0][i] = 32;
		} else if(i %128 < 39){
			computedImage[0][i] = 80;
		} else if(i %128 < 52){
			computedImage[0][i] = 96;
		} else if(i %128 < 65){
			computedImage[0][i] = 128;
		} else if(i %128 < 78){
			computedImage[0][i] = 160;
		} else if(i %128 < 91){
			computedImage[0][i] = 176;
		} else if(i %128 < 104){
			computedImage[0][i] = 192;
		} else if(i %128 < 117){
			computedImage[0][i] = 224;
		} else {
			computedImage[0][i] = 255;
		} 
		//computedImage[0][i] = i % 128 * 2;
	}
	// ------------------------------------------------------------------------
}

/******************************************************************************
 * Compute Average
 ******************************************************************************/
UINT ComputeAverage(BYTE* pImage)
{
	UINT i, uiSum;

	uiSum = 0;
	
	for(i = 0; i < IMAGE_DATA_LENGTH; i ++){
		uiSum += (UINT)pImage[i];
	}

	return (UINT)(uiSum / IMAGE_DATA_LENGTH);
}
