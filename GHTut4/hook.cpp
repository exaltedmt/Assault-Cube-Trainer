#include "stdafx.h"
#include "hook.h"

bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len)
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

BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len)
{
    if (len < 5)
    {
        return 0;
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

Hook::Hook(BYTE* src, BYTE* dst, BYTE* PtrToGatewayFnPtr, uintptr_t len)
{
    // Make local copies
    this->src = src;
    this->dst = dst;
    this->len = len;
    this->PtrToGatewayFnPtr = PtrToGatewayFnPtr;
}

Hook::Hook(const char* exportName, const char* modName, BYTE* dst, BYTE* PtrToGatewayFnPtr, uintptr_t len)
{
    HMODULE hMod = GetModuleHandleA(modName);

    this->src = src;
    this->dst = dst;
    this->len = len;
    this->PtrToGatewayFnPtr = PtrToGatewayFnPtr;
}

void Hook::Enable()
{
    memcpy(originalBytes, src, len);
    *(uintptr_t*)PtrToGatewayFnPtr = (uintptr_t)TrampHook32(src, dst, len);
    bStatus = true;
}

void Hook::Disable()
{
    mem::Patch(src, originalBytes, len);
    bStatus = false;
}

void Hook::Toggle()
{
    if (bStatus) Enable();
    else Disable();
}

/* Without having to use Hex, we can declare the assm opcodes.
DWORD jmpBackAddy;

void __declspec(naked) ourFunct()
{
    __asm
    {
        jmp[jmpBackAddy]
    }
}
*/

/*
bool mem::inlineHook(BYTE* dst, BYTE* src, unsigned int size)
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
