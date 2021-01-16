#pragma once
#include "stdafx.h"

HINSTANCE GetModuleHandleEx(HANDLE hTargetProc, const TCHAR* lpModuleName);
void* GetProcAddressEx(HANDLE hTargetroc, const TCHAR* lpModuleNam, const char* lpProcName);