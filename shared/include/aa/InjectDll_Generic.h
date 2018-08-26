#pragma once
#include <Windows.h>

// TODO: Wait for remote thread to end.
// TODO: Free the memory from VirtualAllocEx
BOOL AA_InjectDll_Generic(HANDLE hProcess, const wchar_t* DLL, HANDLE* hOutThread)
{
	LPVOID address = VirtualAllocEx(hProcess, NULL, 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (address == NULL)
		return FALSE;

	SIZE_T bytesWritten;
	SIZE_T bytesToWrite = wcslen(DLL) * 2 + 2;
	BOOL bWrite = WriteProcessMemory(hProcess, address, DLL, bytesToWrite, &bytesWritten);
	if (bWrite == 0 || bytesWritten != bytesToWrite)
		return FALSE;

	FARPROC pLoadLibrarW = GetProcAddress(
		GetModuleHandleW(L"kernel32.dll"),
		"LoadLibraryW"
	);

	DWORD threadID;
	HANDLE hThread = CreateRemoteThreadEx(
		hProcess,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)pLoadLibrarW,
		address,
		0,
		NULL,
		&threadID
	);
	
	if (hThread == NULL)
		return FALSE;

	if (hOutThread)
		*hOutThread = hThread;

	return TRUE;
}
