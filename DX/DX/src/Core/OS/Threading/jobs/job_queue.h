#pragma once
/**********************************************************************
    class: JobQueue + Job (job_queue.h)
    
    author: S. Hau
    date: October 22, 2017

    See below for a class description.

**********************************************************************/

#include "job.hpp"

namespace Core { namespace OS {

    //**********************************************************************
    // Contains a queue in which jobs can be put in. When a job is added
    // a thread will be notified to execute it. If no thread is idle
    // the job's execution will be deferred until a thread becomes idle.
    //**********************************************************************
    class JobQueue
    {
    public:
        JobQueue() = default;

        //----------------------------------------------------------------------
        // Add a new to the queue. The job will be executed by an arbitrary
        // thread immediately if one is idle. Otherwise the execution will
        // deferred until the job is in turn.
        //----------------------------------------------------------------------
        void addJob(const Job& job);

        //----------------------------------------------------------------------
        // Grab a job from the queue. If the queue is empty, the thread goes
        // to sleep until a job arrives. If more than one thread waits only
        // the CHOSEN ONE will get the job.
        //----------------------------------------------------------------------
        Job grabJob();

        //----------------------------------------------------------------------
        // Wait until the queue becomes empty. Returns immediately if already empty.
        //----------------------------------------------------------------------
        void waitUntilQueueIsEmpty();

    private:
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