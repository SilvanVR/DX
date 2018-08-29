#include "VkMaterial.h"
/**********************************************************************
    class: Material

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    void Material::bind()
    {
        if (m_materialDataVS) m_materialDataVS->bind();
        if (m_materialDataFS) m_materialDataFS->bind();
        if (m_materialDataGS) m_materialDataGS->bind();

        _BindTextures();
    }

    //----------------------------------------------------------------------
    void Material::_ChangedShader()
    {
        _DestroyUniformBuffers();
        _CreateUniformBuffers();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::_CreateUniformBuffers()
    {
        // Create buffer for vertex shader
        if ( auto cb = m_shader->getVSUniformMaterialBuffer() )
            m_materialDataVS = new MappedUniformBuffer( *cb, BufferUsage::LongLived );

        // Create buffer for pixel shader
        if ( m_shader->hasFragmentShader() )
            if ( auto cb = m_shader->getFSUniformMaterialBuffer() )
                m_materialDataFS = new MappedUniformBuffer( *cb, BufferUsage::LongLived );

        // Create buffer for geometry shader
        if ( m_shader->hasGeometryShader() )
            if ( auto cb = m_shader->getGSUniformMaterialBuffer() )
                m_materialDataGS = new MappedUniformBuffer( *cb, BufferUsage::LongLived );
    }
 
    //----------------------------------------------------------------------
    void Material::_UpdateUniformBuffer( StringID name, const void* pData )
    {
        // Because the super material class issues if the uniform does not exist,
        // i dont have to do it here. The update call on the corresponding mapped buffer
        // will do nothing if the name does not exist.
        if (m_materialDataVS) m_materialDataVS->update( name, pData );
        if (m_materialDataFS) m_materialDataFS->update( name, pData );
        if (m_materialDataGS) m_materialDataGS->update( name, pData );
    }

    //----------------------------------------------------------------------
    void Material::_DestroyUniformBuffers()
    {
        SAFE_DELETE( m_materialDataVS );
        SAFE_DELETE( m_materialDataFS );
        SAFE_DELETE( m_materialDataGS );
    }

} } // End namespaces