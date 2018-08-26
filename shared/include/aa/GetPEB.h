#pragma once

inline void * AA_GetPEB_Generic(void)
{
#if _WIN64
	return (void *)__readgsqword(0x60);
#else
	return (void *)__readfsdword(0x30);
#endif
}
