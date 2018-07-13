#include "D3D11Mesh.h"
/**********************************************************************
    class: Mesh (D3D11Mesh.cpp)

    author: S. Hau
    date: March 8, 2018
**********************************************************************/

#include "../Pipeline/Buffers/D3D11Buffers.h"
#include "structs.hpp"
#include "D3D11Shader.h"
#include "D3D11/D3D11Defines.hpp"

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
            case MeshBufferType::Tangent:   _UpdateTangentBuffer();                 break;
            default: ASSERT( false && "Unknown buffer type!" );
            }
            m_queuedBufferUpdates.pop();
        }

        _SetTopology( subMeshIndex );
        _BindVertexBuffer( vertLayout ); 
        _BindIndexBuffer( subMeshIndex );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Mesh::_Clear()
    {
        for (auto& vertexBuffer : m_pVertexBuffers)
            SAFE_DELETE( vertexBuffer.second );
        for (auto& indexBuffer : m_pIndexBuffers)
            SAFE_DELETE( indexBuffer );
        m_pIndexBuffers.clear();
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateVertexBuffer( const ArrayList<Math::Vec3>& vertices )
    {
        ASSERT( m_pVertexBuffers[SID_VERTEX_POSITION] == nullptr );
        m_pVertexBuffers[SID_VERTEX_POSITION] = new VertexBuffer( vertices.data(), (U32)(vertices.size() * sizeof( Math::Vec3 )), m_bufferUsage );
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateIndexBuffer( const SubMesh& subMesh, I32 index )
    {
        switch (subMesh.indexFormat )
        {
            case IndexFormat::U16:
            {
                ArrayList<U16> indicesU16( subMesh.indices.size() );
                for (U32 i = 0; i < indicesU16.size(); i++)
                    indicesU16[i] = subMesh.indices[i];

                auto pIndexBuffer = new D3D11::IndexBuffer( indicesU16.data(), (U32)(subMesh.indices.size() * sizeof( U16 )), m_bufferUsage );
                if (m_pIndexBuffers.size() <= index)
                    m_pIndexBuffers.resize( index + 1 );

                m_pIndexBuffers[index] = pIndexBuffer;
                break;
            }
            case IndexFormat::U32:
            {
                auto pIndexBuffer = new D3D11::IndexBuffer( subMesh.indices.data(), (U32)(subMesh.indices.size() * sizeof( U32 )), m_bufferUsage );
                if (m_pIndexBuffers.size() <= index)
                    m_pIndexBuffers.resize( index + 1 );

                m_pIndexBuffers[index] = pIndexBuffer;
                break;
            }
        }
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateColorBuffer( const ArrayList<Color>& colors )
    {
        ASSERT( m_pVertexBuffers[SID_VERTEX_COLOR] == nullptr );
        ArrayList<F32> colorsNormalized( colors.size() * 4 );
        for (U32 i = 0; i < colors.size(); i++)
        {
            auto normalized = colors[i].normalized();
            colorsNormalized[i * 4 + 0] = normalized[0];
            colorsNormalized[i * 4 + 1] = normalized[1];
            colorsNormalized[i * 4 + 2] = normalized[2];
            colorsNormalized[i * 4 + 3] = normalized[3];
        }
        m_pVertexBuffers[SID_VERTEX_COLOR] = new VertexBuffer( colorsNormalized.data(), static_cast<U32>( colorsNormalized.size() ) * sizeof( F32 ), m_bufferUsage );
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateUVBuffer( const ArrayList<Math::Vec2>& uvs )
    {
        ASSERT( m_pVertexBuffers[SID_VERTEX_UV] == nullptr );
        m_pVertexBuffers[SID_VERTEX_UV] = new VertexBuffer( uvs.data(), U32( uvs.size() * sizeof( Math::Vec2 ) ), m_bufferUsage );
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateNormalBuffer( const ArrayList<Math::Vec3>& normals )
    {
        ASSERT( m_pVertexBuffers[SID_VERTEX_NORMAL] == nullptr );
        m_pVertexBuffers[SID_VERTEX_NORMAL] = new VertexBuffer( normals.data(), U32( normals.size() * sizeof( Math::Vec3 ) ), m_bufferUsage );
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateTangentBuffer( const ArrayList<Math::Vec4>& tangents )
    {
        ASSERT( m_pVertexBuffers[SID_VERTEX_TANGENT] == nullptr );
        m_pVertexBuffers[SID_VERTEX_TANGENT] = new VertexBuffer( tangents.data(), U32( tangents.size() * sizeof( Math::Vec4 ) ), m_bufferUsage );
    }

    //----------------------------------------------------------------------
    void Mesh::_DestroyVertexBuffer()           { SAFE_DELETE( m_pVertexBuffers[SID_VERTEX_POSITION] ); }
    void Mesh::_DestroyIndexBuffer(I32 index)   { SAFE_DELETE( m_pIndexBuffers[index] ); }
    void Mesh::_DestroyColorBuffer()            { SAFE_DELETE( m_pVertexBuffers[SID_VERTEX_COLOR] ); }
    void Mesh::_DestroyUVBuffer()               { SAFE_DELETE( m_pVertexBuffers[SID_VERTEX_UV] ); }
    void Mesh::_DestroyNormalBuffer()           { SAFE_DELETE( m_pVertexBuffers[SID_VERTEX_NORMAL] ); }
    void Mesh::_DestroyTangentBuffer()          { SAFE_DELETE( m_pVertexBuffers[SID_VERTEX_TANGENT] ); }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void Mesh::_UpdateVertexBuffer()
    {
        m_pVertexBuffers[SID_VERTEX_POSITION]->update( m_vertices.data(), m_vertices.size() * sizeof( Math::Vec3 ) );
    }

    //----------------------------------------------------------------------
    void Mesh::_UpdateIndexBuffer( U32 index )
    {
        auto& subMesh = m_subMeshes[index];
        switch ( subMesh.indexFormat )
        {
        case IndexFormat::U16:
        {
            ArrayList<U16> indicesU16( subMesh.indexCount );
            for (U32 i = 0; i < indicesU16.size(); i++)
                indicesU16[i] = subMesh.indices[i];
            m_pIndexBuffers[index]->update( indicesU16.data(), (U32)( subMesh.indexCount * sizeof( U16 ) ) );
            break;
        }
        case IndexFormat::U32:
            m_pIndexBuffers[index]->update( m_subMeshes[index].indices.data(), (U32)(m_subMeshes[index].indexCount * sizeof( U32 )) );
            break;
        }
    }

    //----------------------------------------------------------------------
    void Mesh::_UpdateUVBuffer()
    {
        m_pVertexBuffers[SID_VERTEX_UV]->update( m_uvs0.data(), m_uvs0.size() * sizeof( Math::Vec2 ) );
    }

    //----------------------------------------------------------------------
    void Mesh::_UpdateColorBuffer()
    {
        ArrayList<F32> colorsNormalized( m_colors.size() * 4 );
        for (U32 i = 0; i < m_colors.size(); i++)
        {
            auto normalized = m_colors[i].normalized();
            colorsNormalized[i * 4 + 0] = normalized[0];
            colorsNormalized[i * 4 + 1] = normalized[1];
            colorsNormalized[i * 4 + 2] = normalized[2];
            colorsNormalized[i * 4 + 3] = normalized[3];
        }
        m_pVertexBuffers[SID_VERTEX_COLOR]->update( colorsNormalized.data(), static_cast<U32>( colorsNormalized.size() ) * sizeof( F32 ) );
    }

    //----------------------------------------------------------------------
    void Mesh::_UpdateNormalBuffer()
    {
        m_pVertexBuffers[SID_VERTEX_NORMAL]->update( m_normals.data(), m_normals.size() * sizeof( Math::Vec3 ) );
    }

    //----------------------------------------------------------------------
    void Mesh::_UpdateTangentBuffer()
    {
        m_pVertexBuffers[SID_VERTEX_TANGENT]->update( m_tangents.data(), m_tangents.size() * sizeof( Math::Vec4 ) );
    }

    //----------------------------------------------------------------------
    void Mesh::_RecreateBuffers()
    {
        // Recreate vertex buffer
        if (m_pVertexBuffers.find( SID_VERTEX_POSITION ) != m_pVertexBuffers.end())
        {
            _DestroyVertexBuffer();
            _CreateVertexBuffer( m_vertices );
        }

        // Recreate color buffer
        if (m_pVertexBuffers.find( SID_VERTEX_COLOR ) != m_pVertexBuffers.end())
        {
            _DestroyColorBuffer();
            _CreateColorBuffer( m_colors );
        }

        // Recreate uv buffer
        if (m_pVertexBuffers.find( SID_VERTEX_UV ) != m_pVertexBuffers.end())
        {
            _DestroyUVBuffer();
            _CreateUVBuffer( m_uvs0 );
        }

        // Recreate normal buffer
        if (m_pVertexBuffers.find( SID_VERTEX_NORMAL ) != m_pVertexBuffers.end())
        {
            _DestroyNormalBuffer();
            _CreateNormalBuffer( m_normals );
        }

        // Recreate tangent buffer
        if (m_pVertexBuffers.find( SID_VERTEX_TANGENT ) != m_pVertexBuffers.end())
        {
            _DestroyTangentBuffer();
            _CreateTangentBuffer( m_tangents );
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
    void Mesh::_BindVertexBuffer( const VertexLayout& vertLayout )
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
            auto it = m_pVertexBuffers.find( binding.name );
            if (it != m_pVertexBuffers.end())
            {
                pBuffers[bufferIndex] = it->second->getBuffer();
                strides[bufferIndex] = binding.sizeInBytes;
                offsets[bufferIndex] = 0;
                bufferIndex++;
            }
            else
            {
                LOG_ERROR_RENDERING( "Missing vertex buffer stream '" + binding.name.toString() + "' in a mesh. Fix this!" );
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