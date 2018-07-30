#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <windows.h>

VOID	ShowWaveForm();
VOID	BufferWaveForm(BYTE* btImageData, UINT uiChannel);
VOID	ResetWaveFormBuffer();
VOID	DrawWaveForm();
VOID	CloseWaveForm();

#endif