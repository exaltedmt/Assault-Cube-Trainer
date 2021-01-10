#include "stdafx.h"
#include <iostream>
// #include "mem.h"

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