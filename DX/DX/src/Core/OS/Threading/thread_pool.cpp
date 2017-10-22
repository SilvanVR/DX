#include "thread_pool.h"

/**********************************************************************
    class: ThreadPool (thread_pool.cpp)

    author: S. Hau
    date: October 21, 2017
**********************************************************************/

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    U8 ThreadPool::s_maxHardwareConcurrency = std::thread::hardware_concurrency();

    //----------------------------------------------------------------------
    ThreadPool::ThreadPool( U8 numThreads )
        : m_numThreads( numThreads )
    {
        ASSERT( (m_numThreads > 0) && (m_numThreads < MAX_POSSIBLE_THREADS) );

        // Create threads
        for (U8 i = 0; i < m_numThreads; i++)
        {
            m_threads[i] = new Thread( m_jobQueue );
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
        m_jobQueue.addJob( { job, calledWhenDone } );
    }

    //----------------------------------------------------------------------
    void ThreadPool::waitForThreads()
    {
        // Wait until job queue is empty
        m_jobQueue.waitIsEmpty();

        // Now wait until all jobs are done
        for (U8 i = 0; i < m_numThreads; i++)
        {
            m_threads[i]->waitIdle();
        }
    }


} } // end namespaces