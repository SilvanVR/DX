#include "i_renderer.h"
/**********************************************************************
    class: IRenderer (i_renderer.cpp)

    author: S. Hau
    date: November 28, 2017
**********************************************************************/

#include "Logging/logging.h"
#include "command_buffer.h"
#include <mutex>

namespace Graphics {

    OS::Window* IRenderer::s_window = nullptr;

    static std::mutex s_renderQueueMutex;

    //----------------------------------------------------------------------
    IRenderer::IRenderer( OS::Window* window )
    { 
        s_window = window;
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
    void IRenderer::addGlobalMaterial( CString name, IMaterial* material )
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

} // End namespaces