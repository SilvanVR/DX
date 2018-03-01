#include "thread_pool.h"

/**********************************************************************
    class: ThreadPool (thread_pool.cpp)

    author: S. Hau
    date: October 21, 2017
**********************************************************************/

#include "jobs/job_queue.h"

namespace OS {

    //----------------------------------------------------------------------
    U8 ThreadPool::s_hardwareConcurrency = std::thread::hardware_concurrency();


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
    void ThreadPool::waitForThreads()
    {
        m_jobQueue.waitUntilQueueIsEmpty();

        for (U8 i = 0; i < m_numThreads; i++)
            m_threads[i]->waitForCurrentJob();
    }

    //----------------------------------------------------------------------
    JobPtr ThreadPool::addJob( const std::function<void()>& job )
    {
        // Add job to job queue
        JobPtr newJob = std::make_shared<Job>( std::move( job ) );
        m_jobQueue.addJob( newJob );

        return newJob;
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


} // end namespaces