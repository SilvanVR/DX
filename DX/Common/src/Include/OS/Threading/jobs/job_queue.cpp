#include "job_queue.h"
/**********************************************************************
    class: JobQueue (job_queue.cpp)
    
    author: S. Hau
    date: October 22, 2017
**********************************************************************/

namespace OS {

        //----------------------------------------------------------------------
        void JobQueue::addJob(JobPtr job)
        {
            // Add job to queue
            {
                std::unique_lock<std::mutex> lock( m_mutex );
                m_jobs.push( job );
            }

            // Notify a thread that a job is available
            m_jobCV.notify_one();
        }

        //----------------------------------------------------------------------
        JobPtr JobQueue::grabJob()
        {
            // Lock access to queue
            std::unique_lock<std::mutex> lock( m_mutex );

            // Wait if no job is available
            m_jobCV.wait( lock, [this]() -> bool { 
                return !m_jobs.empty(); 
            } );

            // Retrieve next job
            JobPtr job = m_jobs.front();
            m_jobs.pop();

            if ( m_jobs.empty() )
                m_isEmptyCV.notify_all();

            return job;
        }

        //----------------------------------------------------------------------
        void JobQueue::waitUntilQueueIsEmpty()
        {
            std::unique_lock<std::mutex> lock( m_mutex );
            m_isEmptyCV.wait( lock, [this]() -> bool { return m_jobs.empty(); } );
        }


} // end namespaces