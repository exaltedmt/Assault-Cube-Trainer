#pragma once
#include "stdafx.h"

namespace mem
{
	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);

	// Rake uses Detour32 name
	bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len);
	BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len);

	// bool Hook(BYTE* dst, BYTE* src, unsigned int size);

	DWORD jmpBackAddy;

	void __declspec(naked) ourFunct()
	{
		__asm
		{
			jmp [jmpBackAddy]
		}
	}

	void Nop(BYTE* dst, unsigned int size);
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);

	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);
	uintptr_t FindDMAAddyEx(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
};
