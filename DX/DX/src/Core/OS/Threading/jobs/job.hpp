#pragma once



namespace Core { namespace OS {

    //**********************************************************************
    // Represents a job, which will be executed by any thread.
    // It consists of the function to execute by the thread and an 
    // additional function to call when the job is done.
    //**********************************************************************
    class Job
    {
    public:
        Job(std::nullptr_t null) 
            : Job( null, null ) 
        {}

        Job(const std::function<void()>& job = nullptr, const std::function<void()>& func = nullptr)
            : job( job ), calledWhenJobDone( func ) 
        {}

        //----------------------------------------------------------------------
        // Executes the job. If its done execute the "calledWhenJobDone" function.
        //----------------------------------------------------------------------
        void operator() () 
        {
            job();

            if (calledWhenJobDone != nullptr)
                calledWhenJobDone();
        }

        //----------------------------------------------------------------------
        bool operator == (std::nullptr_t null) const { return job == null; }
        bool operator != (std::nullptr_t null) const { return job != null; }

    private:
        std::function<void()> job;
        std::function<void()> calledWhenJobDone;
    };


} } // end namespaces