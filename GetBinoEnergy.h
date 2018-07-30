#pragma once
#include "ConstantValues.h"
#include "Global.h"
#include <windows.h>

INT GetBinoEnergy(BYTE *btLeftEven, BYTE *btRightEven, BYTE *btLeftOdd, BYTE *btRightOdd, INT iShiftAmount, INT iNumOfPixel, INT thresh);