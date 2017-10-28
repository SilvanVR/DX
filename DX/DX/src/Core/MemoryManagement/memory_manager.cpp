#include "memory_manager.h"

/**********************************************************************
    class: MemoryManager (memory_manager.cpp)

    author: S. Hau
    date: October 12, 2017

**********************************************************************/

#include "locator.h"
#include "memory_tracker.h"

namespace Core { namespace MemoryManagement {

    //----------------------------------------------------------------------
    void MemoryManager::init()
    {
        m_staticAllocationInfo = getGlobalAllocationInfo();
    }

    //----------------------------------------------------------------------
    void MemoryManager::update(F32 delta)
    {
        static const F32 tickRate = 0.5f;
        static F32 timer = 0;
        static U64 lastTimeBytesAllocated = 0;

        timer += delta;
        if (timer > tickRate)
        {
            auto allocInfo = getAllocationInfo();
            
            if ( (lastTimeBytesAllocated != 0) && (lastTimeBytesAllocated != allocInfo.totalBytesAllocated) )
            {
                WARN_MEMORY( "Maybe Memory-Leak detected!" );
                WARN_MEMORY( "Total Bytes Allocated Last Time: " + TS( lastTimeBytesAllocated ) );
                WARN_MEMORY( "Total Bytes Allocated Now: " + TS( allocInfo.totalBytesAllocated ) );
            }

            lastTimeBytesAllocated = getAllocationInfo().totalBytesAllocated;
            timer -= tickRate;
        }
    }

    //----------------------------------------------------------------------
    void MemoryManager::shutdown()
    {
        auto currentAllocationInfo = getAllocationInfo();
        LOG( currentAllocationInfo.toString() );

        if (currentAllocationInfo.currentBytesAllocated != 0)
            _ReportMemoryLeak( currentAllocationInfo );
        else
            LOG( " ~ No memory leak detected. Goodbye! ~ ", Color::GREEN );
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