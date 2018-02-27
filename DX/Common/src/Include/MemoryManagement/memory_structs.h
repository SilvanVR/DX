#pragma once

/**********************************************************************
    class: None (memory_structs.h)

    author: S. Hau
    date: October 7, 2017

    Defines Structs for tracking memory stuff.
**********************************************************************/

namespace Core { namespace MemoryManagement {

    //----------------------------------------------------------------------
    // Stores information about allocations e.g. currentBytesAllocated.
    //----------------------------------------------------------------------
    struct AllocationMemoryInfo
    {
        U64 bytesAllocated          = 0;
        U64 totalBytesAllocated     = 0;
        U64 totalBytesFreed         = 0;
        U64 totalAllocations        = 0;
        U64 totalDeallocations      = 0;

        //----------------------------------------------------------------------
        void addAllocation(Size amtOfBytes);
        void removeAllocation(Size amtOfBytes);

        String toString() const;

        AllocationMemoryInfo operator - (const AllocationMemoryInfo& other) const;
        AllocationMemoryInfo operator + (const AllocationMemoryInfo& other) const;
    };

} } // end namespaces
