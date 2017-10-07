#pragma once

/**********************************************************************
    class: MemoryTracker + GlobalAllocator (memory_tracker.h)

    author: S. Hau
    date: October 7, 2017

    Tracks all allocated memory from global new/delete. 
    Note that libs as a DLL does not account to that.
    @TODO:
      - Static libraries will???
**********************************************************************/

namespace MemoryManagement
{

    struct MemoryInfo
    {
        U64 currentBytesAllocated;
        U64 totalBytesAllocated;
        U64 totalBytesFreed;
        U64 totalAllocations;
        U64 totalDeallocations;
    };

    //**********************************************************************
    // Global new/delete call this functions in order to allocate/deallocate.
    // Manipulates the MemoryInfo struct in the MemoryTracker class.
    //**********************************************************************

    class _GlobalNewAndDeleteAllocator
    {
    public:
        static void* allocate(Size size);
        static void  deallocate(void* mem);

        static void* allocateDebug(Size size, const char* file, U32 line);
        static void  deallocateDebug(void* mem, const char* file, U32 line);
    };

    //**********************************************************************
    // Keeps track of all GLOBAL allocations / deallocations.
    //**********************************************************************

    class MemoryTracker
    {
        friend class _GlobalNewAndDeleteAllocator;

    public:
        //----------------------------------------------------------------------
        // Return the memory information struct. Information in 
        // that struct was gathered through global new/delete.
        //----------------------------------------------------------------------
        static const MemoryInfo& getMemoryInfo() { return memoryInfo; }

        //----------------------------------------------------------------------
        // Log the MemoryInfo
        //----------------------------------------------------------------------
        static void log();

    private:
        // Be careful. The memory is automatically zeroed out, because its static.
        static MemoryInfo memoryInfo;
    };


}