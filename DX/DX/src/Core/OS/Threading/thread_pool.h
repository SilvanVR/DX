#pragma once

/**********************************************************************
    class: ThreadPool (thread_pool.h)
    
    author: S. Hau
    date: October 21, 2017

    Manages a bunch of threads via a job system. A job is basically
    just a function. A job will be executed by an arbitrary thread.
    When adding a new job the job itself will be returned, so the
    calling thread can wait until this specific job has been executed.
    @Considerations:
      - Support "Persistens Jobs", aka jobs running in a while(true) loop.
        For now all jobs have to have a clear end.
      - Add a batch of jobs simultanously. Wait for a batch.
      - Fetch memory for jobs from an custom allocator
**********************************************************************/

#include "thread.h"

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    #define MAX_POSSIBLE_THREADS        32


    //**********************************************************************
    // Manages a bunch of threads.
    //**********************************************************************
    class ThreadPool
    {
        static U8 s_maxHardwareConcurrency;

    public:
        //----------------------------------------------------------------------
        // Create a new threadpool with the specified amount of threads.
        // @Params:
        //  "numThreads": Number of threads to create.
        //----------------------------------------------------------------------
        ThreadPool(U8 numThreads = (s_maxHardwareConcurrency - 1));
        ~ThreadPool();

        //----------------------------------------------------------------------
        U8 numThreads()             const { return m_numThreads; }
        U8 maxHardwareConcurrency() const { return s_maxHardwareConcurrency; }

        //----------------------------------------------------------------------
        // Waits until all jobs has been executed. After this call, all threads
        // will be idle and no job is left in the queue.
        //----------------------------------------------------------------------
        void waitForThreads();

        //----------------------------------------------------------------------
        // Adds a new job. The job will executed by an arbitrary thread.
        // @Params:
        //  "job": Job/Task to execute.
        //----------------------------------------------------------------------
        JobPtr addJob(const std::function<void()>& job);


        //----------------------------------------------------------------------
        Thread& operator[] (U32 index){ ASSERT( index < m_numThreads ); return (*m_threads[index]); }

    private:
        Thread*         m_threads[MAX_POSSIBLE_THREADS];
        U8              m_numThreads;
        JobQueue        m_jobQueue;


        //----------------------------------------------------------------------
        // Wait until all threads have finished their execution and terminate them.
        //----------------------------------------------------------------------
        void _TerminateThreads();

        //----------------------------------------------------------------------
        ThreadPool(const ThreadPool& other)                 = delete;
        ThreadPool& operator = (const ThreadPool& other)    = delete;
        ThreadPool(ThreadPool&& other)                      = delete;
        ThreadPool& operator = (ThreadPool&& other)         = delete;
    };


} } // end namespaces
