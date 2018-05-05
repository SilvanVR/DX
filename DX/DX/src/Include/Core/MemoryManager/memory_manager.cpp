#include "memory_manager.h"

/**********************************************************************
    class: MemoryManager (memory_manager.cpp)

    author: S. Hau
    date: October 12, 2017

    So let's talk about memory leak detection for a moment. The problem
    is how to prevent dynamic memory allocation within the core loop.
    Technically it is possible, but not if you gonna use stuff from the STL.
    I investigated two solutions:
    1.) You could write custom allocators for each STL class, this
        would work but is a very tedious task and you would have to
        write some STL function yourself e.g. std::to_string(), because 
        they return an object with the default allocator.
        Before doing that i would prefer writing own classes (as EA did with the eastl).
    2.) You could use a static instance of an allocator which preallocates
        a large chunk of memory and allocate from it within the global
        new + delete. But it's not worth to do this because it
        could decrease performance tremendously (Custom allocator overhead, 
        thread-safety via mutex and rewrite of the allocator to not do dynamic allocations itself).
    So yea i accept the dynamic allocations from the STL within the core
    loop. They won't happen every frame/gametick so i guess it's still a
    viable solution.
    Nonetheless the MemoryManager tries to detect possible memory leaks,
    whilst keeping an eye on the global new/delete allocations.
**********************************************************************/

#include "Core/locator.h"
#include "Logging/logging.h"
#include "memory_tracker.h"
#include "Common/utils.h"

#define REPORT_CONTINOUS_ALLOCATIONS 0

namespace Core { namespace MemoryManagement {

    //----------------------------------------------------------------------
    void MemoryManager::init()
    {
        Locator::getCoreEngine().subscribe( this );
    }

    //----------------------------------------------------------------------
    void MemoryManager::OnTick( Time::Seconds delta )
    {
        _BasicLeakDetection();
        //_ContinousAllocationLeakDetection();
    }

    //----------------------------------------------------------------------
    void MemoryManager::shutdown()
    {
        auto currentAllocationInfo = getAllocationInfo();
        LOG( "Allocations made throughout the program:" + currentAllocationInfo.toString() );
    }

    //----------------------------------------------------------------------
    void MemoryManager::log()
    {
         LOG( getAllocationInfo().toString() );
    }

    //----------------------------------------------------------------------
    const Memory::AllocationInfo MemoryManager::getAllocationInfo() const
    {
        return MemoryTracker::getAllocationMemoryInfo();
    }


    //----------------------------------------------------------------------
    void MemoryManager::_ContinousAllocationLeakDetection()
    {
        // Create viable solution for detecting memory leaks
    }


    //----------------------------------------------------------------------
    void MemoryManager::_BasicLeakDetection()
    {
        static Memory::AllocationInfo lastAllocInfo;
        auto allocInfo = getAllocationInfo();

#if REPORT_CONTINOUS_ALLOCATIONS
        // Problem: STL stuff allocates dynamically. Mainly string stuff e.g. std::to_string() when logging.
        if ((lastAllocInfo.totalBytesAllocated != 0) && (lastAllocInfo.totalBytesAllocated != allocInfo.totalBytesAllocated))
        {
            WARN_MEMORY("Continous dynamic memory allocations in game-loop!");
            WARN_MEMORY("Total Bytes Allocated Last Time: " + TS(lastAllocInfo.totalBytesAllocated));
            WARN_MEMORY("Total Bytes Allocated Now: " + TS(allocInfo.totalBytesAllocated));
        }
#endif

        // Check if this time the amount of bytes increased. If so there was a new allocation.
        if ( (lastAllocInfo.bytesAllocated < allocInfo.bytesAllocated) && (lastAllocInfo.bytesAllocated != 0) )
        {
            _ReportPossibleMemoryLeak( lastAllocInfo, allocInfo );
        }

        // Print messages allocates itself memory because std::to_string, that's why retrieve the current alloc info again
        allocInfo = getAllocationInfo();
        lastAllocInfo = allocInfo;
    }

    //----------------------------------------------------------------------
    void MemoryManager::_ReportPossibleMemoryLeak( const Memory::AllocationInfo& lastAllocationInfo, const Memory::AllocationInfo& allocInfo )
    {
        auto bytesAllocated = allocInfo.bytesAllocated - lastAllocationInfo.bytesAllocated;
        //LOG_WARN_MEMORY( "<<<< Dynamic Memory Allocation in Game-Loop: Bytes allocated: " + Utils::bytesToString( bytesAllocated ) );
    }


} } // end namespaces