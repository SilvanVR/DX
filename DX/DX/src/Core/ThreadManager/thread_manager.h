#pragma once

/**********************************************************************
    class: ThreadManager (thread_manager.h)

    author: S. Hau
    date: October 22, 2017

    See below for a class description.

**********************************************************************/

#include "Core/OS/Threading/thread_pool.h"
#include "Core/i_subsystem.hpp"

namespace Core { namespace Threading {

    class ThreadManager : public ISubSystem
    {
    public:
        ThreadManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // Return the main engine.ini file-object
        //----------------------------------------------------------------------
        OS::ThreadPool& getThreadPool() { return m_threadPool; }

    private:
        OS::ThreadPool m_threadPool;

        //----------------------------------------------------------------------
        ThreadManager(const ThreadManager& other)               = delete;
        ThreadManager& operator = (const ThreadManager& other)  = delete;
        ThreadManager(ThreadManager&& other)                    = delete;
        ThreadManager& operator = (ThreadManager&& other)       = delete;
    };

} }