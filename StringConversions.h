#pragma once
#include <Windows.h>

wchar_t * AA_ToUtf16_Convert(const char * str, UINT CodePage)
{
	wchar_t * unicode;
	int str_len = (int)lstrlenA(str);

	int size = MultiByteToWideChar(
		CodePage,
		MB_ERR_INVALID_CHARS,
		str,
		str_len,
		NULL,
		0
	);

	if (size == 0)
		return NULL;

	unicode = (wchar_t *)HeapAlloc(GetProcessHeap(), 0, size + sizeof(*unicode));
	if (!unicode)
		return NULL;

	(void)MultiByteToWideChar(
		CodePage,
		MB_ERR_INVALID_CHARS,
		str,
		str_len,
		unicode,
		size
	);

	unicode[size] = 0;

	return unicode;
}

void AA_ToUtf_Free(const void * str)
{
	if (str)
		(void)HeapFree(GetProcessHeap(), 0, (LPVOID)str);
}

char * AA_ToUtf8_Convert(const wchar_t * input)
{
	char * utf8;
	int str_len = (int)lstrlenW(input);
	
	int size = WideCharToMultiByte(
		CP_UTF8,
		0,
		input,
		str_len,
		NULL,
		0,
		NULL,
		NULL
	);
	
	if (size == 0)
		return NULL;
	
	utf8 = (char *)HeapAlloc(GetProcessHeap(), 0, size + sizeof(*utf8));
	if (!utf8)
		return NULL;

	(void)WideCharToMultiByte(
		CP_UTF8,
		0,
		input,
		str_len,
		utf8,
		size,
		NULL,
		NULL
	);

	utf8[size] = 0;

	return utf8;
}
