#include "D3D11Material.h"
/**********************************************************************
    class: Material (D3D11Material.cpp)

    author: S. Hau
    date: March 12, 2018

    @Considerations:
     - Mark material as dynamic or figure it out automatically, 
       so the buffer usage can be changed to FREQUENTLY
     - Cache (NAME, [ShaderType, offset]). Needs testing if its worth
**********************************************************************/

#include "D3D11Shader.h"
#include "../Pipeline/Shaders/D3D11VertexShader.h"
#include "../Pipeline/Shaders/D3D11PixelShader.h"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    // MATERIAL
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::bind()
    {
        // Bind constant buffers
        if ( m_materialDataVS.hasBuffer() )
            m_materialDataVS.bind( ShaderType::Vertex );
        if ( m_materialDataPS.hasBuffer() )
            m_materialDataPS.bind( ShaderType::Fragment );

        // Bind textures
        for (auto& texInfo : m_textureCache)
            texInfo.texture->bind( texInfo.shaderType, texInfo.bindSlot );
    }

    //----------------------------------------------------------------------
    void Material::_ChangedShader()
    {
        m_textureCache.clear();
        _CreateConstantBuffers();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::_SetTexture( StringID name, const TexturePtr& texture )
    {
        auto d3d11Shader = dynamic_cast<Shader*>( m_shader.get() );

        // VERTEX SHADER
        if ( auto binding = d3d11Shader->getVertexShader()->getTextureBindingInfo( name ) )
        {
            // Update texture slot if possible
            for (auto& entry : m_textureCache)
                if (entry.bindSlot == binding->slot)
                {
                    entry.texture = dynamic_cast<D3D11::IBindableTexture*>( texture.get() );
                    return;
                }

            // Texture slot was set for the first time
            TextureCache texCache;
            texCache.bindSlot = binding->slot;
            texCache.shaderType = ShaderType::Vertex;
            texCache.texture = dynamic_cast<D3D11::IBindableTexture*>( texture.get() );

            m_textureCache.emplace_back( texCache );
        }

        // PIXEL SHADER
        if ( auto binding = d3d11Shader->getPixelShader()->getTextureBindingInfo( name ) )
        {
            // Update texture slot if possible
            for (auto& entry : m_textureCache)
                if (entry.bindSlot == binding->slot)
                {
                    entry.texture = dynamic_cast<D3D11::IBindableTexture*>( texture.get() );
                    return;
                }
      
            // Texture slot was set for the first time
            TextureCache texCache;
            texCache.bindSlot = binding->slot;
            texCache.shaderType = ShaderType::Fragment;
            texCache.texture = dynamic_cast<D3D11::IBindableTexture*>( texture.get() );

            m_textureCache.emplace_back( texCache );
        }
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::_CreateConstantBuffers()
    {
        auto d3d11Shader = dynamic_cast<Shader*>( m_shader.get() );

        // Create buffer for vertex shader
        if ( auto cb = d3d11Shader->getVertexShader()->getMaterialBufferInfo() )
            m_materialDataVS.resize( *cb, BufferUsage::LongLived );

        // Create buffer for pixel shader
        if ( auto cb = d3d11Shader->getPixelShader()->getMaterialBufferInfo() )
            m_materialDataPS.resize( *cb, BufferUsage::LongLived );
    }
 
    //----------------------------------------------------------------------
    void Material::_UpdateConstantBuffer( StringID name, const void* pData )
    {
        // Because the super material class issues if the uniform does not exist,
        // i dont have to do it here. The update call on the corresponding mapped buffer
        // will do nothing if the name does not exist.
        m_materialDataVS.update( name, pData );
        m_materialDataPS.update( name, pData );
    }

} } // End namespaces