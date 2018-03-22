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
        m_pConstantBuffer = new D3D11::ConstantBuffer( size, BufferUsage::LONG_LIVED );
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
    }

    //----------------------------------------------------------------------
    void Material::_ChangedShader()
    {
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
        auto d3d11Shader = dynamic_cast<Shader*>( m_shader );

        // Create buffer for vertex shader
        auto vertShader = d3d11Shader->getVertexShader();
        if ( vertShader->hasMaterialBuffer() )
        {
            auto& cb = vertShader->getMaterialBufferInformation();
            m_materialDataVS.resize( (U32) cb.sizeInBytes, cb.slot );
        }

        // Create buffer for pixel shader
        auto pixelShader = d3d11Shader->getPixelShader();
        if ( pixelShader->hasMaterialBuffer() )
        {
            auto& cb = pixelShader->getMaterialBufferInformation();
            m_materialDataPS.resize( (U32) cb.sizeInBytes, cb.slot );
        }
    }
 
    //----------------------------------------------------------------------
    bool Material::_UpdateConstantBuffer( StringID name, const void* pData, Size sizeInBytes )
    {
        auto d3d11Shader = reinterpret_cast<Shader*>(m_shader);

        auto vertexShader = d3d11Shader->getVertexShader();
        if ( vertexShader->hasMaterialBuffer() )
        {
            auto& vertCb = vertexShader->getMaterialBufferInformation();
            for (auto& mem : vertCb.members)
            {
                if (mem.name == name)
                {
                    m_materialDataVS.push( mem.offset, pData, sizeInBytes );
                    return true;
                }
            }
        }

        auto pixelShader = d3d11Shader->getPixelShader();
        if ( pixelShader->hasMaterialBuffer() )
        {
            auto& pixelCb = pixelShader->getMaterialBufferInformation();
            for (auto& mem : pixelCb.members)
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