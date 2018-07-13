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
        // Check if an vertex stream has been updated and perform necessary task
        for (auto& [name, vsStream] : m_vertexStreams)
        {
            if ( vsStream->wasUpdated() )
            {
                auto bufferSize = vsStream->dataSize();
                if ( bufferSize > m_pVertexBuffers[name]->getSize() )
                {
                    _DestroyBuffer( name );
                    _CreateBuffer( name, vsStream );
                }
                else
                {
                    m_pVertexBuffers[name]->update( vsStream->data(), vsStream->dataSize() );
                }
                vsStream->setWasUpdated( false );
            }
        }

        while ( not m_queuedIndexBufferUpdates.empty() )
        {
            auto subMeshIndex = m_queuedIndexBufferUpdates.front();
            _UpdateIndexBuffer( subMeshIndex );
            m_queuedIndexBufferUpdates.pop();
        }

        _SetTopology( subMeshIndex );
        _BindVertexBuffer( vertLayout ); 
        _BindIndexBuffer( subMeshIndex );
    }

    //**********************************************************************
    // PRIVATE
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
    void Mesh::_CreateBuffer( StringID name, const VertexStreamPtr& vs )
    {
        ASSERT( m_pVertexBuffers[name] == nullptr && "Buffer already exists!" );
        m_pVertexBuffers[name] = new VertexBuffer( vs->data(), vs->dataSize(), m_bufferUsage );
    }

    //----------------------------------------------------------------------
    void Mesh::_DestroyBuffer( StringID name )
    {
        SAFE_DELETE( m_pVertexBuffers[name] );
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
    void Mesh::_DestroyIndexBuffer( I32 index ) 
    { 
        SAFE_DELETE( m_pIndexBuffers[index] ); 
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
    void Mesh::_RecreateBuffers()
    {
        // Recreate all vertex buffers
        for (auto& [name, vsStream] : m_vertexStreams)
        {
            _DestroyBuffer( name );
            _CreateBuffer( name, vsStream );
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
                LOG_WARN_RENDERING( "Missing vertex buffer stream '" + binding.name.toString() + "' in a mesh. Fix this!" );
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