#include "memory_tracker.h"

/**********************************************************************
    class: MemoryTracker + GlobalAllocator (memory_tracker.cpp)

    author: S. Hau
    date: October 7, 2017
**********************************************************************/

#include "Logging/logging.h"
#include "memory.hpp"

#ifdef  _DEBUG
    #define TRACK_ALL_ALLOCATIONS 1
#else
    #define TRACK_ALL_ALLOCATIONS 1
#endif //  _DEBUG

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

#if TRACK_ALL_ALLOCATIONS // Track all memory in debug mode
        Size allocationSize = size + 2 * sizeof( Size );
        Byte* mem = (Byte*) std::malloc( allocationSize );

        memset( mem, 0, allocationSize );

        // [&AllocationSize - &RealMemory]
        memcpy( mem, &allocationSize, sizeof( Size ));
        mem += 2 * sizeof( Size ); // Keep allocations 16 byte aligned

        std::lock_guard<std::mutex> lock( getMutex() );
        MemoryTracker::getAllocationMemoryInfo().addAllocation( allocationSize );

        return mem;
#else
        return std::malloc( size );
#endif
    }

    //----------------------------------------------------------------------
    void _GlobalNewAndDeleteAllocator::deallocate( void* memory )
    {
        if (memory == nullptr)
            return;

#if TRACK_ALL_ALLOCATIONS
        Byte* mem = reinterpret_cast<Byte*>( memory );
        mem -= 2 * sizeof( Size );

        Size allocatedSize = *(reinterpret_cast<Size*>( mem ));
        std::lock_guard<std::mutex> lock( getMutex() );
        MemoryTracker::getAllocationMemoryInfo().removeAllocation( allocatedSize );
        std::free( mem );
#else
        std::free( memory );
#endif
    }

    //----------------------------------------------------------------------
    void* _GlobalNewAndDeleteAllocator::allocateDebug( Size size, const char* file, U32 line )
    {
        if (size > (1024 * 1024))
            LOG_WARN ( "Large allocation (>1mb) " + String( file ) + ":" + TS( line ) );

        return allocate( size );
    }

    //----------------------------------------------------------------------
    void _GlobalNewAndDeleteAllocator::deallocateDebug( void* mem, const char* file, U32 line )
    {
        deallocate( mem );
    }

    //----------------------------------------------------------------------
    Memory::AllocationInfo& MemoryTracker::getAllocationMemoryInfo() 
    {
        static Memory::AllocationInfo s_memoryInfo;
        return s_memoryInfo;
    }

    //----------------------------------------------------------------------
    void MemoryTracker::log()
    {
#if TRACK_ALL_ALLOCATIONS
        // It's important to fetch a local copy of the mem-info, otherwise the 
        // dynamically allocated string stuff will mess up the result
        Memory::AllocationInfo memInfo = MemoryTracker::getAllocationMemoryInfo();
        LOG( memInfo.toString() );
#else
        LOG( "Memory Tracking disabled." );
#endif
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