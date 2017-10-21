#pragma once
/**********************************************************************
    class: Thread (thread.h)
    
    author: S. Hau
    date: October 21, 2017

    See below for a class description.

**********************************************************************/


#include <thread>


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

    private:
        std::thread     m_thread;
        bool            m_running;

        void _ThreadLoop();

        //----------------------------------------------------------------------
        Thread(const Thread& other)                 = delete;
        Thread& operator = (const Thread& other)    = delete;
        Thread(Thread&& other)                      = delete;
        Thread& operator = (Thread&& other)         = delete;
    };


} } // end namespaces