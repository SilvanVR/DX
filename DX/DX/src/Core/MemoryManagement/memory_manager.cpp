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
        m_staticAllocationInfo = getGlobalAllocationInfo();
    }

    //----------------------------------------------------------------------
    void MemoryManager::shutdown()
    {
        auto currentAllocationInfo = getAllocationInfo();
        LOG( currentAllocationInfo.toString() );

        if (currentAllocationInfo.currentBytesAllocated != 0)
            _ReportMemoryLeak( currentAllocationInfo );
    }

    //----------------------------------------------------------------------
    void MemoryManager::log()
    {
         LOG( getAllocationInfo().toString() );
    }

    //----------------------------------------------------------------------
    const AllocationMemoryInfo& MemoryManager::getGlobalAllocationInfo() const
    {
        return MemoryTracker::getAllocationMemoryInfo();
    }

    //----------------------------------------------------------------------
    const AllocationMemoryInfo MemoryManager::getAllocationInfo() const
    {
        return (getGlobalAllocationInfo() - m_staticAllocationInfo);
    }

    //----------------------------------------------------------------------
    void MemoryManager::_ReportMemoryLeak(const AllocationMemoryInfo& allocationInfo)
    {
        WARN_MEMORY( "<<<< MemoryLeak detected somewhere! >>>>\nPrinting current memory info:" );
        LOG( allocationInfo.toString() );

    #ifdef _WIN32
        __debugbreak();
    #elif
        ASSERT( false && "MemoryLeak detected somewhere." );
    #endif
    }


} } // end namespaces