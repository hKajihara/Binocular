#include "Utils.h"

/******************************************************************************
 * Get Present Time
 ******************************************************************************/
VOID GetTimeString(char* strTime)
{
	SYSTEMTIME	sysTime;
	TCHAR		strYear[5], strMonth[3], strDay[3];
	TCHAR		strHour[3], strMinute[3], strSecond[3];

	GetLocalTime(&sysTime);

	_itoa_s(sysTime.wYear, strYear, 5, 10);
	lstrcat(strTime, strYear);
	lstrcat(strTime, "-");

	_itoa_s(sysTime.wMonth, strMonth, 3, 10);
	if(sysTime.wMonth < 10){
		strMonth[2] = '\0';
		strMonth[1] = strMonth[0];
		strMonth[0] = '0';
	}
	lstrcat(strTime, strMonth);

	_itoa_s(sysTime.wDay, strDay, 3, 10);
	if(sysTime.wDay < 10){
		strDay[2] = '\0';
		strDay[1] = strDay[0];
		strDay[0] = '0';
	}
	lstrcat(strTime, strDay);
	lstrcat(strTime, "-");

	_itoa_s(sysTime.wHour, strHour, 3, 10);
	if(sysTime.wHour < 10){
		strHour[2] = '\0';
		strHour[1] = strHour[0];
		strHour[0] = '0';
	}
	lstrcat(strTime, strHour);

	_itoa_s(sysTime.wMinute, strMinute, 3, 10);
	if(sysTime.wMinute < 10){
		strMinute[2] = '\0';
		strMinute[1] = strMinute[0];
		strMinute[0] = '0';
	}
	lstrcat(strTime, strMinute);
	lstrcat(strTime, "-");

	_itoa_s(sysTime.wSecond, strSecond, 3, 10);
	if(sysTime.wSecond < 10){
		strSecond[2] = '\0';
		strSecond[1] = strSecond[0];
		strSecond[0] = '0';
	}
	lstrcat(strTime, strSecond);
}