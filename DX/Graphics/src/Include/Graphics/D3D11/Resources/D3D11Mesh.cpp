#include "D3D11Mesh.h"
/**********************************************************************
    class: D3D11Mesh (D3D11Mesh.cpp)

    author: S. Hau
    date: March 8, 2018
**********************************************************************/

#include "../Pipeline/Buffers/D3D11Buffers.h"
#include "../../structs.hpp"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    D3D11Mesh::~D3D11Mesh()
    {
        clear();
    }

    //----------------------------------------------------------------------
    void D3D11Mesh::bind( U32 subMeshIndex )
    {
        // @TODO: Move bind into actual renderpass, where geometry is drawn with a specific input layout
        m_pVertexBuffer->bind( 0, sizeof( Math::Vec3 ), 0 );
        if (m_pColorBuffer != nullptr)
            m_pColorBuffer->bind( 1, sizeof( F32 ) * 4, 0 );

        //U32 strides[] = { sizeof(Math::Vec3), sizeof(F32) * 4 };
        //U32 offsets[] = { 0,0 };
        //ID3D11Buffer* pBuffers[] = { m_pVertexBuffer->getBuffer(), m_pColorBuffer->getBuffer() };
        //g_pImmediateContext->IASetVertexBuffers(0, 2, pBuffers, strides, offsets);

        DXGI_FORMAT dxIndexFormat;
        switch ( m_subMeshes[subMeshIndex].indexFormat )
        {
        case IndexFormat::U16: dxIndexFormat = DXGI_FORMAT_R16_UINT; break;
        case IndexFormat::U32: dxIndexFormat = DXGI_FORMAT_R32_UINT; break;
        }

        m_pIndexBuffers[subMeshIndex]->bind( dxIndexFormat, 0 ); 
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Mesh::clear()
    {
        if (m_pVertexBuffer)
            SAFE_DELETE( m_pVertexBuffer );
        if (m_pColorBuffer)
            SAFE_DELETE( m_pColorBuffer );
        for (auto& indexBuffer : m_pIndexBuffers)
            SAFE_DELETE( indexBuffer );

        m_vertices.clear();
        m_vertexColors.clear();
        m_pIndexBuffers.clear();
        m_subMeshes.clear();
    }

    //----------------------------------------------------------------------
    void D3D11Mesh::setVertices( const ArrayList<Math::Vec3>& vertices )
    {
        if (m_pVertexBuffer != nullptr)
            ASSERT( (m_vertices.size() == vertices.size() &&
                    "IMesh::setVertices(): The amount of vertices given must be the number of vertices already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated.") );

        m_vertices = vertices;
        if (m_pVertexBuffer == nullptr)
        {
            m_pVertexBuffer = new VertexBuffer( m_vertices.data(), getVertexCount() * sizeof( Math::Vec3 ), m_bufferUsage );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            m_pVertexBuffer->update( m_vertices.data(), m_vertices.size() * sizeof( Math::Vec3 ) );
        }
    }

    //----------------------------------------------------------------------
    void D3D11Mesh::setIndices( const ArrayList<U32>& indices, U32 subMeshIndex, U32 baseVertex )
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
            auto& sm = AddSubMesh( indices, baseVertex );

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

            // Update index-buffer
            Size indexSize = 0;
            switch ( getIndexFormat( subMeshIndex ) )
            {
            case IndexFormat::U16: indexSize = sizeof( U16 ); break;
            case IndexFormat::U32: indexSize = sizeof( U32 ); break;
            }
            m_pIndexBuffers[subMeshIndex]->update( sm.indices.data(), sm.indices.size() * indexSize );
        }
    }

    //----------------------------------------------------------------------
    void D3D11Mesh::setColors( const ArrayList<Color>& colors )
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
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Mesh::recreateBuffers()
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

        // Recreate index buffers
        for (auto& indexBuffer : m_pIndexBuffers)
            SAFE_DELETE( indexBuffer );
        m_pIndexBuffers.clear();

        auto subMeshes = m_subMeshes;
        m_subMeshes.clear();
        for (U32 i = 0; i < subMeshes.size(); i++)
            setIndices( subMeshes[i].indices, i, subMeshes[i].baseVertex );
    }


} } // End namespaces