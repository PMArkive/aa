#pragma once

// Might need a different check for user-mode drivers
#if _KERNEL_MODE
#include <ntdef.h>
#else
#include <Windows.h>
#endif

// Just a note that UNICODE_STRING.Length and .MaximumLength is the number of bytes... (with Length excluding the NUL-terminator)

BOOLEAN AA_usrchr(PCUNICODE_STRING in, wchar_t chr, PUNICODE_STRING out)
{
	int len = 0;
	const wchar_t * last = NULL;

	for (int i = 0; i < in->Length; i++) {
		if (in->Buffer[i] == chr) {
			last = &(in->Buffer[i]);
			len = i;
		}
	}

	if (!last)
		return FALSE;

	out->Buffer = (PWCH)last;
	out->Length = in->Length - (USHORT)(len * sizeof(WCHAR));
	out->MaximumLength = out->Length;
	return TRUE;
}

BOOLEAN AA_usBaseFile(PCUNICODE_STRING in, PUNICODE_STRING out)
{
	UNICODE_STRING lastElement;
	if (!AA_usrchr(in, L'\\', &lastElement))
		return FALSE;

	if (lastElement.Length < (2 * sizeof(WCHAR)))
		return FALSE;

	lastElement.Length -= sizeof(WCHAR);
	lastElement.MaximumLength -= sizeof(WCHAR);
	++lastElement.Buffer;
	*out = lastElement;
	return TRUE;
}
