#include "thread_pool.h"

/**********************************************************************
    class: ThreadPool (thread_pool.cpp)
    
    author: S. Hau
    date: October 21, 2017
**********************************************************************/

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    ThreadPool::ThreadPool( U8 numThreads )
        : m_numThreads( numThreads )
    {
        if (m_numThreads == MAX_HARDWARE_CONCURRENCY)
            m_numThreads = std::thread::hardware_concurrency();

        ASSERT( m_numThreads < MAX_POSSIBLE_THREADS );

        // Create threads
        for (U8 i = 0; i < m_numThreads; i++)
        {
            m_threads[i] = new Thread();
        }
    }

    //----------------------------------------------------------------------
    ThreadPool::~ThreadPool()
    {
        waitForThreads();

        // Destroy threads
        for (U8 i = 0; i < m_numThreads; i++)
        {
            delete m_threads[i];
            m_threads[i] = nullptr;
        }
    }

    //----------------------------------------------------------------------
    void ThreadPool::addJob( const std::function<void()>& job, const std::function<void()>& calledWhenDone )
    {
        // Add job to job queue
        m_jobs.push( std::move( job ) );

        // Try to wake up a thread, which will fetch a job from the queue

    }


    //----------------------------------------------------------------------
    void ThreadPool::waitForThreads()
    {
        for (U8 i = 0; i < m_numThreads; i++)
        {
            m_threads[i]->waitIdle();
        }
    }


} } // end namespaces