#pragma once

//#include "StringConversions.h"
#include <string>
#include <Windows.h>
#include <locale>
#include <codecvt>

static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

std::string AA_ToUtf8_Convert_std2(const wchar_t * ws, size_t len)
{
	if (len == 0) len = wcslen(ws);
	return converter.to_bytes(ws, ws + len);
}

std::wstring AA_ToUtf16_Convert_std2(const char * str, size_t len)
{
	if (len == 0) len = strlen(str);
	return converter.from_bytes(str, str + len);
}

//std::string AA_ToUtf8_Convert_std(const wchar_t * ws, size_t len)
//{
//	if (len == 0) len = wcslen(ws);
//
//	std::string str;
//
//	int size = WideCharToMultiByte(
//		CP_UTF8,
//		0,
//		ws,
//		(int)len,
//		NULL,
//		0,
//		NULL,
//		NULL
//	);
//
//	if (size == 0) return "";
//
//	str.reserve(size+1);
//
//	(void)WideCharToMultiByte(
//		CP_UTF8,
//		0,
//		ws,
//		(int)len,
//		(char *)&str[0],
//		size,
//		NULL,
//		NULL
//	);
//
//	//str[size] = 0;
//
//	// Re-allocating because str's size was never updated and it will cause problems.
//	return std::string(&str[0], size);
//	//return str;
//}
//
//std::wstring AA_ToUtf16_Convert_std(const char * str, size_t len)
//{
//	if (len == 0) len = strlen(str);
//
//	std::wstring ws;
//
//	int size = MultiByteToWideChar(
//		CP_UTF8,
//		MB_ERR_INVALID_CHARS,
//		str,
//		(int)len,
//		NULL,
//		0
//	);
//
//	if (size == 0) return L"";
//
//	ws.reserve(size+1);
//
//	(void)MultiByteToWideChar(
//		CP_UTF8,
//		MB_ERR_INVALID_CHARS,
//		str,
//		(int)len,
//		(wchar_t *)&ws[0],
//		size
//	);
//
//	//ws[size] = 0;
//
//	// Re-allocating because ws's size was never updated and it will cause problems.
//	return std::wstring(&ws[0], size);
//	//return ws;
//}

