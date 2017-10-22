#pragma once
/**********************************************************************
    class: JobQueue + Job (job_queue.h)
    
    author: S. Hau
    date: October 22, 2017

    See below for a class description.

**********************************************************************/

#include <thread>
#include <mutex>
#include <condition_variable>

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    class Job
    {
    public:
        Job(std::nullptr_t null) 
            : Job(null, null) {}
        Job(const std::function<void()>& job = nullptr, const std::function<void()>& func = nullptr)
            : job( job ), calledWhenJobDone( func ) {}

        bool operator == (std::nullptr_t null) const { return job == null; }
        bool operator != (std::nullptr_t null) const { return job != null; }

        void operator() () 
        {
            job();

            if (calledWhenJobDone != nullptr)
                calledWhenJobDone();
        }

    private:
        std::function<void()> job;
        std::function<void()> calledWhenJobDone;
    };

    //**********************************************************************
    // Contains a queue in which jobs can be put in. When a job is added
    // a thread will be notified to execute it. If no thread is idle
    // the job's execution will be deferred until a thread becomes idle.
    //**********************************************************************
    class JobQueue
    {
    public:
        JobQueue() = default;

        void addJob(const Job& job)
        {
            {
                std::unique_lock<std::mutex> lock( m_mutex );
                m_jobs.push( std::move( job ) );
            }

            m_jobCV.notify_one();
        }

        Job grabJob()
        {
            // Lock access to queue
            std::unique_lock<std::mutex> lock( m_mutex );

            // Wait if no job is available or the thread should not be terminated
            m_jobCV.wait( lock, [this]() -> bool { 
                return !m_jobs.empty() || m_terminate; 
            } );

            if ( m_jobs.empty() )
                return nullptr;

            // Retrieve next job
            Job job = m_jobs.front();
            m_jobs.pop();

            if ( m_jobs.empty() )
                m_isEmptyCV.notify_all();

            return std::move( job );
        }

        void waitIsEmpty()
        {
            std::unique_lock<std::mutex> lock( m_mutex );
            m_isEmptyCV.wait( lock, [this]() -> bool { return m_jobs.empty(); } );
        }

        void notifyAll()
        {
            m_terminate = true;
            m_jobCV.notify_all();
        }

    private:
        bool                    m_terminate = false;
        std::queue<Job>         m_jobs;
        std::mutex              m_mutex;
        std::condition_variable m_jobCV;
        std::condition_variable m_isEmptyCV;

        //----------------------------------------------------------------------
        JobQueue(const JobQueue& other)                 = delete;
        JobQueue& operator = (const JobQueue& other)    = delete;
        JobQueue(JobQueue&& other)                      = delete;
        JobQueue& operator = (JobQueue&& other)         = delete;
    };


} } // end namespaces