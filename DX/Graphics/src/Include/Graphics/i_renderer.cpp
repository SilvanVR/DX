#include "i_renderer.h"
/**********************************************************************
    class: IRenderer (i_renderer.cpp)

    author: S. Hau
    date: November 28, 2017
**********************************************************************/

#include "Logging/logging.h"
#include "command_buffer.h"
#include "Events/event_dispatcher.h"
#include "Events/event_names.hpp"
#include "VR/OculusRift/oculus_rift.h"
#include <mutex>

namespace Graphics {

    //----------------------------------------------------------------------
    IRenderer::IRenderer( OS::Window* window )
        : m_window( window )
    { 
        // Register to resize window event
        Events::Event& evt = Events::EventDispatcher::GetEvent( EVENT_WINDOW_RESIZE );
        evt.addListener( BIND_THIS_FUNC_0_ARGS( &IRenderer::_OnWindowSizeChanged ) );

        ASSERT( window != nullptr );
        addGlobalMaterial( "NONE", nullptr );
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void IRenderer::dispatch( const CommandBuffer& cmd )
    {
        _LockQueue();
        m_pendingCmdQueue.emplace_back( cmd );
        _UnlockQueue();
    }

    //----------------------------------------------------------------------
    void IRenderer::addGlobalMaterial( CString name, const std::shared_ptr<IMaterial>& material )
    { 
        m_globalMaterials[SID( name )] = material;
    }

    //----------------------------------------------------------------------
    void IRenderer::setGlobalMaterialActive( CString name ) 
    { 
        StringID id = SID( name );
        if ( m_globalMaterials.count( id ) == 0 )
        {
            LOG_WARN_RENDERING( "setGlobalShader(): Global Shader with name '" + String( name ) + "' does not exist." );
            return;
        }

        m_activeGlobalMaterial = m_globalMaterials[id];
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
    bool IRenderer::_InitializeHMD()
    {
        VR::Device hmd = VR::GetFirstSupportedHMDAndInitialize();
        switch (hmd)
        {
        case VR::Device::OculusRift: m_hmd = new VR::OculusRift( getAPI() ); return true;
        }
        return false;
    }

    //----------------------------------------------------------------------
    // PRIVATE
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void IRenderer::_OnWindowSizeChanged()
    {
        OnWindowSizeChanged( m_window->getWidth(), m_window->getHeight() );
    }

} // End namespaces