#include"GetBinoEnergy.h"
#include"math.h"

INT GetBinoEnergy(BYTE *btLeftEven, BYTE *btRightEven, BYTE *btLeftOdd, BYTE *btRightOdd, INT iShiftAmount, INT iNumOfPixel, INT thresh) {

	int x, y, i;

	int iRight_SearchRange;
	int iSumOfEven = 0, iSumPowOfEven = 0, iSumOfOdd = 0, iSumPowOfOdd = 0, iDisparityEnergy = 0;

	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = iNumOfPixel / 2; x < IMAGE_WIDTH - (iNumOfPixel / 2); x++) {


			if ((pow(double(btLeftEven[x + IMAGE_WIDTH * y] - 128), 2) + pow(double(btLeftOdd[x + IMAGE_WIDTH * y] - 128), 2)) > thresh) {//•¨‘Ì‚Ì“Á’¥‚ª”F¯‚³‚ê‚é‚Æ‚«

					//Even Part----------------
				iSumOfEven = (btLeftEven[(x + IMAGE_WIDTH * y)] - 128) + (btRightEven[(x + IMAGE_WIDTH * y) + iShiftAmount] - 128);	//˜a‚ğ‹‚ß‚é
				iSumPowOfEven = (int)pow((double)iSumOfEven, 2);//2æ‚µ‚Ä•‰‚Ì’l‚ğœ‹

				 //Odd Part----------------
				iSumOfOdd = (btLeftOdd[(x + IMAGE_WIDTH*y)] - 128) + (btRightOdd[(x + IMAGE_WIDTH*y) + iShiftAmount] - 128);												//˜a‚ğ‹‚ß‚é
				iSumPowOfOdd = (int)pow((double)iSumOfOdd, 2);	//2æ‚µ‚Ä•‰‚Ì’l‚ğœ‹
				
			
		
			}
		}
	}
	return iSumPowOfEven + iSumPowOfOdd;	//Even‚ÆOdd‚Ì‰ÁZ
}