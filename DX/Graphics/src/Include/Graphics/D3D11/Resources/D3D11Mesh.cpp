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
    D3D11Mesh::D3D11Mesh()
    {
        //pVertexBuffer = new D3D11::VertexBuffer( sizeInBytes, pVertices );
        //pIndexBuffer = new D3D11::IndexBuffer( sizeInBytes2, pIndices );
    }

    //----------------------------------------------------------------------
    D3D11Mesh::~D3D11Mesh()
    {
        if (pVertexBuffer)
            SAFE_DELETE( pVertexBuffer );
        if (pIndexBuffer)
            SAFE_DELETE( pIndexBuffer );
        if (pColorBuffer)
            SAFE_DELETE( pColorBuffer );
    }

    //----------------------------------------------------------------------
    void D3D11Mesh::bind()
    {
        // @TODO: Build vertexlayout automatically based on present arrays
        pVertexBuffer->bind( 0, sizeof(Math::Vec3), 0 );
        if (pColorBuffer != nullptr)
            pColorBuffer->bind( 1, sizeof(F32) * 4, 0 );

        //U32 strides[] = { sizeof(Math::Vec3), sizeof(F32) * 4 };
        //U32 offsets[] = { 0,0 };
        //ID3D11Buffer* pBuffers[] = { pVertexBuffer->getBuffer(), pColorBuffer->getBuffer() };
        //g_pImmediateContext->IASetVertexBuffers(0, 2, pBuffers, strides, offsets);

        switch (m_indexFormat)
        {
        case IndexFormat::U16:  pIndexBuffer->bind( DXGI_FORMAT_R16_UINT, 0 );  break;
        case IndexFormat::U32:  pIndexBuffer->bind( DXGI_FORMAT_R32_UINT, 0 );  break;
        }
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void D3D11Mesh::clear()
    {

    }

    //----------------------------------------------------------------------
    void D3D11Mesh::setVertices( const ArrayList<Math::Vec3>& vertices )
    {
        if (pVertexBuffer != nullptr)
            ASSERT( (m_vertices.size() == vertices.size() &&
                    "IMesh::setVertices(): The amount of vertices given must be the number of vertices already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated.") );

        m_vertices = vertices;
        if (pVertexBuffer == nullptr)
        {
            pVertexBuffer = new VertexBuffer( m_vertices.data(), numVertices() * sizeof( Math::Vec3 ) );
        }
        else
        {
            // Update vertex-buffer
        }
    }

    //----------------------------------------------------------------------
    void D3D11Mesh::setTriangles( const ArrayList<U32>& indices )
    {
        if (pIndexBuffer != nullptr)
            ASSERT( m_indices.size() == m_indices.size() &&
                    "IMesh::setTriangles(): The amount of indices given must be the number of indices already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated." );

        m_indices = indices;
        if (pIndexBuffer == nullptr)
        {
            if (numIndices() > 65535)
                m_indexFormat = IndexFormat::U32;

            switch (m_indexFormat)
            {
                case IndexFormat::U16:
                {
                    ArrayList<U16> indicesU16;
                    for ( auto& index : m_indices )
                        indicesU16.push_back( index );
                    pIndexBuffer = new D3D11::IndexBuffer( indicesU16.data(), numIndices() * sizeof( U16 ) );
                    break;
                }
                case IndexFormat::U32:
                {
                    pIndexBuffer = new D3D11::IndexBuffer( m_indices.data(), numIndices() * sizeof( U32 ) );
                    break;
                }
            }
        }
        else
        {
            // Update index-buffer
        }
    }

    //----------------------------------------------------------------------
    void D3D11Mesh::setColors( const ArrayList<Color>& colors )
    {
        if (pColorBuffer != nullptr)
            ASSERT( m_vertexColors.size() == colors.size() &&
                "IMesh::setColors(): The amount of colors given must be the number of colors already present! "
                "Otherwise call clear() before, so the gpu buffer will be recreated.");

        m_vertexColors = colors;
        if (pColorBuffer == nullptr)
        {
            ArrayList<F32> colorsNormalized;
            for (auto& color : colors)
            {
                auto normalized = color.normalized();
                colorsNormalized.push_back( normalized[0] );
                colorsNormalized.push_back( normalized[1] );
                colorsNormalized.push_back( normalized[2] );
                colorsNormalized.push_back( normalized[3] );
            }
            pColorBuffer = new VertexBuffer( colorsNormalized.data(), static_cast<U32>( colorsNormalized.size() ) * sizeof( F32 ) );
        }
        else
        {
            // Update vertex-buffer
        }
    }


} } // End namespaces