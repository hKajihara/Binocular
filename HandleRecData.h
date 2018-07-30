#ifndef HANDLERECDATA_H
#define HANDLERECDATA_H

#include <windows.h>

VOID Load(HWND);
VOID UnLoad(HWND);
VOID PlayWithProcCheck(HWND);
VOID PlayButtonDown(HWND);
VOID ForwardButtonDown(HWND);
VOID BackwardButtonDown(HWND);
VOID StopButtonDown(HWND);
VOID RwdButtonDown(HWND);
VOID DrawOneFrame(HWND);
VOID ExportMenuClick();
INT	 ExportBitmap(UINT, UINT);

#endif
