#pragma once
#include "ConstantValues.h"
#include "Global.h"
#include <windows.h>
VOID ClassicDisparity(BYTE *btRight,BYTE *btLeft, BYTE *Output, INT NumOfPixel);
VOID MaxOfSumDisparity(BYTE *btLeft, BYTE *btRight, BYTE *btOutput, INT iNumOfPixel);
VOID NormalClassicDisparity(BYTE *btLeft, BYTE *btRight, BYTE *btOutput, INT iNumOfPixel);