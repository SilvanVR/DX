#include "i_renderer.h"
/**********************************************************************
    class: IRenderer (i_renderer.cpp)

    author: S. Hau
    date: November 28, 2017

**********************************************************************/



namespace Graphics {


    //----------------------------------------------------------------------
    IRenderer::IRenderer(OS::Window* window) 
        : m_window( window )
    { 
        ASSERT( window != nullptr );
        m_window->setCallbackSizeChanged( BIND_THIS_FUNC_2_ARGS( &IRenderer::OnWindowSizeChanged ) );
        addGlobalShader( "NONE", nullptr );
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void IRenderer::addGlobalShader( CString name, IShader* shader ) 
    { 
        m_globalShaders[SID( name )] = shader;
    }

    //----------------------------------------------------------------------
    void IRenderer::setGlobalShaderActive( CString name ) 
    { 
        StringID id = SID( name );
        if ( m_globalShaders.count(id) == 0 )
        {
            WARN_RENDERING( "setGlobalShader(): Global Shader with name '" + String( name ) + "' does not exist." );
            return;
        }

        m_activeGlobalShader = m_globalShaders[id]; 
    }



} // End namespaces