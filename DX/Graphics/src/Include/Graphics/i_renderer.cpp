#include "i_renderer.h"
/**********************************************************************
    class: IRenderer (i_renderer.cpp)

    author: S. Hau
    date: November 28, 2017

**********************************************************************/

#include "Logging/logging.h"

namespace Graphics {

    //----------------------------------------------------------------------
    IRenderer::IRenderer(OS::Window* window) 
        : m_window( window )
    { 
        ASSERT( window != nullptr );
        addGlobalMaterial( "NONE", nullptr );
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

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

} // End namespaces