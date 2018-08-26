#pragma once
#include <winternl.h>
#include "GetPEB.h"

inline HMODULE AA_GetCurrentExe(void)
{
	PPEB peb = (PPEB)AA_GetPEB_Generic();
	return (HMODULE)peb->Reserved3[1]; // ImageBaseAddress
}
