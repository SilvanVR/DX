#pragma once

/**********************************************************************
    class: ThreadPool (thread_pool.h)
    
    author: S. Hau
    date: October 21, 2017

    See below for a class description.
**********************************************************************/

#include "thread.h"

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    #define MAX_POSSIBLE_THREADS        32
    #define MAX_HARDWARE_CONCURRENCY    0


    //**********************************************************************
    // Manages a bunch of threads.
    //**********************************************************************
    class ThreadPool
    {
    public:
        ThreadPool(U8 numThreads = MAX_HARDWARE_CONCURRENCY);
        ~ThreadPool();

        //----------------------------------------------------------------------
        // Wait until all threads have finished their execution.
        //----------------------------------------------------------------------
        void waitForThreads();

        //----------------------------------------------------------------------
        Thread& operator[] (U32 index){ ASSERT( index < m_numThreads ); return (*m_threads[index]); }


    private:
        Thread* m_threads[MAX_POSSIBLE_THREADS];
        U8      m_numThreads;

        //----------------------------------------------------------------------
        ThreadPool(const ThreadPool& other)                 = delete;
        ThreadPool& operator = (const ThreadPool& other)    = delete;
        ThreadPool(ThreadPool&& other)                      = delete;
        ThreadPool& operator = (ThreadPool&& other)         = delete;
    };


} } // end namespaces
