#pragma once

/**********************************************************************
    class: None (memory_structs.h)

    author: S. Hau
    date: October 7, 2017

    Defines Structs for tracking memory stuff.
**********************************************************************/

namespace Memory {

    //----------------------------------------------------------------------
    // Stores information about allocations e.g. currentBytesAllocated.
    //----------------------------------------------------------------------
    struct AllocationInfo
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

        AllocationInfo operator - (const AllocationInfo& other) const;
        AllocationInfo operator + (const AllocationInfo& other) const;
    };

} // end namespaces
