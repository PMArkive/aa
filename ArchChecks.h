#pragma once
#include <Windows.h>

/*
CS values:
	64 bits (native) = 0x33
	32 bits (wow64)  = 0x23
	32 bits (native) = 0x1B
*/

BOOL AA_Is64bitOS(void)
{
#ifdef _WIN64
	return TRUE;
#else
	SHORT csValue;

	__asm {
		mov csValue, cs
	}

	return csValue == 0x0023;
#endif
}
