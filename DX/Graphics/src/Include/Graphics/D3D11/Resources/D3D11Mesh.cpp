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
    void Mesh::clear()
    {
        SAFE_DELETE( m_pVertexBuffer );
        SAFE_DELETE( m_pColorBuffer );
        SAFE_DELETE( m_pUVBuffer );
        SAFE_DELETE( m_pNormalBuffer );
        for (auto& indexBuffer : m_pIndexBuffers)
            SAFE_DELETE( indexBuffer );

        m_vertices.clear();
        m_vertexColors.clear();
        m_uvs0.clear();
        m_normals.clear();
        m_pIndexBuffers.clear();
        m_subMeshes.clear();
    }

    //----------------------------------------------------------------------
    void Mesh::setVertices( const ArrayList<Math::Vec3>& vertices )
    {
#if _DEBUG
        if (m_pVertexBuffer != nullptr)
            ASSERT( (m_vertices.size() == vertices.size() &&
                    "IMesh::setVertices(): The amount of vertices given must be the number of vertices already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated.") );
#endif
        m_vertices = vertices;
        if (m_pVertexBuffer == nullptr)
        {
            m_pVertexBuffer = new VertexBuffer( m_vertices.data(), getVertexCount() * sizeof( Math::Vec3 ), m_bufferUsage );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );
            m_queuedBufferUpdates.push( { MeshBufferType::Vertex } );
        }
    }

    //----------------------------------------------------------------------
    void Mesh::setIndices( const ArrayList<U32>& indices, U32 subMeshIndex, MeshTopology topology, U32 baseVertex )
    {
#if _DEBUG
        if ( !m_subMeshes.empty() )
        {
            if ( subMeshIndex < m_pIndexBuffers.size() )
            {
                ASSERT( m_subMeshes[subMeshIndex].indices.size() == indices.size() &&
                        "IMesh::setTriangles(): The amount of indices given must be the number of indices already present for the given submesh! "
                        "Otherwise call clear() before, so the gpu buffer will be recreated." );
            }
            else // Check if subMeshIndex is valid
            {
                String errorMessage = "The submesh index is invalid. It must be in in ascending order."
                                      " The next index would be: " + TS( m_pIndexBuffers.size() );
                ASSERT( subMeshIndex == m_pIndexBuffers.size() && "Invalid submesh index." );
            }
        }
#endif
        if ( not hasSubMesh( subMeshIndex ) )
        {
            auto& sm = AddSubMesh( indices, topology, baseVertex );

            switch ( sm.indexFormat )
            {
                case IndexFormat::U16:
                {
                    ArrayList<U16> indicesU16;
                    for ( auto& index : sm.indices )
                        indicesU16.push_back( index );
                    auto pIndexBuffer = new D3D11::IndexBuffer( indicesU16.data(), getIndexCount( subMeshIndex ) * sizeof( U16 ), m_bufferUsage );
                    m_pIndexBuffers.push_back( pIndexBuffer );
                    break;
                }
                case IndexFormat::U32:
                {
                    auto pIndexBuffer = new D3D11::IndexBuffer( sm.indices.data(), getIndexCount( subMeshIndex ) * sizeof( U32 ), m_bufferUsage );
                    m_pIndexBuffers.push_back( pIndexBuffer );
                    break;
                }
            }
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            auto& sm = m_subMeshes[subMeshIndex];
            sm.indices      = indices;
            sm.baseVertex   = baseVertex;

            m_queuedBufferUpdates.push( { MeshBufferType::Index, subMeshIndex } );
        }
    }

    //----------------------------------------------------------------------
    void Mesh::setColors( const ArrayList<Color>& colors )
    {
        if (m_pColorBuffer != nullptr)
            ASSERT( m_vertexColors.size() == colors.size() &&
                "IMesh::setColors(): The amount of colors given must be the number of colors already present! "
                "Otherwise call clear() before, so the gpu buffer will be recreated.");

        m_vertexColors = colors;
        if (m_pColorBuffer == nullptr)
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
            m_pColorBuffer = new VertexBuffer( colorsNormalized.data(), static_cast<U32>( colorsNormalized.size() ) * sizeof( F32 ), m_bufferUsage );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );
            m_queuedBufferUpdates.push( { MeshBufferType::Color } );
        }
    }

    //----------------------------------------------------------------------
    void Mesh::setUVs( const ArrayList<Math::Vec2>& uvs )
    {
#if _DEBUG
        if (m_pUVBuffer != nullptr)
            ASSERT( (m_uvs0.size() == m_uvs0.size() &&
                "IMesh::setUVs(): The amount of uvs given must be the number of uvs already present! "
                "Otherwise call clear() before, so the gpu buffer will be recreated.") );
#endif
        m_uvs0 = uvs;
        if (m_pUVBuffer == nullptr)
        {
            m_pUVBuffer = new VertexBuffer( m_uvs0.data(), getVertexCount() * sizeof( Math::Vec2 ), m_bufferUsage );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );
            m_queuedBufferUpdates.push( { MeshBufferType::TexCoord } );
        }
    }

    //----------------------------------------------------------------------
    void Mesh::setNormals(const ArrayList<Math::Vec3>& normals)
    {
#if _DEBUG
        if (m_pNormalBuffer != nullptr)
            ASSERT((m_normals.size() == normals.size() &&
                "IMesh::setNormals(): The amount of normals given must be the number of normals already present! "
                "Otherwise call clear() before, so the gpu buffer will be recreated."));
#endif
        m_normals = normals;
        if (m_pNormalBuffer == nullptr)
        {
            m_pNormalBuffer = new VertexBuffer( normals.data(), getVertexCount() * sizeof( Math::Vec3 ), m_bufferUsage );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                   "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh.");
            m_queuedBufferUpdates.push({ MeshBufferType::Normal });
        }
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
        // Update index-buffer
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
            setVertices( m_vertices );
        }

        // Recreate color buffer
        if (m_pColorBuffer)
        {
            SAFE_DELETE( m_pColorBuffer );
            setColors( m_vertexColors );
        }

        // Recreate uv buffer
        if (m_pColorBuffer)
        {
            SAFE_DELETE( m_pUVBuffer );
            setUVs( m_uvs0 );
        }

        // Recreate normal buffer
        if (m_pNormalBuffer)
        {
            SAFE_DELETE( m_pNormalBuffer );
            setNormals( m_normals );
        }

        // Recreate index buffers
        for (auto& indexBuffer : m_pIndexBuffers)
            SAFE_DELETE( indexBuffer );
        m_pIndexBuffers.clear();

        auto subMeshes = m_subMeshes;
        m_subMeshes.clear();
        for (U32 i = 0; i < subMeshes.size(); i++)
            setIndices( subMeshes[i].indices, i, subMeshes[i].topology, subMeshes[i].baseVertex );
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
        }
        g_pImmediateContext->IASetPrimitiveTopology( dxTopology );
    }

    //----------------------------------------------------------------------
    void Mesh::_BindVertexBuffer( const VertexLayout& vertLayout, U32 subMesh )
    {
        auto& vertexDescription = vertLayout.getLayoutDescription();

        ArrayList<ID3D11Buffer*> pBuffers;
        ArrayList<U32>           strides;
        ArrayList<U32>           offsets;
        for ( auto& binding : vertexDescription )
        {
            switch (binding.type)
            {
            case InputLayoutType::POSITION:
            {
                pBuffers.emplace_back( m_pVertexBuffer->getBuffer() );
                strides.emplace_back( static_cast<U32>( sizeof( Math::Vec3 ) ) );
                offsets.emplace_back( 0 );
                break;
            }
            case InputLayoutType::COLOR:
            {
                ASSERT( m_pColorBuffer && "Shader requires a color-buffer, but mesh has none!" );
                pBuffers.emplace_back( m_pColorBuffer->getBuffer() );
                strides.emplace_back( static_cast<U32>( sizeof( F32 ) * 4 ) );
                offsets.emplace_back( 0 );
                break;
            }
            case InputLayoutType::TEXCOORD0:
            {
                ASSERT( m_pUVBuffer && "Shader requires a uv-buffer, but mesh has none!" );
                pBuffers.emplace_back( m_pUVBuffer->getBuffer() );
                strides.emplace_back( static_cast<U32>( sizeof( Math::Vec2 ) ) );
                offsets.emplace_back( 0 );
                break;
            }
            case InputLayoutType::NORMAL:
            {
                ASSERT( m_pNormalBuffer && "Shader requires a normal-buffer, but mesh has none!");
                pBuffers.emplace_back( m_pNormalBuffer->getBuffer() );
                strides.emplace_back( static_cast<U32>( sizeof( Math::Vec3 ) ) );
                offsets.emplace_back( 0 );
                break;
            }
            default:
                ASSERT( false && "implement!" );
            }
        }

        // Bind vertex buffers
        g_pImmediateContext->IASetVertexBuffers( 0, static_cast<U32>( pBuffers.size() ), pBuffers.data(), strides.data(), offsets.data() );
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