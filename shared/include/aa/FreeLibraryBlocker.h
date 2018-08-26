#pragma once
//#define WIN32_NO_STATUS
#include <Windows.h>
//#undef WIN32_NO_STATUS
//#include <ntstatus.h>

// If a module's load count hits the max (0xFFFF I believe) then it should act as pinned.

// Located in NTDLL.dll
typedef NTSTATUS (* WINAPI fn_LdrAddRefDll)(
	_In_ DWORD dwFlags,
	_In_ HMODULE hModule
);

bool FreeLibraryBlocker_HMODULE(HMODULE hModule, fn_LdrAddRefDll pLdrAddRefDll)
{
	return 0 == //STATUS_SUCCESS ==
		pLdrAddRefDll(GET_MODULE_HANDLE_EX_FLAG_PIN, hModule);
}

HMODULE FreeLibraryBlocker_GetModuleHandleA(LPCSTR lpModuleName)
{
	HMODULE hModule;
	if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_PIN, lpModuleName, &hModule))
		return hModule;
	return 0;
}

HMODULE FreeLibraryBlocker_GetModuleHandleW(LPCWSTR lpModuleName)
{
	HMODULE hModule;
	if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, lpModuleName, &hModule))
		return hModule;
	return 0;
}
