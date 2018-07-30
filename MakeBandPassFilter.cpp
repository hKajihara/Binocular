#include"MakeBandPassFilter.h"

#define VALUE_OF_PARATEAU 16

VOID MakeBandPassFilter(BYTE *btInput, BYTE *btOutput, INT iNumOfData) {
	INT	y, x;

	INT	iSummingOfNear;
	INT	uiSum = 0, uiCountOutOfBounds = 0;

	BYTE btConventionalVert[IMAGE_DATA_LENGTH];
	//INT ValueOfParateau = 16;

	//coså^ècóvëfã≠í≤âÊëúçÏê¨
	//----------------------ç∂ñ⁄ècóvëfã≠í≤------------------------------	
	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = 0; x < IMAGE_WIDTH; x++) {
			for (iSummingOfNear = -iNumOfData / 2; iSummingOfNear <= iNumOfData / 2; iSummingOfNear++) {//ë´ÇµçûÇ›îÕàÕÇÃéwíË
				if (x + ((IMAGE_WIDTH * y) + (iSummingOfNear * IMAGE_WIDTH)) < 0) {
					uiCountOutOfBounds++;
				}
				else if (x + ((IMAGE_WIDTH * y) + (iSummingOfNear * IMAGE_WIDTH)) >= IMAGE_DATA_LENGTH) {
					uiSum += btInput[x + (IMAGE_WIDTH * (IMAGE_HEIGHT - 1))];//ë´ÇµçûÇ›
				}
				else {
					uiSum += btInput[x + (IMAGE_WIDTH * (y + iSummingOfNear))] * (uiCountOutOfBounds + 1);//ë´ÇµçûÇ›
					uiCountOutOfBounds = 0;
				}
				
			}

			btConventionalVert[x + (IMAGE_WIDTH * y)] = uiSum / iNumOfData;//ë´ÇµçûÇÒÇæílÇÃïΩãœílÇèoóÕ
			uiSum = 0;

			if (btConventionalVert[x + (IMAGE_WIDTH * y)] > 128 + VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 + VALUE_OF_PARATEAU;
			}
			else if (btConventionalVert[x + (IMAGE_WIDTH * y)] < 128 - VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 - VALUE_OF_PARATEAU;
			}
			else {
				btOutput[x + (IMAGE_WIDTH * y)] = btConventionalVert[x + (IMAGE_WIDTH * y)];
			}
			
		}
	}
}

VOID MakeDiagonallyEdgeEmphasis45(BYTE *btInput, BYTE *btOutput, INT iNumOfData) {
	INT	y, x;

	INT		iSummingOfNear;
	INT	uiSum = 0, uiCountOutOfBounds = 0;
	INT iValueOfLastPixel=0;

	BYTE btConventional45[IMAGE_DATA_LENGTH];
	INT ValueOfParateau = 16;

	//coså^Ç»Ç»Çﬂ45óvëfã≠í≤âÊëúçÏê¨
	//----------------------ç∂ñ⁄Ç»Ç»Çﬂ45óvëfã≠í≤------------------------------	
	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = 0; x < IMAGE_WIDTH; x++) {
			for (iSummingOfNear = -iNumOfData / 2; iSummingOfNear <= iNumOfData / 2; iSummingOfNear++) {//ë´ÇµçûÇ›îÕàÕÇÃéwíË
				if ((x + ((IMAGE_WIDTH * y) + (iSummingOfNear * IMAGE_WIDTH)) - iSummingOfNear < 0) || (x - iSummingOfNear >= IMAGE_WIDTH)) {//è„Ç‹ÇΩÇÕâEÇ…ÇÕÇ›èoÇµÇΩÇ∆Ç´
					uiCountOutOfBounds++;
					
				}
				else if ((x + ((IMAGE_WIDTH * y) + (iSummingOfNear * IMAGE_WIDTH)) - iSummingOfNear >= IMAGE_DATA_LENGTH) || (x - iSummingOfNear < 0)) {
					uiSum += iValueOfLastPixel;//ë´ÇµçûÇ›
				}
				else {
					uiSum += btInput[x + (IMAGE_WIDTH * (y + iSummingOfNear)) - iSummingOfNear] * (uiCountOutOfBounds + 1);//ë´ÇµçûÇ›
					iValueOfLastPixel = btInput[x + (IMAGE_WIDTH * (y + iSummingOfNear)) - iSummingOfNear];//éüÇÕÇ›èoÇµÇΩÇ∆Ç´ÇÃÇΩÇﬂÇ…åªç›ÇÃâÊëfíläiî[
					uiCountOutOfBounds = 0;
				}

			}
			btConventional45[x + (IMAGE_WIDTH * y)] = uiSum / iNumOfData;//ë´ÇµçûÇÒÇæílÇÃïΩãœílÇèoóÕ
			uiSum = 0;

			if (btConventional45[x + (IMAGE_WIDTH * y)] > 128 + VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 + VALUE_OF_PARATEAU;
			}
			else if (btConventional45[x + (IMAGE_WIDTH * y)] < 128 - VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 - VALUE_OF_PARATEAU;
			}
			else {
				btOutput[x + (IMAGE_WIDTH * y)] = btConventional45[x + (IMAGE_WIDTH * y)];
			}
		}
	}
}

