#include "D3D11Material.h"
/**********************************************************************
    class: Material (D3D11Material.cpp)

    author: S. Hau
    date: March 12, 2018

    @Considerations:
     - Mark material as dynamic or figure it out automatically, 
       so the buffer usage can be changed to FREQUENTLY
**********************************************************************/

#include "D3D11Shader.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    void Material::bind()
    {
        // Bind constant buffers
        if ( m_materialDataVS )
            m_materialDataVS->bind( ShaderType::Vertex );
        if ( m_materialDataPS )
            m_materialDataPS->bind( ShaderType::Fragment );

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

    //----------------------------------------------------------------------
    //void Material::_SetTexture( StringID name, const TexturePtr& texture )
    //{
    //    auto shaderDecl = m_shader->getShaderResource( name );

    //    if (shaderDecl)
    //    {
    //        // Update texture slot if possible
    //        for (auto& entry : m_textureCache)
    //            if (entry.bindSlot == shaderDecl->getBindingSlot())
    //            {
    //                entry.texture = dynamic_cast<D3D11::IBindableTexture*>( texture.get() );
    //                return;
    //            }

    //        // Texture slot was set for the first time
    //        TextureCache texCache;
    //        texCache.bindSlot   = shaderDecl->getBindingSlot();
    //        texCache.shaderType = shaderDecl->getShaderType();
    //        texCache.texture    = dynamic_cast<D3D11::IBindableTexture*>( texture.get() );

    //        m_textureCache.emplace_back(texCache);
    //    }
    //}

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
        if ( auto cb = m_shader->getFSUniformMaterialBuffer() )
            m_materialDataPS = new MappedConstantBuffer( *cb, BufferUsage::LongLived );
    }
 
    //----------------------------------------------------------------------
    void Material::_UpdateConstantBuffer( StringID name, const void* pData )
    {
        // Because the super material class issues if the uniform does not exist,
        // i dont have to do it here. The update call on the corresponding mapped buffer
        // will do nothing if the name does not exist.
        if (m_materialDataVS) m_materialDataVS->update( name, pData );
        if (m_materialDataPS) m_materialDataPS->update( name, pData );
    }

    //----------------------------------------------------------------------
    void Material::_DestroyConstantBuffers()
    {
        SAFE_DELETE( m_materialDataVS );
        SAFE_DELETE( m_materialDataPS );
    }

} } // End namespaces