#include "i_renderer.h"
/**********************************************************************
    class: IRenderer (i_renderer.cpp)

    author: S. Hau
    date: November 28, 2017
**********************************************************************/

#include "Logging/logging.h"
#include "command_buffer.h"
#include "Events/event_dispatcher.h"
#include "VR/vr.h"
#include <mutex>

namespace Graphics {

    //----------------------------------------------------------------------
    IRenderer::IRenderer( OS::Window* window )
        : m_window( window )
    { 
        ASSERT( window != nullptr );

        // Register to resize window event
        Events::Event& evt = Events::EventDispatcher::GetEvent( EVENT_WINDOW_RESIZE );
        m_windowResizeListener = evt.addListener( BIND_THIS_FUNC_0_ARGS( &IRenderer::_OnWindowSizeChanged ) );
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void IRenderer::dispatch( const CommandBuffer& cmd, CmdDispatchMode dispatchMode )
    {
        _LockQueue();
        switch (dispatchMode)
        {
        case CmdDispatchMode::Immediate: m_immediatePendingCmdQueue.push_back( cmd ); break;
        case CmdDispatchMode::Deferred:  m_deferredPendingCmdQueue.push_back( cmd ); break;
        default: LOG_WARN( "IRenderer::dispatch(): Unknown dispatch mode." );
        }
        _UnlockQueue();
    }

    //----------------------------------------------------------------------
    // PROTECTED
    //----------------------------------------------------------------------
    static std::mutex s_renderQueueMutex;

    //----------------------------------------------------------------------
    void IRenderer::_LockQueue()
    {
        s_renderQueueMutex.lock();
    }

    //----------------------------------------------------------------------
    void IRenderer::_UnlockQueue()
    {
        s_renderQueueMutex.unlock();
    }

    //----------------------------------------------------------------------
    IRenderBuffer* IRenderer::_CreateTempRenderTarget( I32 maxFramesAlive )
    {
        TempRenderTarget tempRT{ createRenderBuffer(), maxFramesAlive };
        return m_tempRenderTargets.emplace_back( tempRT ).rt;
    }

    //----------------------------------------------------------------------
    void IRenderer::_CheckAndDestroyTemporaryRenderTargets()
    {
        for (auto it = m_tempRenderTargets.begin(); it != m_tempRenderTargets.end();)
        {
            --it->frameAliveCount;
            if (it->frameAliveCount)
            {
                ++it;
            }
            else
            {
                SAFE_DELETE( it->rt );
                it = m_tempRenderTargets.erase( it );
            }
        }
    }

    //----------------------------------------------------------------------
    void IRenderer::_Shutdown()
    {
        _DestroyAllTempRenderTargets();
        SAFE_DELETE( m_hmd );
    }

    //----------------------------------------------------------------------
    // PRIVATE
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void IRenderer::_DestroyAllTempRenderTargets()
    {
        for (auto& rt : m_tempRenderTargets)
            SAFE_DELETE( rt.rt );
        m_tempRenderTargets.clear();
    }

    //----------------------------------------------------------------------
    void IRenderer::_OnWindowSizeChanged()
    {
        OnWindowSizeChanged( m_window->getWidth(), m_window->getHeight() );
    }

} // End namespaces