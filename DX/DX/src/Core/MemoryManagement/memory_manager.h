#pragma once

/**********************************************************************
    class: MemoryManager (memory_manager.h)

    author: S. Hau
    date: October 12, 2017

    Reports memory leaks on shutdown.
    @Considerations
      - Check for memory leak while the program is running
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
        void shutdown() override;

        //----------------------------------------------------------------------
        // Log the current allocation info to the console (exclude static allocs)
        //----------------------------------------------------------------------
        void log();

        //----------------------------------------------------------------------
        // @Return:
        //   Contains all allocations made with global new/delete
        //----------------------------------------------------------------------
        const AllocationMemoryInfo& getGlobalAllocationInfo() const;

        //----------------------------------------------------------------------
        // @Return: 
        //   Contains all allocations made before this class was initialized
        //   These are most likely allocations made from static objects.
        //----------------------------------------------------------------------
        const AllocationMemoryInfo& getStaticAllocationInfo() const { return m_staticAllocationInfo; }

        //----------------------------------------------------------------------
        // @Return:
        //   Contains all allocations made after this class was initialized
        //----------------------------------------------------------------------
        const AllocationMemoryInfo getAllocationInfo() const;

    private:
        // Contains all allocations made before this class was initialized
        AllocationMemoryInfo m_staticAllocationInfo;

        //----------------------------------------------------------------------
        void _ReportMemoryLeak();

        //----------------------------------------------------------------------
        MemoryManager(const MemoryManager& other)               = delete;
        MemoryManager& operator = (const MemoryManager& other)  = delete;
        MemoryManager(MemoryManager&& other)                    = delete;
        MemoryManager& operator = (MemoryManager&& other)       = delete;
    };






} }