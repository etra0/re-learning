#include "header.h"

DWORD getProcId(const wchar_t* procName);

uintptr_t getModuleBaseAddress(DWORD procId, const wchar_t* modName);

uintptr_t followDynamicPointer(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

void getByteVector(HANDLE hProc, uintptr_t ptr, std::vector<BYTE> &target, int n);

void writeByteVector(HANDLE hProc, uintptr_t ptr, std::vector<BYTE> &source);