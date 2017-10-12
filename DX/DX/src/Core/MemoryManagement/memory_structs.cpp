#include "memory_structs.h"

/**********************************************************************
    class: None (memory_structs.cpp)

    author: S. Hau
    date: October 12, 2017

**********************************************************************/

#include "Core/locator.h"
#include "Utils/utils.h"

namespace Core { namespace MemoryManagement {


    void AllocationMemoryInfo::log() const
    {
        LOG("-------------- MEMORY INFO ---------------");
        LOG("Current Allocated: " + Utils::bytesToString(currentBytesAllocated));
        LOG("Total Allocated: " + Utils::bytesToString(totalBytesAllocated));
        LOG("Total Freed: " + Utils::bytesToString(totalBytesFreed));
        LOG("Total Allocations: " + TS(totalAllocations));
        LOG("Total Deallocations: " + TS(totalDeallocations));
        LOG("------------------------------------------");
    }


} } // end namespaces