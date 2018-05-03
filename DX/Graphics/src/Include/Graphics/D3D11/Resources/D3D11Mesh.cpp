#include "D3D11Mesh.h"
/**********************************************************************
    class: Mesh (D3D11Mesh.cpp)

    author: S. Hau
    date: March 8, 2018
**********************************************************************/

#include "../Pipeline/Buffers/D3D11Buffers.h"
#include "../../structs.hpp"
#include "D3D11Shader.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    Mesh::~Mesh()
    {
        clear();
    }

    //----------------------------------------------------------------------
    void Mesh::bind( const VertexLayout& vertLayout, U32 subMeshIndex )
    {
        while ( not m_queuedBufferUpdates.empty() )
        {
            auto buffUpdate = m_queuedBufferUpdates.front();
            switch ( buffUpdate.type )
            {
            case MeshBufferType::Vertex:    _UpdateVertexBuffer();                  break;
            case MeshBufferType::TexCoord:  _UpdateUVBuffer();                      break;
            case MeshBufferType::Color:     _UpdateColorBuffer();                   break;
            case MeshBufferType::Index:     _UpdateIndexBuffer(buffUpdate.index);   break;
            case MeshBufferType::Normal:    _UpdateNormalBuffer();                  break;
            }
            m_queuedBufferUpdates.pop();
        }

        _SetTopology( subMeshIndex );
        _BindVertexBuffer( vertLayout, subMeshIndex ); 
        _BindIndexBuffer( subMeshIndex );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Mesh::_Clear()
    {
        SAFE_DELETE( m_pVertexBuffer );
        SAFE_DELETE( m_pColorBuffer );
        SAFE_DELETE( m_pUVBuffer );
        SAFE_DELETE( m_pNormalBuffer );
        for (auto& indexBuffer : m_pIndexBuffers)
            SAFE_DELETE( indexBuffer );
        m_pIndexBuffers.clear();
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateVertexBuffer( const ArrayList<Math::Vec3>& vertices )
    {
        ASSERT( m_pVertexBuffer == nullptr );
        m_pVertexBuffer = new VertexBuffer( m_vertices.data(), getVertexCount() * sizeof( Math::Vec3 ), m_bufferUsage );
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateIndexBuffer( const SubMesh& subMesh, I32 index )
    {
        switch (subMesh.indexFormat )
        {
            case IndexFormat::U16:
            {
                ArrayList<U16> indicesU16;
                for ( auto& index : subMesh.indices )
                    indicesU16.push_back( index );
                auto pIndexBuffer = new D3D11::IndexBuffer( indicesU16.data(), getIndexCount( index ) * sizeof( U16 ), m_bufferUsage );
                m_pIndexBuffers.push_back( pIndexBuffer );
                break;
            }
            case IndexFormat::U32:
            {
                auto pIndexBuffer = new D3D11::IndexBuffer( subMesh.indices.data(), getIndexCount( index ) * sizeof( U32 ), m_bufferUsage );
                m_pIndexBuffers.push_back( pIndexBuffer );
                break;
            }
        }
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateColorBuffer( const ArrayList<Color>& colors )
    {
        ASSERT( m_pColorBuffer == nullptr );
        ArrayList<F32> colorsNormalized( m_vertexColors.size() * 4 );
        for (U32 i = 0; i < m_vertexColors.size(); i++)
        {
            auto normalized = m_vertexColors[i].normalized();
            colorsNormalized[i * 4 + 0] = normalized[0];
            colorsNormalized[i * 4 + 1] = normalized[1];
            colorsNormalized[i * 4 + 2] = normalized[2];
            colorsNormalized[i * 4 + 3] = normalized[3];
        }
        m_pColorBuffer = new VertexBuffer( colorsNormalized.data(), static_cast<U32>( colorsNormalized.size() ) * sizeof( F32 ), m_bufferUsage );
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateUVBuffer( const ArrayList<Math::Vec2>& uvs )
    {
        ASSERT( m_pUVBuffer == nullptr );
        m_pUVBuffer = new VertexBuffer( m_uvs0.data(), getVertexCount() * sizeof( Math::Vec2 ), m_bufferUsage );
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateNormalBuffer( const ArrayList<Math::Vec3>& normals )
    {
        ASSERT( m_pNormalBuffer == nullptr );
        m_pNormalBuffer = new VertexBuffer( normals.data(), getVertexCount() * sizeof( Math::Vec3 ), m_bufferUsage );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Mesh::_UpdateVertexBuffer()
    {
        m_pVertexBuffer->update( m_vertices.data(), m_vertices.size() * sizeof( Math::Vec3 ) );
    }

    //----------------------------------------------------------------------
    void Mesh::_UpdateIndexBuffer( U32 index )
    {
        Size indexSize = 0;
        switch ( getIndexFormat( index ) )
        {
        case IndexFormat::U16: indexSize = sizeof( U16 ); break;
        case IndexFormat::U32: indexSize = sizeof( U32 ); break;
        }
        m_pIndexBuffers[index]->update( m_subMeshes[index].indices.data(), m_subMeshes[index].indices.size() * indexSize );
    }

    //----------------------------------------------------------------------
    void Mesh::_UpdateUVBuffer()
    {
        m_pUVBuffer->update( m_uvs0.data(), m_uvs0.size() * sizeof( Math::Vec2 ) );
    }

    //----------------------------------------------------------------------
    void Mesh::_UpdateColorBuffer()
    {
        ArrayList<F32> colorsNormalized( m_vertexColors.size() * 4 );
        for (U32 i = 0; i < m_vertexColors.size(); i++)
        {
            auto normalized = m_vertexColors[i].normalized();
            colorsNormalized[i * 4 + 0] = normalized[0];
            colorsNormalized[i * 4 + 1] = normalized[1];
            colorsNormalized[i * 4 + 2] = normalized[2];
            colorsNormalized[i * 4 + 3] = normalized[3];
        }
        m_pColorBuffer->update( colorsNormalized.data(), static_cast<U32>( colorsNormalized.size() ) * sizeof( F32 ) );
    }

    //----------------------------------------------------------------------
    void Mesh::_UpdateNormalBuffer()
    {
        m_pNormalBuffer->update( m_normals.data(), m_normals.size() * sizeof( Math::Vec3 ) );
    }

    //----------------------------------------------------------------------
    void Mesh::_RecreateBuffers()
    {
        // Recreate vertex buffer
        if (m_pVertexBuffer)
        {
            SAFE_DELETE( m_pVertexBuffer );
            _CreateVertexBuffer( m_vertices );
        }

        // Recreate color buffer
        if (m_pColorBuffer)
        {
            SAFE_DELETE( m_pColorBuffer );
            _CreateColorBuffer( m_vertexColors );
        }

        // Recreate uv buffer
        if (m_pColorBuffer)
        {
            SAFE_DELETE( m_pUVBuffer );
            _CreateUVBuffer( m_uvs0 );
        }

        // Recreate normal buffer
        if (m_pNormalBuffer)
        {
            SAFE_DELETE( m_pNormalBuffer );
            _CreateNormalBuffer( m_normals );
        }

        // Recreate index buffers
        for (auto& indexBuffer : m_pIndexBuffers)
            SAFE_DELETE( indexBuffer );
        m_pIndexBuffers.clear();

        for (U32 i = 0; i < m_subMeshes.size(); i++)
            _CreateIndexBuffer( m_subMeshes[i], i );
    }

    //----------------------------------------------------------------------
    void Mesh::_SetTopology( U32 subMeshIndex )
    {
        D3D_PRIMITIVE_TOPOLOGY dxTopology;
        switch (m_subMeshes[subMeshIndex].topology)
        {
        case MeshTopology::Lines:           dxTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
        case MeshTopology::LineStrip:       dxTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
        case MeshTopology::Points:          dxTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
        case MeshTopology::Triangles:       dxTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
        case MeshTopology::TriangleStrip:   dxTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
        default: LOG_WARN_RENDERING( "Mesh::_SetTopology(): Unknown topology type." );
        }
        g_pImmediateContext->IASetPrimitiveTopology( dxTopology );
    }

    //----------------------------------------------------------------------
    void Mesh::_BindVertexBuffer( const VertexLayout& vertLayout, U32 subMesh )
    {
        #define MAX_BUFFERS 8
        auto& vertexDescription = vertLayout.getLayoutDescription();

        ASSERT( vertexDescription.size() <= MAX_BUFFERS );

        ID3D11Buffer* pBuffers[MAX_BUFFERS];
        U32 strides[MAX_BUFFERS];
        U32 offsets[MAX_BUFFERS];
        U32 bufferIndex = 0;

        for ( auto& binding : vertexDescription )
        {
            switch (binding.type)
            {
            case InputLayoutType::POSITION:
            {
                pBuffers[bufferIndex] = m_pVertexBuffer->getBuffer();
                strides[bufferIndex] = static_cast<U32>( sizeof( Math::Vec3 ) );
                offsets[bufferIndex] = 0;
                bufferIndex++;
                break;
            }
            case InputLayoutType::COLOR:
            {
                ASSERT( m_pColorBuffer && "Shader requires a color-buffer, but mesh has none!" );
                pBuffers[bufferIndex] = m_pColorBuffer->getBuffer();
                strides[bufferIndex] = static_cast<U32>( sizeof( F32 ) * 4 );
                offsets[bufferIndex] = 0;
                bufferIndex++;
                break;
            }
            case InputLayoutType::TEXCOORD0:
            {
                ASSERT( m_pUVBuffer && "Shader requires a uv-buffer, but mesh has none!" );
                pBuffers[bufferIndex] = m_pUVBuffer->getBuffer();
                strides[bufferIndex] = static_cast<U32>( sizeof( Math::Vec2 ) );
                offsets[bufferIndex] = 0;
                bufferIndex++;
                break;
            }
            case InputLayoutType::NORMAL:
            {
                ASSERT( m_pNormalBuffer && "Shader requires a normal-buffer, but mesh has none!");
                pBuffers[bufferIndex] = m_pNormalBuffer->getBuffer();
                strides[bufferIndex] = static_cast<U32>( sizeof( Math::Vec3 ) );
                offsets[bufferIndex] = 0;
                bufferIndex++;
                break;
            }
            default:
                ASSERT( false && "implement!" );
            }
        }

        // Bind vertex buffers
        g_pImmediateContext->IASetVertexBuffers( 0, bufferIndex, pBuffers, strides, offsets );
    }

    //----------------------------------------------------------------------
    void Mesh::_BindIndexBuffer( U32 subMeshIndex )
    {
        switch (m_subMeshes[subMeshIndex].indexFormat)
        {
        case IndexFormat::U16: m_pIndexBuffers[subMeshIndex]->bind( DXGI_FORMAT_R16_UINT, 0 ); break;
        case IndexFormat::U32: m_pIndexBuffers[subMeshIndex]->bind( DXGI_FORMAT_R32_UINT, 0 ); break;
        }
    }


} } // End namespaces