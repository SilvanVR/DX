#pragma once
/**********************************************************************
    class: Job (job.hpp)

    author: S. Hau
    date: October 22, 2017

    See below for a class description.

**********************************************************************/

namespace OS {

    //**********************************************************************
    // Represents a job, which will be executed by a thread.
    //**********************************************************************
    class Job
    {
    public:
        Job(const std::function<void()>& job = nullptr)
            : m_job( job )
        {}

        //----------------------------------------------------------------------
        // Wait until a thread has completed its execution.
        //----------------------------------------------------------------------
        void wait()
        {
            std::unique_lock<std::mutex> lock( m_mutex );
            m_cv.wait( lock, [this]() -> bool { return m_done; } );
        }

        //----------------------------------------------------------------------
        // Executes the job. Notify possible waiting threads when it's done.
        //----------------------------------------------------------------------
        void operator() () 
        {
            std::unique_lock<std::mutex> lock( m_mutex );
            m_job();

            m_done = true;
            m_cv.notify_all();
        }

    private:
        std::function<void()>       m_job;
        std::mutex                  m_mutex;
        std::condition_variable     m_cv;
        bool                        m_done = false;
    };

    //----------------------------------------------------------------------
    using JobPtr = std::shared_ptr<Job>;


} // end namespaces