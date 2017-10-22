#pragma once
/**********************************************************************
    class: Thread (thread.h)
    
    author: S. Hau
    date: October 21, 2017

    See below for a class description.

**********************************************************************/


#include <thread>
//#include <mutex>
//#include <condition_variable>


namespace Core { namespace OS {


    //**********************************************************************
    // Represents a thread in the system. Run's concurrently to the main thread.
    //**********************************************************************
    class Thread
    {
    public:
        Thread();
        ~Thread();

        //----------------------------------------------------------------------
        // Wait for this thread to finish his current job
        //----------------------------------------------------------------------
        void waitIdle();

        //----------------------------------------------------------------------
        // Check whether this thread currently executes a job (is idle)
        //----------------------------------------------------------------------
        bool hasJob() const { return m_currentJob != nullptr; }
        bool isIdle() const { return !hasJob(); }

    private:
        // Order of initialization matters. "m_running" has to be set to true first,
        bool                    m_running       = true;
        std::function<void()>   m_currentJob    = nullptr;
        std::thread             m_thread        = std::thread( &Thread::_ThreadLoop, this );

        //----------------------------------------------------------------------
        void _ThreadLoop();

        //----------------------------------------------------------------------
        Thread(const Thread& other)                 = delete;
        Thread& operator = (const Thread& other)    = delete;
        Thread(Thread&& other)                      = delete;
        Thread& operator = (Thread&& other)         = delete;
    };


} } // end namespaces