#ifndef DISPLAY_H
#define DISPLAY_H

#include <windows.h>

VOID FinalizeDisplay(HWND);

INT_PTR CALLBACK DisplayDlgProc(HWND hWnd, UINT uiMessage, WPARAM wParameter, LPARAM lParameter);

#endif