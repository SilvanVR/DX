#include "memory_structs.h"

/**********************************************************************
    class: None (memory_structs.cpp)

    author: S. Hau
    date: October 12, 2017

**********************************************************************/

#include "Common/utils.h"

namespace Memory {

    //----------------------------------------------------------------------
    void AllocationInfo::addAllocation( Size amtOfBytes )
    {
        bytesAllocated += amtOfBytes;
        totalBytesAllocated += amtOfBytes;
        totalAllocations++;
    }

    //----------------------------------------------------------------------
    void AllocationInfo::removeAllocation( Size amtOfBytes )
    {
        ASSERT( totalDeallocations != totalAllocations );
        bytesAllocated -= amtOfBytes;
        totalBytesFreed += amtOfBytes;
        totalDeallocations++;
    }

    //----------------------------------------------------------------------
    String AllocationInfo::toString() const
    {
        String result;
        result.reserve( 255 );

        result += ( "\n-------------- MEMORY INFO ---------------");
        result += ( "\nCurrent Allocated: " + Utils::bytesToString( bytesAllocated ) );
        result += ( "\nTotal Allocated: " + Utils::bytesToString( totalBytesAllocated ) );
        result += ( "\nTotal Freed: " + Utils::bytesToString( totalBytesFreed ) );
        result += ( "\nTotal Allocations: " + TS( totalAllocations ) );
        result += ( "\nTotal Deallocations: " + TS( totalDeallocations ) );
        result += ( "\n------------------------------------------\n" );

        return result;
    }

    //----------------------------------------------------------------------
    AllocationInfo AllocationInfo::operator - (const AllocationInfo& other) const
    {
        AllocationInfo result = {};

        result.bytesAllocated        = bytesAllocated - other.bytesAllocated;
        result.totalBytesAllocated   = totalBytesAllocated - other.totalBytesAllocated;
        result.totalBytesFreed       = totalBytesFreed - other.totalBytesFreed;
        result.totalAllocations      = totalAllocations - other.totalAllocations;
        result.totalDeallocations    = totalDeallocations - other.totalDeallocations;

        return result;
    }

    //----------------------------------------------------------------------
    AllocationInfo AllocationInfo::operator + (const AllocationInfo& other) const
    {
        AllocationInfo result = {};

        result.bytesAllocated        = bytesAllocated + other.bytesAllocated;
        result.totalBytesAllocated   = totalBytesAllocated + other.totalBytesAllocated;
        result.totalBytesFreed       = totalBytesFreed + other.totalBytesFreed;
        result.totalAllocations      = totalAllocations + other.totalAllocations;
        result.totalDeallocations    = totalDeallocations + other.totalDeallocations;

        return result;
    }

} // end namespaces