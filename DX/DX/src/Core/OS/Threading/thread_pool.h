#pragma once

/**********************************************************************
    class: ThreadPool (thread_pool.h)
    
    author: S. Hau
    date: October 21, 2017

    Manages a bunch of threads via a job system. A job is basically
    comprised of two functions. The first is the job itself,
    and the latter a function which will be called when the job is done.
    A job will be executed by an arbitrary thread.
    @Considerations:
      - Give each job an ID, wait until a job has been finished:
         JobID id = pool.addJob(...);
         pool.waitForJob( id );
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
        U8 numThreads() const { return m_numThreads; }
        U8 maxHardwareConcurrency() const { return s_maxHardwareConcurrency; }

        //----------------------------------------------------------------------
        // Adds a new job. The job will executed by an arbitrary thread. When
        // the job is done, a callback will be called if desired.
        // @Params:
        //  "job": Job/Task to execute.
        //  "calledWhenDone": Function to be called by the main thread 
        //                    when the job is done.
        //----------------------------------------------------------------------
        void addJob(const std::function<void()>& job, const std::function<void()>& calledWhenDone = nullptr);

        //----------------------------------------------------------------------
        // Wait until all threads have finished their execution and all jobs are done.
        //----------------------------------------------------------------------
        void waitForThreads();

        //----------------------------------------------------------------------
        Thread& operator[] (U32 index){ ASSERT( index < m_numThreads ); return (*m_threads[index]); }


    private:
        Thread*         m_threads[MAX_POSSIBLE_THREADS];
        U8              m_numThreads;
        JobQueue        m_jobQueue;


        //----------------------------------------------------------------------
        ThreadPool(const ThreadPool& other)                 = delete;
        ThreadPool& operator = (const ThreadPool& other)    = delete;
        ThreadPool(ThreadPool&& other)                      = delete;
        ThreadPool& operator = (ThreadPool&& other)         = delete;
    };


} } // end namespaces
