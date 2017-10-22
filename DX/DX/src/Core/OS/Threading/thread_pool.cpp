#include "thread_pool.h"

/**********************************************************************
    class: ThreadPool (thread_pool.cpp)

    author: S. Hau
    date: October 21, 2017
**********************************************************************/

#include "jobs/job_queue.h"

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
        _TerminateThreads();
    }

    //----------------------------------------------------------------------
    void ThreadPool::addJob( const std::function<void()>& job, const std::function<void()>& calledWhenDone )
    {
        // Add job to job queue
        m_jobQueue.addJob( { job, calledWhenDone } );
    }

    //----------------------------------------------------------------------
    void ThreadPool::_TerminateThreads()
    {
        // Add termination jobs to the queue.
        // They will be picked up by any thread to terminate itself.
        for (U8 i = 0; i < m_numThreads; i++)
            m_jobQueue.addJob( nullptr );

        // Wait until job queue is empty
        m_jobQueue.waitUntilQueueIsEmpty();

        // Destroy threads
        for (U8 i = 0; i < m_numThreads; i++)
        {
            delete m_threads[i];
            m_threads[i] = nullptr;
        }
    }


} } // end namespaces