#include "VkMesh.h"
/**********************************************************************
    class: Mesh

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "structs.hpp"

namespace Graphics { namespace Vulkan {

    //----------------------------------------------------------------------
    void Mesh::bind( const VertexLayout& vertLayout, U32 subMeshIndex )
    {
        // Check if an vertex stream has been updated and perform necessary task
        for (auto& [name, vsStream] : m_vertexStreams)
        {
            if ( vsStream->wasUpdated() )
            {
                auto bufferSize = vsStream->bufferSize();
                if ( bufferSize > m_vertexBuffers[name].size )
                {
                    _DestroyBuffer( name );
                    _CreateBuffer( name, *vsStream );
                }
                else
                {
                    m_vertexBuffers[name].update( vsStream->data(), bufferSize );
                }
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
        for (auto& [name, vertexBuffer] : m_vertexBuffers)
            vertexBuffer.destroy();
        for (auto& indexBuffer : m_indexBuffers)
            indexBuffer.destroy();
        m_indexBuffers.clear();
        m_vertexBuffers.clear();
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateBuffer( StringID name, const VertexStreamBase& vs )
    {
        ASSERT( m_vertexBuffers.find( name ) != m_vertexBuffers.end() && "Buffer already exists!" );
        m_vertexBuffers[name].create( vs.data(), vs.bufferSize(), m_bufferUsage );
    }

    //----------------------------------------------------------------------
    void Mesh::_DestroyBuffer( StringID name )
    {
        m_vertexBuffers[name].destroy();
    }

    //----------------------------------------------------------------------
    void Mesh::_CreateIndexBuffer( const SubMesh& subMesh, I32 index )
    {
        if (m_indexBuffers.size() <= index)
            m_indexBuffers.resize( index + 1 );

        switch (subMesh.indexFormat )
        {
            case IndexFormat::U16:
            {
                ArrayList<U16> indicesU16( subMesh.indices.size() );
                for (U32 i = 0; i < indicesU16.size(); i++)
                    indicesU16[i] = subMesh.indices[i];
                m_indexBuffers[index].create( indicesU16.data(), U32(indicesU16.size() * sizeof(U16)), m_bufferUsage);
                break;
            }
            case IndexFormat::U32:
            {
                m_indexBuffers[index].create( subMesh.indices.data(), U32(subMesh.indices.size() * sizeof(U16)), m_bufferUsage );
                break;
            }
        }
    }

    //----------------------------------------------------------------------
    void Mesh::_DestroyIndexBuffer( I32 index ) 
    { 
        m_indexBuffers[index].destroy();
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
            m_indexBuffers[index].update( indicesU16.data(), (U32)( subMesh.indexCount * sizeof( U16 ) ) );
            break;
        }
        case IndexFormat::U32:
            m_indexBuffers[index].update( m_subMeshes[index].indices.data(), (U32)(m_subMeshes[index].indexCount * sizeof( U32 )) );
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
            _CreateBuffer( name, *vsStream );
        }

        // Recreate index buffers
        for (auto& indexBuffer : m_indexBuffers)
            indexBuffer.destroy();
        m_indexBuffers.clear();

        for (U32 i = 0; i < m_subMeshes.size(); i++)
            _CreateIndexBuffer( m_subMeshes[i], i );
    }

    //----------------------------------------------------------------------
    void Mesh::_SetTopology( U32 subMeshIndex )
    {
        VkPrimitiveTopology vkTopology;
        switch (m_subMeshes[subMeshIndex].topology)
        {
        case MeshTopology::Lines:           vkTopology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
        case MeshTopology::LineStrip:       vkTopology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
        case MeshTopology::Points:          vkTopology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
        case MeshTopology::Triangles:       vkTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
        case MeshTopology::TriangleStrip:   vkTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
        default: LOG_WARN_RENDERING( "Mesh::_SetTopology(): Unknown topology type." );
        }
        g_vulkan.ctx.IASetPrimitiveTopology( vkTopology );
    }

    //----------------------------------------------------------------------
    void Mesh::_BindVertexBuffer( const VertexLayout& vertLayout )
    {
        #define MAX_BUFFERS 8
        auto& vertexDescription = vertLayout.getLayoutDescription();

        ASSERT( vertexDescription.size() <= MAX_BUFFERS );

        VkBuffer pBuffers[MAX_BUFFERS];
        VkDeviceSize offsets[MAX_BUFFERS];
        U32 bufferIndex = 0;

        for ( auto& binding : vertexDescription )
        {
            auto it = m_vertexBuffers.find( binding.name );
            if (it != m_vertexBuffers.end())
            {
                pBuffers[bufferIndex] = it->second.buffer;
                offsets[bufferIndex] = 0;
                bufferIndex++;
            }
            else
            {
                LOG_WARN_RENDERING( "Missing vertex buffer stream '" + binding.name.toString() + "' in a mesh. Fix this!" );
            }
        }

        // Bind vertex buffers
        g_vulkan.ctx.IASetVertexBuffers( 0, bufferIndex, pBuffers, offsets );
    }

    //----------------------------------------------------------------------
    void Mesh::_BindIndexBuffer( U32 subMeshIndex )
    {
        switch (m_subMeshes[subMeshIndex].indexFormat)
        {
        case IndexFormat::U16: g_vulkan.ctx.IASetIndexBuffer( m_indexBuffers[subMeshIndex].buffer, 0, VK_INDEX_TYPE_UINT16 ); break;
        case IndexFormat::U32: g_vulkan.ctx.IASetIndexBuffer( m_indexBuffers[subMeshIndex].buffer, 0, VK_INDEX_TYPE_UINT32 ); break;
        }
    }


} } // End namespaces