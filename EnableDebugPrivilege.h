#pragma once
#include <Windows.h>


typedef NTSTATUS(__stdcall *fn_RtlAdjustPrivilege)(
	ULONG Privilege,
	BOOLEAN Enable,
	BOOLEAN Client, // TRUE for thread, FALSE for process
	PBOOLEAN WasEnabled
);

bool AA_GrantDebugPrivilege_Generic0(void)
{
	TOKEN_PRIVILEGES tokenPrivileges;
	HANDLE hToken;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken))
		return false;

	if (!LookupPrivilegeValueW(L"", L"SeDebugPrivilege", &luid))
		return false;

	tokenPrivileges.PrivilegeCount = 1;
	tokenPrivileges.Privileges[0].Luid = luid;
	tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	return !!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tokenPrivileges,
		sizeof(tokenPrivileges),
		NULL,
		NULL
	);
}

// BYOF = Bring your own function
bool AA_GrantDebugPrivilege_BYOF(fn_RtlAdjustPrivilege pRtlAdjustPrivilege)
{
	BOOLEAN WasEnabled;
	NTSTATUS status = pRtlAdjustPrivilege(
		20L, // SE_DEBUG_PRIVILEGE
		TRUE,
		FALSE, // For process
		&WasEnabled
	);

	return status == 0; // STATUS_SUCCESS;
}

bool AA_GrantDebugPrivilege_Generic1(void)
{
	fn_RtlAdjustPrivilege pRtlAdjustPrivilege = (fn_RtlAdjustPrivilege)GetProcAddress(
		GetModuleHandleW(L"ntdll.dll"),
		"RtlAdjustPrivilege"
	);

	return AA_GrantDebugPrivilege_BYOF(pRtlAdjustPrivilege);
}