VOID MakeDiagonallyEdgeEmphasis135(BYTE *btInput, BYTE *btOutput, INT iNumOfData) {
	INT	y, x;

	INT		iSummingOfNear;
	INT	uiSum = 0, uiCountOutOfBounds = 0;
	INT iValueOfLastPixel = 0;

	BYTE btConventional135[IMAGE_DATA_LENGTH];
	INT ValueOfParateau = 16;

	//coså^Ç»Ç»Çﬂ135óvëfã≠í≤âÊëúçÏê¨
	//----------------------ç∂ñ⁄Ç»Ç»Çﬂ135óvëfã≠í≤------------------------------	
	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = 0; x < IMAGE_WIDTH; x++) {
			for (iSummingOfNear = -iNumOfData / 2; iSummingOfNear <= iNumOfData / 2; iSummingOfNear++) {//ë´ÇµçûÇ›îÕàÕÇÃéwíË
				if ((x + ((IMAGE_WIDTH * y) + (iSummingOfNear * IMAGE_WIDTH)) + iSummingOfNear < 0) || (x + iSummingOfNear < 0)) {//è„Ç‹ÇΩÇÕç∂Ç…ÇÕÇ›èoÇµÇΩÇ∆Ç´
					uiCountOutOfBounds++;

				}
				else if ((x + ((IMAGE_WIDTH * y) + (iSummingOfNear * IMAGE_WIDTH)) + iSummingOfNear >= IMAGE_DATA_LENGTH) || (x + iSummingOfNear >= IMAGE_WIDTH)) {
					uiSum += iValueOfLastPixel;//ë´ÇµçûÇ›
				}
				else {
					uiSum += btInput[x + (IMAGE_WIDTH * (y + iSummingOfNear)) + iSummingOfNear] * (uiCountOutOfBounds + 1);//ë´ÇµçûÇ›
					iValueOfLastPixel = btInput[x + (IMAGE_WIDTH * (y + iSummingOfNear)) + iSummingOfNear];//éüÇÕÇ›èoÇµÇΩÇ∆Ç´ÇÃÇΩÇﬂÇ…åªç›ÇÃâÊëfíläiî[
					uiCountOutOfBounds = 0;
				}

			}
			btConventional135[x + (IMAGE_WIDTH * y)] = uiSum / iNumOfData;//ë´ÇµçûÇÒÇæílÇÃïΩãœílÇèoóÕ
			uiSum = 0;

			if (btConventional135[x + (IMAGE_WIDTH * y)] > 128 + VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 + VALUE_OF_PARATEAU;
			}
			else if (btConventional135[x + (IMAGE_WIDTH * y)] < 128 - VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 - VALUE_OF_PARATEAU;
			}
			else {
				btOutput[x + (IMAGE_WIDTH * y)] = btConventional135[x + (IMAGE_WIDTH * y)];
			}
			
		}
	}
}

VOID MakeVerticalOdd(BYTE *btInput, BYTE *btOutput, INT iDistanceAway) {
	INT	y, x;

	BYTE btConventionalOddVert[IMAGE_DATA_LENGTH];

	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = 0; x < IMAGE_WIDTH ; x++) {

			if (x < iDistanceAway) {
				btConventionalOddVert[x + IMAGE_WIDTH*y] = max(min(btInput[IMAGE_WIDTH*y] - btInput[(x + IMAGE_WIDTH*y) + iDistanceAway] + 128, 255), 0);
			}
			else if (x >= IMAGE_WIDTH - iDistanceAway) {
				btConventionalOddVert[x + IMAGE_WIDTH*y] = max(min(btInput[(x + IMAGE_WIDTH*y) - iDistanceAway] - btInput[(IMAGE_WIDTH-1) + IMAGE_WIDTH*y] + 128, 255), 0);
			}
			else {
				btConventionalOddVert[x + IMAGE_WIDTH*y] = max(min(btInput[(x + IMAGE_WIDTH*y) - iDistanceAway] - btInput[(x + IMAGE_WIDTH*y) + iDistanceAway] + 128, 255), 0);
			}

			if (btConventionalOddVert[x + (IMAGE_WIDTH * y)] > 128 + VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 + VALUE_OF_PARATEAU;
			}
			else if (btConventionalOddVert[x + (IMAGE_WIDTH * y)] < 128 - VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 - VALUE_OF_PARATEAU;
			}
			else {
				btOutput[x + (IMAGE_WIDTH * y)] = btConventionalOddVert[x + (IMAGE_WIDTH * y)];
			}

		}
	}
}

