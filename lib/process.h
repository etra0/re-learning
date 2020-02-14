#include <vector>
#include <windows.h>
#include <string.h>
#include <iostream>
#include <TlHelp32.h>

DWORD getProcId(const wchar_t* procName);

uintptr_t getModuleBaseAddress(DWORD procId, const wchar_t* modName);

uintptr_t followDynamicPointer(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);