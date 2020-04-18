#include "Utils/MemoryMgr.h"
#include "Utils/Patterns.h"

void OnInitializeHook() {
    using namespace Memory::VP;
    using namespace hook;

    auto sprint = get_pattern("F3 0F 11 4B 0C E9 CD", 0);
    Nop(sprint, 5);


}
