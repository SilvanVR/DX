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
        addGlobalMaterial( "NONE", nullptr );
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void IRenderer::addGlobalMaterial( CString name, MaterialPtr material )
    { 
        m_globalMaterials[SID( name )] = material;
    }

    //----------------------------------------------------------------------
    void IRenderer::setGlobalMaterialActive( CString name ) 
    { 
        StringID id = SID( name );
        if ( m_globalMaterials.count( id ) == 0 )
        {
            WARN_RENDERING( "setGlobalShader(): Global Shader with name '" + String( name ) + "' does not exist." );
            return;
        }

        m_activeGlobalMaterial = m_globalMaterials[id];
    }

} // End namespaces