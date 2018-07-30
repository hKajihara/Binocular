#include"GetBinoEnergy.h"
#include"math.h"

INT GetBinoEnergy(BYTE *btLeftEven, BYTE *btRightEven, BYTE *btLeftOdd, BYTE *btRightOdd, INT iShiftAmount, INT iNumOfPixel, INT thresh) {

	int x, y, i;

	int iRight_SearchRange;
	int iSumOfEven = 0, iSumPowOfEven = 0, iSumOfOdd = 0, iSumPowOfOdd = 0, iDisparityEnergy = 0;

	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = iNumOfPixel / 2; x < IMAGE_WIDTH - (iNumOfPixel / 2); x++) {


			if ((pow(double(btLeftEven[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btLeftOdd[x + IMAGE_WIDTH * y] - 128), 2)) > thresh) {//���̂̓������F�������Ƃ�

					//Even Part----------------
				iSumOfEven = (btLeftEven[(x + IMAGE_WIDTH * y)] - 128) + (btRightEven[(x + IMAGE_WIDTH * y) + iShiftAmount] - 128);	//�a�����߂�
				iSumPowOfEven = (int)pow((double)iSumOfEven, 2);//2�悵�ĕ��̒l������

				 //Odd Part----------------
				iSumOfOdd = (btLeftOdd[(x + IMAGE_WIDTH*y)] - 128) + (btRightOdd[(x + IMAGE_WIDTH*y) + iShiftAmount] - 128);												//�a�����߂�
				iSumPowOfOdd = (int)pow((double)iSumOfOdd, 2);	//2�悵�ĕ��̒l������
				
			
		
			}
		}
	}
	return iSumPowOfEven + iSumPowOfOdd;	//Even��Odd�̉��Z
}