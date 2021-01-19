#include "stdafx.h"
#include "mem.h"

void mem::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
    DWORD oldprotect;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldprotect, &oldprotect);
    
}

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
    DWORD oldprotect;
    VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
    WriteProcessMemory(hProcess, dst, src, size, nullptr);
    VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

bool mem::Detour32(BYTE* src, BYTE* dst, const uintptr_t len)
{
    if (len < 5)
    {
        return false;
    }

    DWORD oldprotect;
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oldprotect);

    // were not noping like in the simple hook.
    uintptr_t relativeAddress = dst - src - 5;

    // jmp
    *src = 0xE9;
    *(uintptr_t*)(src + 1) = relativeAddress;

    DWORD curprotect;
    VirtualProtect(src, len, oldprotect, &curprotect);

    return true;
}

BYTE* mem::TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len)
{
    if (len < 5)
    {
        return false;
    }

    // Create Gateway
    BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    // write the stolen bytes to the gateway
    memcpy_s(gateway, len, src, len);

    // Get the gateway to destination address
    uintptr_t gatewayRelativeAddr = src - gateway - 5;

    // add the jmp opcode to the end of the gateway
    *(gateway + len) = 0xE9;

    // Write the address of the gateway to the jmp
    *(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

    // Perform the detour
    Detour32(src, dst, len);

    return gateway;

}

/*
bool mem::Hook(BYTE* dst, BYTE* src, unsigned int size)
{
    // We check the length to be at least 5 bytes because this is the smallest relative jmp in x86. - https://guidedhacking.com/threads/c-detour-hooking-function-tutorial.7930/
    // "Typically you are doing this to detour the code into a memory region where your own code exists. Thus you're forcing the game to execute your code."
    if (size < 5)
    {
        return false;
    }

    DWORD oldprotect;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);

    // any stray bytes at end of jump, can cause unwanted behavior
    // nop
    memset(dst, 0x90, size);
    DWORD relativeAddress = ((DWORD)src - (DWORD)dst) - 5;

    // jmp
    *(BYTE*)dst = 0xE9;
    *(DWORD*)((DWORD)dst + 1) = relativeAddress;

    DWORD curprotect;
    VirtualProtect(dst, size, oldprotect, &curprotect);

    return true;
}
*/


void mem::Nop(BYTE* dst, unsigned int size)
{
    DWORD oldprotect;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
    memset(dst, 0x90, size);
    VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
    BYTE* nopArray = new BYTE[size];
    // fills array with 0x90 aka nop code
    memset(nopArray, 0x90, size);

    PatchEx(dst, nopArray, size, hProcess);
    delete[] nopArray;
}

uintptr_t mem::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets)
{
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); ++i)
    {
        addr = *(uintptr_t*)addr;
        addr += offsets[i];
    }
    return addr;

}

uintptr_t mem::FindDMAAddyEx(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); ++i)
    {
        ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];
    }
    return addr;
}