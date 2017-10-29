#pragma once

/**********************************************************************
    class: MemoryManager (memory_manager.h)

    author: S. Hau
    date: October 12, 2017

    Reports memory leaks on shutdown.
    @Considerations
      - Allocations from Allocators fetch there memory from a
        universalalloctor in this class?
         => Preallocate a large chunk of memory
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "memory_structs.h"


namespace Core { namespace MemoryManagement{

    //*********************************************************************
    // MemoryManager class, which tracks memory allocations.
    //*********************************************************************
    class MemoryManager : public ISubSystem
    {
    public:
        MemoryManager() {}

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void update(F32 delta) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // Log the current allocation info to the console.
        //----------------------------------------------------------------------
        void log();

        //----------------------------------------------------------------------
        // @Return:
        //   Contains all allocations made in this program.
        //----------------------------------------------------------------------
        const AllocationMemoryInfo getAllocationInfo() const;

    private:
        //----------------------------------------------------------------------
        void _ReportPossibleMemoryLeak(const AllocationMemoryInfo& lastAllocationInfo, const AllocationMemoryInfo& allocationInfo);

        // List here different strategies for memory leak detection
        void _ContinousAllocationLeakDetection();
        void _BasicLeakDetection();

        //----------------------------------------------------------------------
        MemoryManager(const MemoryManager& other)               = delete;
        MemoryManager& operator = (const MemoryManager& other)  = delete;
        MemoryManager(MemoryManager&& other)                    = delete;
        MemoryManager& operator = (MemoryManager&& other)       = delete;
    };






} }