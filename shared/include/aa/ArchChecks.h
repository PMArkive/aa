#pragma once

/*
CS values:
	64 bits (native) = 0x33
	32 bits (wow64)  = 0x23
	32 bits (native) = 0x1B
*/

bool AA_Is64bitOS(void)
{
#ifdef _WIN64
	return true;
#else
	short csValue;

	__asm {
		mov csValue, cs
	}

	return csValue == 0x0023;
#endif
}
