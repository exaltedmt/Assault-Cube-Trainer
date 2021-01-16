#include "stdafx.h"
#include "mem.h"

// Created with ReClass.NET 1.2 by KN4CK3R
// Using Public_Void's padding - https://guidedhacking.com/threads/auto-padding-for-class-recreation.13478/

// Defining the ReClass variable types.
struct Vector3 { float x, y, z; };

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}
#define DEFINE_MEMBER_0(type, name, offset) struct {type name;}

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

class ammoClip
{
public:

	union {
		//             Type     Name    Offset
		// 0x0 offset value for these variables, which throws error.
		// Two ways to fix this:
		// Either create DEFINE_MEMBER_0 definition or...
		// Just instantiate the variable at the top of union
		// For completeness, I'll perform the latter.
		// int32_t ammo; //0x0000
		DEFINE_MEMBER_0(int, ammo, 0x0000);
		DEFINE_MEMBER_N(int, recoil, 0x0028);
	};

}; //Size: 0x0044

class clipAmmo
{
public:

	union {
		//              Type     Name    Offset
		// int32_t clip; //0x0000
		DEFINE_MEMBER_0(int, clip, 0x0000);
		DEFINE_MEMBER_N(int, shotsFired, 0x0010);
		DEFINE_MEMBER_N(int, lastWepClip, 0x0014);
	};

}; //Size: 0x0044

class weaponType
{
public:

	union {
		//			    Type     Name    Offset
		// char weaponName[16]; //0x0000
		DEFINE_MEMBER_0(char, weaponName[16], 0x0000);
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
		DEFINE_MEMBER_N(int, shield, 0x00FC);
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

DWORD WINAPI HackThread(HMODULE hModule)
{
	//Create Console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	std::cout << "Injection successful! Enjoy leet hax.\n";

	uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

	//calling it with NULL also gives you the address of the .exe module
	moduleBase = (uintptr_t)GetModuleHandle(NULL);

	bool bHealth = false, bAmmo = false, bRecoil = false;

	while (true)
	{
		if (GetAsyncKeyState(VK_END) & 1)
		{
			break;
		}

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
			bHealth = !bHealth;

		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			bAmmo = !bAmmo;
		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			bRecoil = !bRecoil;

			if (bRecoil)
			{
				//nop recoil version
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

		//need to use uintptr_t for pointer arithmetic later
		// uintptr_t* localPlayerPtr = (uintptr_t*)(moduleBase + 0x10F4F4);
		ent* localPlayerPtr = *(ent**)(moduleBase + 0x10F4F4);
		//continuous writes / freeze

		if (localPlayerPtr)
		{
			if (bHealth)
			{

				//*localPlayerPtr = derference the pointer, to get the localPlayerAddr
				// add 0xF8 to get health address
				//cast to an int pointer, this pointer now points to the health address
				//derference it and assign the value 1337 to the health variable it points to
				// *(int*)(*localPlayerPtr + 0xF8) = 1337; - without ReClass variables

				localPlayerPtr->health = 1337;

			}

			if (bAmmo)
			{
				//We aren't external now, we can now efficiently calculate all pointers dynamically
				//before we only resolved pointers when needed for efficiency reasons
				//we are executing internally, we can calculate everything when needed
				//uintptr_t ammoAddr = mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 });
				//int* ammo = (int*)ammoAddr;
				//*ammo = 1337;

				// or just...
				// *(int*)mem::FindDMAAddy(moduleBase + 0x10F4F4, { 0x374, 0x14, 0x0 }) = 1337;

				// LPP points to two classes, so we dereference it firstly
				// No need to explicitly call ammo variable, since 0x0.
				*localPlayerPtr->currWeapon->ammoPtr = 1337;
			}

		}
		Sleep(5);
	}

	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

/*
class weapon
{
public:
	char pad_0000[12]; //0x0000
	class weaponType* weaponTypePtr; //0x000C
	class clipAmmo* clipPtr; //0x0010
	class ammoClip* ammoPtr; //0x0014
	char pad_0018[4]; //0x0018
	int32_t ammoDec; //0x001C
	char pad_0020[36]; //0x0020
}; //Size: 0x0044
static_assert(sizeof(weapon) == 0x44);

class ammoClip
{
public:
	int32_t ammo; //0x0000
	char pad_0004[36]; //0x0004
	int32_t recoil; //0x0028
	char pad_002C[24]; //0x002C
}; //Size: 0x0044
static_assert(sizeof(ammoClip) == 0x44);

class clipAmmo
{
public:
	int32_t clip; //0x0000
	char pad_0004[12]; //0x0004
	int32_t shotsFired; //0x0010
	int32_t lastWepClip; //0x0014
	char pad_0018[16]; //0x0018
	int32_t currAmmo; //0x0028
	char pad_002C[24]; //0x002C
}; //Size: 0x0044
static_assert(sizeof(clipAmmo) == 0x44);

class weaponType
{
public:
	char weaponName[16]; //0x0000
	char pad_0010[244]; //0x0010
	int16_t N000001F6; //0x0104
	int16_t N0000063A; //0x0106
	int16_t N000001F7; //0x0108
	int16_t N0000063C; //0x010A
	int16_t N00000603; //0x010C
	int16_t N0000063E; //0x010E
	int16_t N000005EC; //0x0110
	int16_t N00000640; //0x0112
	int16_t N00000605; //0x0114
	int16_t N00000642; //0x0116
	int16_t magSize; //0x0118
	int16_t N00000644; //0x011A
	int16_t N00000607; //0x011C
	int16_t N00000646; //0x011E
	int16_t wepTypeRecoil1; //0x0120
	int16_t wepTypeRecoil2; //0x0122
	int16_t N00000609; //0x0124
	int16_t N0000064A; //0x0126
}; //Size: 0x0128
static_assert(sizeof(weaponType) == 0x128);

*/

// Removing static asserts since we're not longer using original ReClass, ex: static_assert(sizeof(clipAmmo) == 0x44);