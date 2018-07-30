#include"GetBinoEnergy.h"
#include"math.h"

INT GetBinoEnergy(BYTE *btLeftEven, BYTE *btRightEven, BYTE *btLeftOdd, BYTE *btRightOdd, INT iShiftAmount, INT iNumOfPixel, INT thresh) {

	int x, y, i;

	int iRight_SearchRange;
	int iSumOfEven = 0, iSumPowOfEven = 0, iSumOfOdd = 0, iSumPowOfOdd = 0, iDisparityEnergy = 0;

	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = iNumOfPixel / 2; x < IMAGE_WIDTH - (iNumOfPixel / 2); x++) {


			if ((pow(double(btLeftEven[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btLeftOdd[x + IMAGE_WIDTH * y] - 128), 2)) > thresh) {//物体の特徴が認識されるとき

					//Even Part----------------
				iSumOfEven = (btLeftEven[(x + IMAGE_WIDTH * y)] - 128) + (btRightEven[(x + IMAGE_WIDTH * y) + iShiftAmount] - 128);	//和を求める
				iSumPowOfEven = (int)pow((double)iSumOfEven, 2);//2乗して負の値を除去

				 //Odd Part----------------
				iSumOfOdd = (btLeftOdd[(x + IMAGE_WIDTH*y)] - 128) + (btRightOdd[(x + IMAGE_WIDTH*y) + iShiftAmount] - 128);												//和を求める
				iSumPowOfOdd = (int)pow((double)iSumOfOdd, 2);	//2乗して負の値を除去
				
			
		
			}
		}
	}
	return iSumPowOfEven + iSumPowOfOdd;	//EvenとOddの加算
}