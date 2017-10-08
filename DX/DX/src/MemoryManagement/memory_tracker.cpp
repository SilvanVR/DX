#include "memory_tracker.h"

/**********************************************************************
    class: MemoryTracker + GlobalAllocator (memory_tracker.cpp)

    author: S. Hau
    date: October 7, 2017
**********************************************************************/

#include "Utils/utils.h"

#include <iostream>

namespace MemoryManagement
{

    AllocationMemoryInfo MemoryTracker::s_memoryInfo;

    //----------------------------------------------------------------------
    void* _GlobalNewAndDeleteAllocator::allocate(Size size)
    {
        Size allocationSize = size + sizeof( Size );
        Byte* mem = (Byte*) std::malloc( allocationSize );

        memset( mem, 0, allocationSize);

        // [&AllocationSize - &RealMemory]
        memcpy( mem, &allocationSize, sizeof( Size ));
        mem += sizeof( Size );

        MemoryTracker::s_memoryInfo.addAllocation( allocationSize );

        return mem;
    }

    //----------------------------------------------------------------------
    void _GlobalNewAndDeleteAllocator::deallocate(void* memory)
    {
        Byte* mem = reinterpret_cast<Byte*>( memory );
        mem -= sizeof( Size );

        Size allocatedSize = *(reinterpret_cast<Size*>( mem ));
        MemoryTracker::s_memoryInfo.removeAllocation( allocatedSize );

        std::free( mem );
    }

    //----------------------------------------------------------------------
    void* _GlobalNewAndDeleteAllocator::allocateDebug(Size size, const char* file, U32 line)
    {
        // TODO LOG LARGE ALLOCATIONS
        //if (size > 1024 * 1024)
        //    Logger::Log("Large allocation (>1mb) " + std::string(file) + ":" + std::to_string(line), LOGTYPE_WARNING);

        return allocate( size );
    }

    //----------------------------------------------------------------------
    void _GlobalNewAndDeleteAllocator::deallocateDebug(void* mem, const char* file, U32 line)
    {
        deallocate( mem );
    }

    //----------------------------------------------------------------------
    void MemoryTracker::log()
    {
        //@TODO add to Logging Mechanism

        // It's important to fetch a local copy of the mem-info, otherwise the 
        // dynamically allocated string stuff will mess up the result
        AllocationMemoryInfo memInfo = MemoryTracker::getAllocationMemoryInfo();

        std::cout << ("-------------- MEMORY INFO ---------------") << std::endl;
        std::cout << ("Current Allocated: " + Utils::bytesToString(memInfo.currentBytesAllocated)) << std::endl;
        std::cout << ("Total Allocated: " + Utils::bytesToString(memInfo.totalBytesAllocated)) << std::endl;
        std::cout << ("Total Freed: " + Utils::bytesToString(memInfo.totalBytesFreed)) << std::endl;
        std::cout << ("Total Allocations: " + TS(memInfo.totalAllocations)) << std::endl;
        std::cout << ("Total Deallocations: " + TS(memInfo.totalDeallocations)) << std::endl;
        std::cout << ("------------------------------------------") << std::endl;
    }

}