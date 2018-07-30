#include"ClassicDisparity.h"
#include"math.h"

VOID ClassicDisparity(BYTE *btLeft, BYTE *btRight, BYTE *btOutput, INT iNumOfPixel) {
	//変数定義
	UINT	y, x, i;


	INT		iLeft_SearchRange, iRight_SearchRange, iSub[17], iSubPow[17], iSubPowSum[17], iMin = 256 * 17, iMin_index;

	int thresh1 = dblGpIn[1], thresh2 = dblGpIn[2], thresh3 = dblGpIn[3], thresh4 = dblGpIn[4];


	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = iNumOfPixel/2; x < IMAGE_WIDTH - iNumOfPixel / 2; x++) {

			if (btLeft[x + IMAGE_WIDTH * y] < 128 - 8 || btLeft[x + IMAGE_WIDTH * y]>128 + 8) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -iNumOfPixel / 2; iRight_SearchRange <= iNumOfPixel / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)
					for (iLeft_SearchRange = 0; iLeft_SearchRange < 5; iLeft_SearchRange++) {//左目の探索エリア(5点)
						iSub[iLeft_SearchRange] = btLeft[(x + IMAGE_WIDTH*y) + iLeft_SearchRange] - btRight[(x + IMAGE_WIDTH*y) + iLeft_SearchRange + iRight_SearchRange];//差を求める
						iSubPow[iLeft_SearchRange] = (int)pow((double)iSub[iLeft_SearchRange], 2);//差の2乗を求める
						iSubPowSum[iRight_SearchRange + iNumOfPixel / 2] = iSubPowSum[iRight_SearchRange + iNumOfPixel / 2] + iSubPow[iLeft_SearchRange];//総和を求める
					}
				}




				//最小値の更新
				iMin = iSubPowSum[0];
				iMin_index = 0;

				for (i = 1; i < iNumOfPixel; i++) {
					if (iMin > iSubPowSum[i]) {
						iMin = iSubPowSum[i];
						iMin_index = i;
					}
				}

				////デバッグ用//
				//dblGpOut[6] = iMin;
				////デバッグ用//
				//dblGpOut[7] = iMin_index;

				//視差表示
				btOutput[x + IMAGE_WIDTH*y] = 256 - (8 * iMin_index + 64);		//-8から8までの17点のときの出力式

				//初期化
				for (i = 0; i < iNumOfPixel; i++) {
					iSubPowSum[i] = 0;
				}
			}
			else {
				btOutput[x + IMAGE_WIDTH*y] = 0;
			}
		}
	}
}

VOID MaxOfSumDisparity(BYTE *btLeft, BYTE *btRight, BYTE *btOutput, INT iNumOfPixel) {

	UINT	y, x, i;
	int thresh1 = dblGpIn[1], thresh2 = dblGpIn[2], thresh3 = dblGpIn[3], thresh4 = dblGpIn[4], thresh5 = dblGpIn[5], thresh6 = dblGpIn[6];

	int	   iLeft_SearchRange, iRight_SearchRange, iSum[17], iSumPow[17], iSumPowSum[17], iMax = 0, iMax_index;

	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = 8; x < IMAGE_WIDTH-8; x++) {

			if (btLeft[x + IMAGE_WIDTH * y] < 128 - 8 || btLeft[x + IMAGE_WIDTH * y]>128 + 8) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -8; iRight_SearchRange <= 8; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)
					for (iLeft_SearchRange = 0; iLeft_SearchRange < 5; iLeft_SearchRange++) {//左目の探索エリア(5点)
						iSum[iLeft_SearchRange] = (btLeft[(x + IMAGE_WIDTH*y) + iLeft_SearchRange] - 128)
							+ (btRight[(x + IMAGE_WIDTH*y) + iLeft_SearchRange + iRight_SearchRange] - 128);//和を求める
						iSumPow[iLeft_SearchRange] = (int)pow((double)iSum[iLeft_SearchRange], 2);//2乗して負の値を除去
						iSumPowSum[iRight_SearchRange + 8] += iSumPow[iLeft_SearchRange];//総和を求める
					}
				}

				//最大値の更新
				iMax = iSumPowSum[0];
				iMax_index = 0;

				for (i = 1; i < 17; i++) {
					if (iMax < iSumPowSum[i]) {
						iMax = iSumPowSum[i];
						iMax_index = i;
					}
				}

				btOutput[x + IMAGE_WIDTH*y] = 256 - (8 * iMax_index + 64);//-8から8までの17点のときの出力式
														 
				//初期化
				for (i = 0; i < 17; i++) {
					iSumPowSum[i] = 0;
				}
			}
			else {
				btOutput[x + IMAGE_WIDTH*y] = 0;
			}
		}
	}
}

VOID NormalClassicDisparity(BYTE *btLeft, BYTE *btRight, BYTE *btOutput, INT iNumOfPixel) {
	//変数定義
	UINT	y, x, i;


	INT		iLeft_SearchRange, iRight_SearchRange, iSub[17], iSubPow[17], iSubPowSum[17], iMin = 256 * 17, iMin_index;

	int thresh1 = dblGpIn[1], thresh2 = dblGpIn[2], thresh3 = dblGpIn[3], thresh4 = dblGpIn[4];


	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = iNumOfPixel / 2; x < IMAGE_WIDTH -1  - iNumOfPixel / 2; x++) {

			if (abs(btLeft[x + IMAGE_WIDTH * y] - btLeft[x + 1 + IMAGE_WIDTH * y])>8) {//物体の特徴が認識されるとき

				for (iRight_SearchRange = -iNumOfPixel / 2; iRight_SearchRange <= iNumOfPixel / 2; iRight_SearchRange++) {//右目の探索エリア(左より-8ズレたときから右8ズレたときまで)
					for (iLeft_SearchRange = 0; iLeft_SearchRange < 5; iLeft_SearchRange++) {//左目の探索エリア(5点)
						iSub[iLeft_SearchRange] = btLeft[(x + IMAGE_WIDTH*y) + iLeft_SearchRange] - btRight[(x + IMAGE_WIDTH*y) + iLeft_SearchRange + iRight_SearchRange];//差を求める
						iSubPow[iLeft_SearchRange] = (int)pow((double)iSub[iLeft_SearchRange], 2);//差の2乗を求める
						iSubPowSum[iRight_SearchRange + iNumOfPixel / 2] = iSubPowSum[iRight_SearchRange + iNumOfPixel / 2] + iSubPow[iLeft_SearchRange];//総和を求める
					}
				}




				//最小値の更新
				iMin = iSubPowSum[0];
				iMin_index = 0;

				for (i = 1; i < iNumOfPixel; i++) {
					if (iMin > iSubPowSum[i]) {
						iMin = iSubPowSum[i];
						iMin_index = i;
					}
				}

				////デバッグ用//
				//dblGpOut[6] = iMin;
				////デバッグ用//
				//dblGpOut[7] = iMin_index;

				btOutput[x + IMAGE_WIDTH*y] = 256 - (8 * iMin_index + 64);//-8から8までの17点のときの出力式

				//初期化
				for (i = 0; i < iNumOfPixel; i++) {
					iSubPowSum[i] = 0;
				}
			}
			else {
				btOutput[x + IMAGE_WIDTH*y] = 0;
			}
		}
	}
}