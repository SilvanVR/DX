#pragma once

/**********************************************************************
    class: None (memory_structs.h)

    author: S. Hau
    date: October 7, 2017

    Defines Structs for tracking memory stuff.
**********************************************************************/

namespace MemoryManagement
{

    //----------------------------------------------------------------------
    // Stores information about allocations e.g. currentBytesAllocated.
    //----------------------------------------------------------------------
    struct AllocationMemoryInfo
    {
        U64 currentBytesAllocated;
        U64 totalBytesAllocated;
        U64 totalBytesFreed;
        U64 totalAllocations;
        U64 totalDeallocations;

        void addAllocation(Size amtOfBytes)
        {
            currentBytesAllocated += amtOfBytes;
            totalBytesAllocated += amtOfBytes;
            totalAllocations++;
        }

        void removeAllocation(Size amtOfBytes)
        {
            currentBytesAllocated -= amtOfBytes;
            totalBytesFreed += amtOfBytes;
            totalDeallocations++;
        }
    };


}
