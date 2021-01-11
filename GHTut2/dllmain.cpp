#pragma once
#include "stdafx.h"
#include "mem.h"

DWORD WINAPI HackThread(HMODULE hModule)
{
	//Create console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	std::cout << "OG for a fee, stay sipin' fam\n";

	//get module base
	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

	bool bHealth = false, bAmmo = false, bRecoil = false;

	//hack loop
	while (true)
	{
		//key input
		if (GetAsyncKeyState(VK_END) & 1)
		{
			break;
		}

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			bHealth = !bHealth;
		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			bAmmo = !bAmmo;
		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			bRecoil = !bRecoil;

			if (bRecoil)
			{
				//nop
				//mem::Nop((BYTE*)(moduleBase + 0x63786), 10);

				//ret 0008 - found at the end of the recoil call.
				mem::Patch((BYTE*)(moduleBase + 0x62020), (BYTE*)"\xC2\x08\x00", 3);
			}

			else
			{
				//write back original instructions
				//mem::Patch((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);

				// push ebp - original assembly - taken from CE Memory View
				mem::Patch((BYTE*)(moduleBase + 0x62020), (BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x3C\x53\x56\x8B\xF1\x8B\x46\x0C", 16);
			}
		}
	}

	//continous write/freeze
	uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10F4F4);

	if (localPlayerPtr)
	{
		if (bHealth)
		{
			// Dereference localPlayerPtr, offset 0xF8, cast type to int*, and finally dereference to change value.
			*(int*)(*localPlayerPtr + 0xF8) = 1337;
		}

		if (bAmmo)
		{
			uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 });
			int* ammo = (int*)ammoAddr;
			*ammo = 1337;

			// *(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1337;
		}
	}
	Sleep(5);

	//cleanup & eject
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain ( HMODULE hModule,
					    DWORD ul_reason_for_call,
					    LPVOID lpReserved
					   )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
		}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

// Created with ReClass.NET 1.2 by KN4CK3R

// Defining the ReClass variable types.
struct Vector3 { float x, y, z; };

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

// Defining these classes in player entity class, so bring above.
class weapon
{
public:

	// ReClass had variable type as clipAmmo* and ammoClip*, as a result of it's pointer convention.
	// We are pointing to a value, so change to int*
	union {
		//              Type     Name    Offset
		DEFINE_MEMBER_N(int*, clipPtr, 0x0010);
		DEFINE_MEMBER_N(int*, ammoPtr, 0x0014);
		DEFINE_MEMBER_N(int, ammoDec, 0x001C); 
		DEFINE_MEMBER_N(int*, weaponTypePtr, 0x000C);
	};

}; //Size: 0x0044

class weaponType
{
public:

	union {
		//              Type     Name    Offset
		// DEFINE_MEMBER_N(int, clip, 0x0000);
		DEFINE_MEMBER_N(int, shotsFired, 0x0010);
		DEFINE_MEMBER_N(int, lastWepClip, 0x0014);
	};

}; //Size: 0x0044

class ammoClip
{
public:

	union {
		//             Type     Name    Offset
		// No offset value for these variables, so no need to define them, + throws error. Just call address.
		// DEFINE_MEMBER_N(int, ammo, 0x0000);
		DEFINE_MEMBER_N(int, recoil, 0x0028);
	};

}; //Size: 0x0044

class clipAmmo
{
public:

	union {
		//              Type     Name    Offset
		// DEFINE_MEMBER_N(int, clip, 0x0000);
		DEFINE_MEMBER_N(int, shotsFired, 0x0010);
		DEFINE_MEMBER_N(int, lastWepClip, 0x0014);
	};

}; //Size: 0x0044

class weaponType
{
public:

	// char weaponName[16]; //0x0000
	union {
	//					Type     Name    Offset
		DEFINE_MEMBER_N(int, magSize, 0x0118);
		DEFINE_MEMBER_N(int, wepTypeRecoil1, 0x0120);
		DEFINE_MEMBER_N(int, wepTypeRecoil2, 0x0122);
	};

}; //Size: 0x0128

// Player Entity
class ent
{
public:

	union {
		//              Type     Name    Offset
		DEFINE_MEMBER_N(Vector3, poshead, 0x4);
		DEFINE_MEMBER_N(Vector3, pos, 0x0034);
		DEFINE_MEMBER_N(Vector3, angle, 0x0040);
		DEFINE_MEMBER_N(int, health, 0x00F8);
		DEFINE_MEMBER_N(weapon*, currWeapon, 0x0374);
	};

	/* Original ReClass definitions
	Vector3 poshead; //0x0004
	char pad_0010[36]; //0x0010
	Vector3 pos; //0x0034
	Vector3 angle; //0x0040
	char pad_004C[172]; //0x004C
	int32_t health; //0x00F8
	int32_t shield; //0x00FC
	char pad_0100[293]; //0x0100
	char N00001D91[16]; //0x0225
	char pad_0235[319]; //0x0235
	class weapon* currWeapon; //0x0374
	char pad_0378[244]; //0x0378

	virtual void Function0();
	virtual void Function1();
	virtual void Function2();
	virtual void Function3();
	virtual void Function4();
	virtual void Function5();
	virtual void Function6();
	virtual void Function7();
	virtual void Function8();
	virtual void Function9();
	*/
}; //Size: 0x046C

// Removing static asserts since we're not longer using original ReClass, ex: static_assert(sizeof(clipAmmo) == 0x44);