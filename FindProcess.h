#pragma once
#include <Windows.h>
#include <TlHelp32.h>

// TODO:
// NtQuerySystemInformation with SystemProcessInformation or Extended/Full versions.

DWORD AA_FindProcessByExe_Generic(const wchar_t* name)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == NULL || hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(entry);

	DWORD pid = 0;
	BOOL GotProcs = Process32FirstW(hSnapshot, &entry);

	while (GotProcs) {
		if (!_wcsicmp(name, entry.szExeFile)) {
			pid = entry.th32ProcessID;
			break;
		}
		GotProcs = Process32NextW(hSnapshot, &entry);
	}

	CloseHandle(hSnapshot);
	return pid;
}
