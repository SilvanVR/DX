#include "memory_tracker.h"

/**********************************************************************
    class: MemoryTracker + GlobalAllocator (memory_tracker.cpp)

    author: S. Hau
    date: October 7, 2017
**********************************************************************/

#include "Core/locator.h"

namespace Core { namespace MemoryManagement {

    AllocationMemoryInfo MemoryTracker::s_memoryInfo;

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

        MemoryTracker::s_memoryInfo.addAllocation( allocationSize );

        return mem;
    }

    //----------------------------------------------------------------------
    void _GlobalNewAndDeleteAllocator::deallocate( void* memory )
    {
        Byte* mem = reinterpret_cast<Byte*>( memory );
        mem -= sizeof( Size );

        Size allocatedSize = *(reinterpret_cast<Size*>( mem ));
        MemoryTracker::s_memoryInfo.removeAllocation( allocatedSize );

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
        memInfo.log();
    }

    //----------------------------------------------------------------------
    MemoryTracker MemoryTracker::s_memoryLeakDetectionInstance;

    void MemoryTracker::_CheckForMemoryLeak()
    {
#ifdef _WIN32
        if( s_memoryInfo.currentBytesAllocated != 0 )
            __debugbreak();
#elif
        ASSERT( false && "Memory Leak detected somewhere");

#endif // _WIN32

    }

} } // end namespaces