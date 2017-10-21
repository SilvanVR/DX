#include "thread.h"
/**********************************************************************
    class: Thread (thread.cpp)
    
    author: S. Hau
    date: October 21, 2017
**********************************************************************/

#include "Core/locator.h"

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    Thread::Thread()
        : m_running( true )
    {
        m_thread = std::thread( &Thread::_ThreadLoop, this );
    }

    //----------------------------------------------------------------------
    Thread::~Thread()
    {
        if ( m_thread.joinable() )
        {
            waitIdle();
            m_running = false;
            m_thread.join();
        }
    }

    //----------------------------------------------------------------------
    void Thread::waitIdle()
    {
        m_running = false;
    }

    //----------------------------------------------------------------------
    void Thread::_ThreadLoop()
    {
        while (m_running)
        {
            LOG("HELLO");


        }
    }


} } // end namespaces