VOID MakeDiagonallyOdd45(BYTE *btInput, BYTE *btOutput, INT iDistanceAway) {
	INT	y, x;
	INT iPx1, iPx2;

	BYTE btConventionalOdd45[IMAGE_DATA_LENGTH];

	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = 0; x < IMAGE_WIDTH; x++) {

			if (x < iDistanceAway) {
				iPx1 = btInput[IMAGE_WIDTH*(y-(iDistanceAway-1))];
			}
			else if (y < iDistanceAway) {
				iPx1 = btInput[(x-iDistanceAway+1) + IMAGE_WIDTH*(y - (iDistanceAway - 1))];
			}
			else{
				iPx1 = btInput[x + (IMAGE_WIDTH*(y - iDistanceAway)) - iDistanceAway];
			}

			if (x >= IMAGE_WIDTH - iDistanceAway) {
				iPx2 = btInput[(IMAGE_WIDTH-1) + IMAGE_WIDTH*(y + (iDistanceAway - 1))];
			}
			else if (y >= IMAGE_HEIGHT - iDistanceAway) {
				iPx2 = btInput[(x + iDistanceAway - 1) + IMAGE_WIDTH*(IMAGE_HEIGHT-1)];
			}
			else {
				iPx2 = btInput[x + (IMAGE_WIDTH*(y + iDistanceAway)) + iDistanceAway];
			}

			btConventionalOdd45[x + IMAGE_WIDTH*y] = max(min(iPx1 - iPx2 + 128, 255), 0);

			if (btConventionalOdd45[x + (IMAGE_WIDTH * y)] > 128 + VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 + VALUE_OF_PARATEAU;
			}
			else if (btConventionalOdd45[x + (IMAGE_WIDTH * y)] < 128 - VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 - VALUE_OF_PARATEAU;
			}
			else {
				btOutput[x + (IMAGE_WIDTH * y)] = btConventionalOdd45[x + (IMAGE_WIDTH * y)];
			}
		}
	}
}

VOID MakeDiagonallyOdd135(BYTE *btInput, BYTE *btOutput, INT iDistanceAway) {
	INT	y, x;
	INT iPx1, iPx2;

	BYTE btConventionalOdd135[IMAGE_DATA_LENGTH];

	for (y = 0; y < IMAGE_HEIGHT; y++) {
		for (x = 0; x < IMAGE_WIDTH; x++) {

			if (x < iDistanceAway) {
				iPx1 = btInput[IMAGE_WIDTH*(y + (iDistanceAway - 1))];
			}
			else if (y >= IMAGE_HEIGHT - iDistanceAway) {
				iPx1 = btInput[(x - iDistanceAway + 1) + IMAGE_WIDTH*(IMAGE_HEIGHT - 1)];
			}
			else {
				iPx1 = btInput[x + (IMAGE_WIDTH*(y + iDistanceAway)) - iDistanceAway];
			}

			if (x >= IMAGE_WIDTH - iDistanceAway) {
				iPx2 = btInput[(IMAGE_WIDTH - 1) + IMAGE_WIDTH*(y - (iDistanceAway - 1))];
			}
			else if (y < iDistanceAway) {
				iPx2 = btInput[(x + iDistanceAway - 1) + IMAGE_WIDTH*(y - (iDistanceAway - 1))];
			}
			else {
				iPx2 = btInput[x + (IMAGE_WIDTH*(y - iDistanceAway)) + iDistanceAway];
			}

			btConventionalOdd135[x + IMAGE_WIDTH*y] = max(min(iPx1 - iPx2 + 128, 255), 0);

			if (btConventionalOdd135[x + (IMAGE_WIDTH * y)] > 128 + VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 + VALUE_OF_PARATEAU;
			}
			else if (btConventionalOdd135[x + (IMAGE_WIDTH * y)] < 128 - VALUE_OF_PARATEAU) {
				btOutput[x + (IMAGE_WIDTH * y)] = 128 - VALUE_OF_PARATEAU;
			}
			else {
				btOutput[x + (IMAGE_WIDTH * y)] = btConventionalOdd135[x + (IMAGE_WIDTH * y)];
			}

		}
	}
}
