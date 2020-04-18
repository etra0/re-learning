#include "Utils/MemoryMgr.h"
#include "Utils/Patterns.h"

void OnInitializeHook() {
    using namespace Memory::VP;
    using namespace hook;

    #ifndef KIWAMI
    auto sprint = get_pattern("F3 0F 11 4B 0C E9 CD", 0);
    #else
    auto sprint = get_pattern("F3 0F 11 43 0C E9 D3", 0);
    #endif
    Nop(sprint, 5);


}
