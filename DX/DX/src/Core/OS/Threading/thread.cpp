#include "thread.h"
/**********************************************************************
    class: Thread (thread.cpp)
    
    author: S. Hau
    date: October 21, 2017
**********************************************************************/

#include "Core/locator.h"
#include <sstream>

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    Thread::Thread( JobQueue& jobQueue )
        : m_jobQueue( jobQueue )
    {
    
        int i = 52;
    }

    //----------------------------------------------------------------------
    Thread::~Thread()
    {
        _Join();
    }

    //----------------------------------------------------------------------
    void Thread::waitIdle()
    {
        // Wait until thread has finished his current job
        m_running = false;
    }

    //----------------------------------------------------------------------
    void Thread::_ThreadLoop()
    {
        //std::stringstream ss;
        //ss << m_thread.get_id();
        //String id = ss.str();

        while (m_running)
        {
            // Try to grab a job. The thread will be put to sleep if no job is available
            m_currentJob = m_jobQueue.grabJob();

            if (m_currentJob != nullptr)
            {
                // Execute the job
                //LOG( "Job executed by thread #" + id );
                m_currentJob();

                m_currentJob = nullptr;
            }
        } 
        //LOG( "Terminate thread #" + id );
    }

    //----------------------------------------------------------------------
    void Thread::_Join()
    {
        if ( m_thread.joinable() )
        {
            m_running = false;
            m_jobQueue.notifyAll();
            m_thread.join();
        }
    }


} } // end namespaces