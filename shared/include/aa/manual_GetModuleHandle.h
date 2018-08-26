#pragma once
#include <Windows.h>
#include "GetCurrentExe.h"
#include "GetPEB.h"
#include "StringConversions.h"

// Short definition so we can include BaseDllName which will add padding too
typedef struct _LDR_DATA_TABLE_ENTRY_SHORT {
	PVOID Reserved1[2];
	LIST_ENTRY InMemoryOrderLinks;
	PVOID Reserved2[2];
	PVOID DllBase;
	PVOID Reserved3[2];
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG a;
} LDR_DATA_TABLE_ENTRY_SHORT, *PLDR_DATA_TABLE_ENTRY_SHORT;

// TODO: Lock the loader-lock...
HMODULE WINAPI AA_GetModuleHandleW_Generic(
	_In_opt_ LPCWSTR lpModuleName
)
{
	if (!lpModuleName)
		return AA_GetCurrentExe();

	//PLDR_DATA_TABLE_ENTRY_SHORT ldrHead = (PLDR_DATA_TABLE_ENTRY_SHORT)
	//	(->InMemoryOrderModuleList.Flink;
	//PLDR_DATA_TABLE_ENTRY_SHORT ldrNext = ldrHead;

	PLIST_ENTRY lpHead = &((PPEB)AA_GetPEB_Generic())->Ldr->InMemoryOrderModuleList, lpCurrent = lpHead;

	while ((lpCurrent = lpCurrent->Flink) != lpHead)
	{
		PLDR_DATA_TABLE_ENTRY_SHORT lpDataTable = CONTAINING_RECORD(lpCurrent, LDR_DATA_TABLE_ENTRY_SHORT, InMemoryOrderLinks);
		const wchar_t * baseDllName = lpDataTable->BaseDllName.Buffer;
		if (!lstrcmpiW(lpModuleName, baseDllName))
			return (HMODULE)lpDataTable->DllBase;
	}

	return 0;
}

HMODULE WINAPI AA_GetModuleHandleA_Generic(
	_In_opt_ LPCSTR lpModuleName,
	_In_opt_ UINT CodePage
)
{
	if (!lpModuleName)
		return AA_GetCurrentExe();

	wchar_t * str = AA_ToUnicode_Convert(lpModuleName, CodePage);

	HMODULE hModule = AA_GetModuleHandleW_Generic(str);
	AA_ToUnicode_Free(str);
	return hModule;
}
