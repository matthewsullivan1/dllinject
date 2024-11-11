#include <windows.h>
#include <iostream>

typedef int (WINAPI* MessageBoxW_t)(HWND, LPCWSTR, LPCWSTR, UINT);
MessageBoxW_t OriginalMessageBoxW = nullptr;

BYTE originalBytes[14] = { 0 };

int WINAPI HookedMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
    OutputDebugStringA("Hooked MessageBoxW called\n");

    //Temporarily restore the original bytes to call the original function to prevent infinite recursion
    DWORD oldProtect;
    VirtualProtect(OriginalMessageBoxW, sizeof(originalBytes), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(OriginalMessageBoxW, originalBytes, sizeof(originalBytes));
    VirtualProtect(OriginalMessageBoxW, sizeof(originalBytes), oldProtect, &oldProtect);

    // Call unhooked MBW
    int mbw = OriginalMessageBoxW(hWnd, L"Hooked MBW", L"Hooked MBW", uType);

    // Reapply the hook after calling the original function
    VirtualProtect(OriginalMessageBoxW, sizeof(originalBytes), PAGE_EXECUTE_READWRITE, &oldProtect);
    BYTE jmp[14] = { 0x48, 0xB8 }; // mov rax, HookedMessageBoxW
    *(void**)(jmp + 2) = (void*)HookedMessageBoxW;
    jmp[10] = 0xFF; jmp[11] = 0xE0; // jmp rax
    memcpy(OriginalMessageBoxW, jmp, sizeof(jmp));
    VirtualProtect(OriginalMessageBoxW, sizeof(originalBytes), oldProtect, &oldProtect);

    return mbw;
}

void HookMessageBoxW() {
    HMODULE hUser32 = LoadLibraryW(L"user32.dll");
    if (hUser32 == NULL) {
        OutputDebugStringA("Failed to load user32.dll\n");
        return;
    }

    OriginalMessageBoxW = (MessageBoxW_t)GetProcAddress(hUser32, "MessageBoxW");
    if (OriginalMessageBoxW == NULL) {
        OutputDebugStringA("Failed to get address of MessageBoxW\n");
        return;
    }

    // Save the original bytes
    memcpy(originalBytes, OriginalMessageBoxW, sizeof(originalBytes));

    // Apply the hook
    DWORD oldProtect;
    VirtualProtect(OriginalMessageBoxW, sizeof(originalBytes), PAGE_EXECUTE_READWRITE, &oldProtect);
    BYTE jmp[14] = { 0x48, 0xB8 }; // mov rax, HookedMessageBoxW
    *(void**)(jmp + 2) = (void*)HookedMessageBoxW;
    jmp[10] = 0xFF; jmp[11] = 0xE0; // jmp rax
    memcpy(OriginalMessageBoxW, jmp, sizeof(jmp));
    VirtualProtect(OriginalMessageBoxW, sizeof(originalBytes), oldProtect, &oldProtect);
}


// entry
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        MessageBoxW(NULL, L"DLL Injected", L"Status", MB_OK); // For confirmation
        HookMessageBoxW();
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
