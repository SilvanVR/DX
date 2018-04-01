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
    // MATERIAL DATA
    //**********************************************************************

    //----------------------------------------------------------------------
    void MaterialData::push( U32 offset, const void* pData, Size sizeInBytes )
    {
        ASSERT( (offset + sizeInBytes) <= m_materialData.size() );
        memcpy( &m_materialData[offset], pData, sizeInBytes );
        m_gpuUpToDate = false;
    }

    //----------------------------------------------------------------------
    void MaterialData::resize( U32 size, U32 bindSlot )
    {
        SAFE_DELETE( m_pConstantBuffer );
        m_materialData.resize( size );
        m_pConstantBuffer = new D3D11::ConstantBuffer( size, BufferUsage::LongLived );
        m_bindSlot = bindSlot;
    }

    //----------------------------------------------------------------------
    void MaterialData::bind( ShaderType shaderType )
    {
        if ( not m_gpuUpToDate )
        {
            m_pConstantBuffer->update( m_materialData.data(), m_materialData.size() );
            m_gpuUpToDate = true;
        }

        switch (shaderType)
        {
        case ShaderType::Vertex:    m_pConstantBuffer->bindToVertexShader( m_bindSlot ); break;
        case ShaderType::Fragment:  m_pConstantBuffer->bindToPixelShader( m_bindSlot ); break;
        default: ASSERT( false );
        }
    }

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
            texInfo.texture->bind( texInfo.bindSlot );
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
    bool Material::_SetTexture( StringID name, Graphics::Texture* texture )
    {
        auto d3d11Shader = dynamic_cast<Shader*>( m_shader );
        D3D11::IBindableTexture* d3d11Texture = dynamic_cast<D3D11::IBindableTexture*>( texture );

        I32 bindingSlot = d3d11Shader->getTextureBindingSlot( name );
        if ( bindingSlot >= 0 )
        {
            TextureCache texCache;
            texCache.bindSlot = bindingSlot;
            texCache.texture  = d3d11Texture;

            m_textureCache.emplace_back( texCache );
            return true;
        }

        return false;
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Material::_CreateConstantBuffers()
    {
        auto d3d11Shader = dynamic_cast<Shader*>( m_shader );

        // Create buffer for vertex shader
        if ( auto cb = d3d11Shader->getVertexShader()->getMaterialBufferInfo() )
            m_materialDataVS.resize( static_cast<U32>( cb->sizeInBytes ), cb->slot );

        // Create buffer for pixel shader
        if ( auto  cb = d3d11Shader->getPixelShader()->getMaterialBufferInfo() )
            m_materialDataPS.resize( static_cast<U32>( cb->sizeInBytes ), cb->slot );
    }
 
    //----------------------------------------------------------------------
    bool Material::_UpdateConstantBuffer( StringID name, const void* pData, Size sizeInBytes )
    {
        auto d3d11Shader = reinterpret_cast<Shader*>( m_shader );
        
        // Check if uniform is in vertex-shader
        if ( auto cb = d3d11Shader->getVertexShader()->getMaterialBufferInfo() )
        {
            for (auto& mem : cb->members)
            {
                if (mem.name == name)
                {
                    m_materialDataVS.push( mem.offset, pData, sizeInBytes );
                    return true;
                }
            }
        }

        // Check if uniform is in pixel-shader
        if ( auto cb = d3d11Shader->getPixelShader()->getMaterialBufferInfo() )
        {
            for (auto& mem : cb->members)
            {
                if (mem.name == name)
                {
                    m_materialDataPS.push( mem.offset, pData, sizeInBytes );
                    return true;
                }
            }
        }

        return false;
    }

} } // End namespaces