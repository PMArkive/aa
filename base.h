#pragma once

// To link ProcessHacker phlib and phnt properly...
// Add the following to VC++ Directories - Include Directories:
//   processhacker\phnt\include
//   processhacker\phlib\include
// Add the following to VC++ Directories - Source Directories:
//   processhacker\phlib

// Allow usage of Windows 10 APIs.
// To allow the exe to run on XP:
//   Properties->Linker->Minimum Required version = 5.01 (x32) or 5.02 (x64)
#define _WIN32_WINNT 0x0A00
#define PHNT_VERSION 105 // PHNT_REDSTONE4 // Windows 10 RS4
#include <phbase.h>
