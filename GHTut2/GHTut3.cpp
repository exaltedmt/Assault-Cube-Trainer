// #pragma once
#include "stdafx.h"
#include "proc.h"
#include "mem.h"

int test2()
{
	HANDLE hProcess = 0;

	uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
	bool bHealth = false, bAmmo = false, bRecoil = false;

	const int newValue = 1337;

	DWORD procId = GetProcId(L"ac_client.exe");

	if (procId)
	{

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
		
		moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

		localPlayerPtr = moduleBase + 0x10F4F4;

		healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xF8 });

	}

	else 
	{
		std::cout << "Process not found, press enter to exit\n";
		getchar();
		return 0;
	}

	DWORD dwExit = 0;

	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
	{
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			bHealth = !bHealth;
		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			
			bAmmo = !bAmmo;

			if (bAmmo)
			{
				// Increases ammo instead of decrease
				// x06: inc [esi]
				mem::PatchEx((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x06", 2, hProcess);

				mem::PatchEx((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2, hProcess);
			}
		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			bRecoil = !bRecoil;

			// "nop" variant commented out
			if (bRecoil)
			{
				// mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);

				// Max size is 3 byte space for ret with value opcode.
				// Commonly: ret XXXX
				// aka ret 0008 - found at the end of the recoil call.
				mem::PatchEx((BYTE*)(moduleBase + 0x62020), (BYTE*)"\xC2\x08\x00", 3, hProcess);
			}
			else
			{
				// mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);

				// push ebp - original assembly - taken from CE
				mem::PatchEx((BYTE*)(moduleBase + 0x62020), (BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x3C\x53\x56\x8B\xF1\x8B\x46\x0C", 16, hProcess);
			}

		}

		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			return 0;
		}

		if (bHealth)
		{
			mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
		}

		Sleep(10);
	}

	std::cout << "Process not found, press enter to exit\n";
	getchar();
	return 0;
}