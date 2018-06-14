#include "D3D11Material.h"
/**********************************************************************
    class: Material (D3D11Material.cpp)

    author: S. Hau
    date: March 12, 2018

    @Considerations:
     - Mark material as dynamic or figure it out automatically, 
       so the buffer usage can be changed to FREQUENTLY
**********************************************************************/

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    void Material::bind()
    {
        // Bind constant buffers
        if (m_materialDataVS) m_materialDataVS->bind( ShaderType::Vertex );
        if (m_materialDataPS) m_materialDataPS->bind( ShaderType::Fragment );
        if (m_materialDataGS) m_materialDataGS->bind( ShaderType::Geometry );

        _BindTextures();
    }

    //----------------------------------------------------------------------
    void Material::_ChangedShader()
    {
        _DestroyConstantBuffers();
        _CreateConstantBuffers();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::_CreateConstantBuffers()
    {
        // Create buffer for vertex shader
        if ( auto cb = m_shader->getVSUniformMaterialBuffer() )
            m_materialDataVS = new MappedConstantBuffer( *cb, BufferUsage::LongLived );

        // Create buffer for pixel shader
        if ( m_shader->hasFragmentShader() )
            if ( auto cb = m_shader->getFSUniformMaterialBuffer() )
                m_materialDataPS = new MappedConstantBuffer( *cb, BufferUsage::LongLived );

        // Create buffer for geometry shader
        if ( m_shader->hasGeometryShader() )
            if ( auto cb = m_shader->getGSUniformMaterialBuffer() )
                m_materialDataGS = new MappedConstantBuffer( *cb, BufferUsage::LongLived );
    }
 
    //----------------------------------------------------------------------
    void Material::_UpdateConstantBuffer( StringID name, const void* pData )
    {
        // Because the super material class issues if the uniform does not exist,
        // i dont have to do it here. The update call on the corresponding mapped buffer
        // will do nothing if the name does not exist.
        if (m_materialDataVS) m_materialDataVS->update( name, pData );
        if (m_materialDataPS) m_materialDataPS->update( name, pData );
        if (m_materialDataGS) m_materialDataGS->update( name, pData );
    }

    //----------------------------------------------------------------------
    void Material::_DestroyConstantBuffers()
    {
        SAFE_DELETE( m_materialDataVS );
        SAFE_DELETE( m_materialDataPS );
        SAFE_DELETE( m_materialDataGS );
    }

} } // End namespaces