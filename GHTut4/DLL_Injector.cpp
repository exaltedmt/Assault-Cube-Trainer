#include "stdafx.h"
#include "proc.h"

int main()
{
	// Our project uses Unicode Character set, instead of Multibyte.
	// Function in proc takes wchar.
	const char* dllPath = "C:\\Users\\virul\\Documents\\Projects\\Memory_Editing\\GHTut4\\Debug\\GHTut4.dll";
	const wchar_t* procName = L"ac_client.exe";
	bool retval = 0;
	DWORD procId = 0;

	while (!procId)
	{
		// Not effected by the difference b/w 
		procId = GetProcId(procName);
		Sleep(30);
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

	if (hProc && hProc != INVALID_HANDLE_VALUE)
	{
		// LPVOID is simply a Windows API typedef for void*. 
		void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		// prevents loc = 0 warning
		if (loc)
		{
			//Does not write if our dllPath is also wchar_t*
			//WriteProcessMemory(hProc, loc, dllPath, wcslen(dllPath) + 1, 0);

			retval = WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
		}
		
		HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

		if (hThread)
		{
			CloseHandle(hThread);
		}
	}

	if (hProc)
	{
		CloseHandle(hProc);
	}

	return 0;
}