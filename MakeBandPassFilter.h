#pragma once
#include "ConstantValues.h"
#include "Global.h"
#include <windows.h>
VOID MakeBandPassFilter(BYTE *btInput, BYTE *btOutput, INT iNumOfData);
VOID MakeDiagonallyEdgeEmphasis45(BYTE *btInput, BYTE *btOutput, INT iNumOfData);
VOID MakeDiagonallyEdgeEmphasis135(BYTE *btInput, BYTE *btOutput, INT iNumOfData);
VOID MakeVerticalOdd(BYTE *btInput, BYTE *btOutput, INT iDistanceAway);
VOID MakeDiagonallyOdd45(BYTE *btInput, BYTE *btOutput, INT iDistanceAway);
VOID MakeDiagonallyOdd135(BYTE *btInput, BYTE *btOutput, INT iDistanceAway);
