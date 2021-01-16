// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// TODO: reference additional headers your program requires here
// #define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// https://stackoverflow.com/questions/11040133/what-does-defining-win32-lean-and-mean-exclude-exactly - According the to Windows Dev Center WIN32_LEAN_AND_MEAN excludes APIs such as Cryptography, DDE, RPC, Shell, and Windows Sockets.

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <TlHelp32.h>

/* http://www.mschaef.com/windows_h_is_wierd - if wanting to be lean...
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS

#include <ntstatus.h>
*/
