/*{REPLACEMEWITHLICENSE}*/
#pragma once
#include <Windows.h>
#include <string.h>

/* Usage:

#include "win32_ImportAddressTableHook.hpp"
typedef int (WINAPI * type_MessageBoxW)(HWND, LPCWSTR, LPCWSTR, UINT);
type_MessageBoxW original_MessageBoxW = NULL;
int WINAPI detour_MessageBoxW(HWND hwnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	return original_MessageBoxW(NULL, L"FUCK YOU", L":(", uType);
}

bool TestHook()
{
	if (!win32::ImportAddressTableHook("MessageBoxW", "user32.dll", detour_MessageBoxW, (void **)&original_MessageBoxW))
		return false;
	MessageBoxW(NULL, L"Hi, friend!", L":)", MB_OK);
	return true;
}

*/

namespace win32 {

bool ImportAddressTableHook(const char * name, const char * modname, void * new_func, void ** old_func)
{
	UINT_PTR module = (UINT_PTR)GetModuleHandle(NULL);
	PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt_headers = (PIMAGE_NT_HEADERS)(module + dos_header->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR id = (PIMAGE_IMPORT_DESCRIPTOR)(
		module + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
	);

	for (; id->Characteristics; ++id)
	{
		if (modname && 0 != _stricmp(modname, (const char *)(module + id->Name)))
			continue;
		auto first = (PIMAGE_THUNK_DATA)(module + id->FirstThunk),
		     orig = (PIMAGE_THUNK_DATA)(module + id->OriginalFirstThunk);
		for (; orig->u1.Function; ++orig, ++first)
		{
			if (orig->u1.Ordinal & IMAGE_ORDINAL_FLAG)
				continue;
			PIMAGE_IMPORT_BY_NAME x = (PIMAGE_IMPORT_BY_NAME)(module + orig->u1.AddressOfData);
			if (0 == strcmp(name, x->Name))
			{
				MEMORY_BASIC_INFORMATION mbi;
				if (!VirtualQuery(first, &mbi, sizeof(mbi)))
					return false;
				if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect))
					return false;

				void * oldy = (void *)first->u1.Function;
				first->u1.Function = (UINT_PTR)new_func;

				if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &mbi.Protect))
				{
					// well, let's just reset it...
					first->u1.Function = (UINT_PTR)oldy;
					return false;
				}

				if (old_func)
					*old_func = oldy;
				return true;
			}
		}
	}

	return false;
}

}
