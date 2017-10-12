#include "memory_manager.h"

/**********************************************************************
    class: MemoryManager (memory_manager.cpp)

    author: S. Hau
    date: October 12, 2017

**********************************************************************/

#include "Core/locator.h"
#include "memory_tracker.h"

namespace Core { namespace MemoryManagement {

    //----------------------------------------------------------------------
    void MemoryManager::init()
    {
        m_staticAllocationInfo = MemoryTracker::getAllocationMemoryInfo();
    }

    //----------------------------------------------------------------------
    void MemoryManager::shutdown()
    {
        auto currentAllocationInfo = getAllocationInfo();

        if (currentAllocationInfo.currentBytesAllocated != 0)
            _ReportMemoryLeak();
    }

    //----------------------------------------------------------------------
    void MemoryManager::log()
    {
         getAllocationInfo().log();
    }

    //----------------------------------------------------------------------
    const AllocationMemoryInfo MemoryManager::getAllocationInfo() const
    {
        return (MemoryTracker::getAllocationMemoryInfo() - m_staticAllocationInfo);
    }

    //----------------------------------------------------------------------
    void MemoryManager::_ReportMemoryLeak()
    {
        WARN_MEMORY( "<<<< MemoryLeak detected somewhere! >>>>\nPrinting current memory info:" );
        log();

    #ifdef _WIN32
        // Memory leak somewhere
        __debugbreak();
    #elif
        ASSERT( false && "MemoryLeak detected somewhere." );
    #endif
    }


} } // end namespaces