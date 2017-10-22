#pragma once
/**********************************************************************
    class: Thread (thread.h)
    
    author: S. Hau
    date: October 21, 2017

    See below for a class description.

**********************************************************************/

#include "jobs/job_queue.h"

namespace Core { namespace OS {


    //**********************************************************************
    // Represents a thread in the system. Run's concurrently to the main thread.
    //**********************************************************************
    class Thread
    {
        static U32 s_threadCounter;

    public:
        Thread(JobQueue& jobQueue);
        ~Thread();

        //----------------------------------------------------------------------
        // Check whether this thread currently executes a job (is not idle)
        //----------------------------------------------------------------------
        bool hasJob() const { return m_currentJob != nullptr; }
        bool isIdle() const { return !hasJob(); }

    private:
        // Order of initialization matters.
        U32                     m_threadID      = s_threadCounter++;
        Job                     m_currentJob    = nullptr;
        JobQueue&               m_jobQueue;

        // Initialize thread at last. !IMPORTANT!
        std::thread             m_thread        = std::thread( &Thread::_ThreadLoop, this );

        //----------------------------------------------------------------------
        void _ThreadLoop();
        void _Join();

        //----------------------------------------------------------------------
        Thread(const Thread& other)                 = delete;
        Thread& operator = (const Thread& other)    = delete;
        Thread(Thread&& other)                      = delete;
        Thread& operator = (Thread&& other)         = delete;
    };


} } // end namespaces