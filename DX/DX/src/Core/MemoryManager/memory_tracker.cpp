#include "memory_tracker.h"

/**********************************************************************
    class: MemoryTracker + GlobalAllocator (memory_tracker.cpp)

    author: S. Hau
    date: October 7, 2017
**********************************************************************/

#include "logging.h"
#include "memory.hpp"

namespace Core { namespace MemoryManagement {

    //----------------------------------------------------------------------
    std::mutex& getMutex()
    {
        static std::mutex mutex; // Ensures that the mutex will be first initialized when needed
        return mutex;
    }

    //----------------------------------------------------------------------
    void* _GlobalNewAndDeleteAllocator::allocate( Size size )
    {
        ASSERT ( size < ( 1024i64 * 1024i64 * 1024i64 * 32i64) ); // Less than 32GB

        Size allocationSize = size + sizeof( Size );
        Byte* mem = (Byte*) std::malloc( allocationSize );

        memset( mem, 0, allocationSize );

        // [&AllocationSize - &RealMemory]
        memcpy( mem, &allocationSize, sizeof( Size ));
        mem += sizeof( Size );

        std::lock_guard<std::mutex> lock( getMutex() );
        MemoryTracker::getAllocationMemoryInfo().addAllocation( allocationSize );

        return mem;
    }

    //----------------------------------------------------------------------
    void _GlobalNewAndDeleteAllocator::deallocate( void* memory )
    {
        if (memory == nullptr)
            return;

        Byte* mem = reinterpret_cast<Byte*>( memory );
        mem -= sizeof( Size );

        Size allocatedSize = *(reinterpret_cast<Size*>( mem ));
        std::lock_guard<std::mutex> lock( getMutex() );
        MemoryTracker::getAllocationMemoryInfo().removeAllocation( allocatedSize );

        std::free( mem );
    }

    //----------------------------------------------------------------------
    void* _GlobalNewAndDeleteAllocator::allocateDebug( Size size, const char* file, U32 line )
    {
        if (size > (1024 * 1024))
            WARN ( "Large allocation (>1mb) " + String( file ) + ":" + TS( line ) );

        return allocate( size );
    }

    //----------------------------------------------------------------------
    void _GlobalNewAndDeleteAllocator::deallocateDebug( void* mem, const char* file, U32 line )
    {
        deallocate( mem );
    }

    //----------------------------------------------------------------------
    void MemoryTracker::log()
    {
        // It's important to fetch a local copy of the mem-info, otherwise the 
        // dynamically allocated string stuff will mess up the result
        AllocationMemoryInfo memInfo = MemoryTracker::getAllocationMemoryInfo();
        LOG( memInfo.toString() );
    }

    //----------------------------------------------------------------------
#ifndef STATIC_LIB
    MemoryTracker MemoryTracker::s_memoryLeakDetectionInstance;
#endif

    void MemoryTracker::_CheckForMemoryLeak()
    {
#ifdef _WIN32
        auto memInfo = getAllocationMemoryInfo();
        if (memInfo.bytesAllocated != 0)
        {
            printf( "Current bytes allocated: %lld \n", memInfo.bytesAllocated);
            printf( "Num allocations left: %lld", memInfo.totalAllocations - memInfo.totalDeallocations );
            __debugbreak();
        }
#elif
        ASSERT( false && "Memory Leak detected somewhere");

#endif // _WIN32

    }

} } // end namespaces