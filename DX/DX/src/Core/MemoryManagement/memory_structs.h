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
        U64 currentBytesAllocated   = 0;
        U64 totalBytesAllocated     = 0;
        U64 totalBytesFreed         = 0;
        U64 totalAllocations        = 0;
        U64 totalDeallocations      = 0;

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

        // Log to console
        void log() const;

        AllocationMemoryInfo operator - (const AllocationMemoryInfo& other) const
        {
            AllocationMemoryInfo result = {};

            result.currentBytesAllocated = currentBytesAllocated - other.currentBytesAllocated;
            result.totalBytesAllocated   = totalBytesAllocated - other.totalBytesAllocated;
            result.totalBytesFreed       = totalBytesFreed - other.totalBytesFreed;
            result.totalAllocations      = totalAllocations - other.totalAllocations;
            result.totalDeallocations    = totalDeallocations - other.totalDeallocations;

            return result;
        }

        AllocationMemoryInfo operator + (const AllocationMemoryInfo& other) const
        {
            AllocationMemoryInfo result = {};

            result.currentBytesAllocated = currentBytesAllocated + other.currentBytesAllocated;
            result.totalBytesAllocated   = totalBytesAllocated + other.totalBytesAllocated;
            result.totalBytesFreed       = totalBytesFreed + other.totalBytesFreed;
            result.totalAllocations      = totalAllocations + other.totalAllocations;
            result.totalDeallocations    = totalDeallocations + other.totalDeallocations;

            return result;
        }
    };

} } // end namespaces
