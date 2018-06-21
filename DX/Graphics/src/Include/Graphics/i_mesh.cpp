#include "i_mesh.h"
/**********************************************************************
    class: IMesh (i_mesh.cpp)

    author: S. Hau
    date: March 30, 2018
**********************************************************************/

#include "Logging/logging.h"

namespace Graphics {

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void IMesh::clear()
    {
        m_vertices.clear();
        m_colors.clear();
        m_uvs0.clear();
        m_normals.clear();
        m_tangents.clear();
        m_subMeshes.clear();
        _Clear();
    }

    //----------------------------------------------------------------------
    void IMesh::setVertices( const ArrayList<Math::Vec3>& vertices )
    {
        bool hasBuffer = not m_vertices.empty();

        if ( not hasBuffer )
        {
            m_vertices = vertices;
            _CreateVertexBuffer( vertices );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            // Buffer always grows but never shrink
            bool enoughCapacity = vertices.size() <= m_vertices.size();
            if (not enoughCapacity)
            {
                m_vertices.resize( vertices.size() );

                _DestroyVertexBuffer();
                _CreateVertexBuffer( m_vertices );
            }

            // Copy new vertex data into vertex array
            memcpy( m_vertices.data(), vertices.data(), vertices.size() * sizeof( Math::Vec3 ) );

            m_queuedBufferUpdates.push({ MeshBufferType::Vertex });
        }
        _RecalculateBounds();
    }

    //----------------------------------------------------------------------
    void IMesh::setIndices( const ArrayList<U32>& indices, U32 subMeshIndex, MeshTopology topology, U32 baseVertex )
    {
        bool hasBuffer = hasSubMesh( subMeshIndex );

        if ( not hasBuffer )
        {
            String errorMessage = "The submesh index is invalid. It must be in in ascending order. "
                                  "The next index would be: " + TS( m_subMeshes.size() );
            ASSERT( subMeshIndex == m_subMeshes.size() && errorMessage.c_str() );

            auto& sm = _AddSubMesh( indices, topology, baseVertex );
            _CreateIndexBuffer( sm, subMeshIndex );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            // Buffer always grows but never shrink
            auto& subMesh = m_subMeshes[subMeshIndex];
            subMesh.baseVertex  = baseVertex;
            subMesh.topology    = topology;
            subMesh.indexCount  = (U32)indices.size();

            bool enoughCapacity = indices.size() <= subMesh.indices.size();
            if (not enoughCapacity)
            {
                subMesh.indices.resize( indices.size() );

                _DestroyIndexBuffer( subMeshIndex );
                _CreateIndexBuffer( subMesh, subMeshIndex );
            }

            // Copy new index data into index array
            memcpy( subMesh.indices.data(), indices.data(), indices.size() * sizeof( U32 ) );

            m_queuedBufferUpdates.push({ MeshBufferType::Index, subMeshIndex });
        }
    }

    //----------------------------------------------------------------------
    void IMesh::setColors( const ArrayList<Color>& colors )
    {
        bool hasBuffer = not m_colors.empty();

        if ( not hasBuffer )
        {
            m_colors = colors;
            _CreateColorBuffer( colors );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            // Buffer always grows but never shrink
            bool enoughCapacity = colors.size() <= m_colors.size();
            if (not enoughCapacity)
            {
                m_colors.resize( colors.size() );

                _DestroyColorBuffer();
                _CreateColorBuffer( m_colors );
            }

            // Copy new color data into color array
            memcpy( m_colors.data(), colors.data(), colors.size() * sizeof( Color ) );

            m_queuedBufferUpdates.push({ MeshBufferType::Color });
        }
    }

    //----------------------------------------------------------------------
    void IMesh::setUVs( const ArrayList<Math::Vec2>& uvs )
    {
        bool hasBuffer = not m_uvs0.empty();

        if ( not hasBuffer )
        {
            m_uvs0 = uvs;
            _CreateUVBuffer( uvs );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            // Buffer always grows but never shrink
            bool enoughCapacity = uvs.size() <= m_uvs0.size();
            if (not enoughCapacity)
            {
                m_uvs0.resize( uvs.size() );

                _DestroyUVBuffer();
                _CreateUVBuffer( m_uvs0 );
            }

            // Copy new uv data into uv array
            memcpy( m_uvs0.data(), uvs.data(), uvs.size() * sizeof( Math::Vec2 ) );

            m_queuedBufferUpdates.push({ MeshBufferType::TexCoord });
        }
    }

    //----------------------------------------------------------------------
    void IMesh::setNormals( const ArrayList<Math::Vec3>& normals )
    {
        bool hasBuffer = not m_normals.empty();

        if ( not hasBuffer )
        {
            m_normals = normals;
            _CreateNormalBuffer( m_normals );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            // Buffer always grows but never shrink
            bool enoughCapacity = normals.size() <= m_normals.size();
            if (not enoughCapacity)
            {
                m_normals.resize( normals.size() );

                _DestroyNormalBuffer();
                _CreateNormalBuffer( m_normals );
            }

            // Copy new uv data into uv array
            memcpy( m_normals.data(), normals.data(), normals.size() * sizeof( Math::Vec3 ) );

            m_queuedBufferUpdates.push({ MeshBufferType::Normal });
        }
    }

    //----------------------------------------------------------------------
    void IMesh::setTangents( const ArrayList<Math::Vec4>& tangents )
    {
        bool hasBuffer = not m_tangents.empty();

        if ( not hasBuffer )
        {
            m_tangents = tangents;
            _CreateTangentBuffer( m_tangents );
        }
        else
        {
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            // Buffer always grows but never shrink
            bool enoughCapacity = tangents.size() <= m_tangents.size();
            if (not enoughCapacity)
            {
                m_tangents.resize( tangents.size() );

                _DestroyTangentBuffer();
                _CreateTangentBuffer( m_tangents );
            }

            // Copy new uv data into uv array
            memcpy( m_tangents.data(), tangents.data(), tangents.size() * sizeof( Math::Vec4 ) );

            m_queuedBufferUpdates.push({ MeshBufferType::Tangent });
        }
    }

    //----------------------------------------------------------------------
    void IMesh::recalculateNormals()
    {
        ArrayList<Math::Vec3> normals( m_vertices.size(), Math::Vec3( 0.0f ) );

        // Calculate normals
        for (auto& subMesh : m_subMeshes)
        {
            if (subMesh.topology != MeshTopology::Triangles)
            {
                LOG_WARN_RENDERING( "IMesh::recalculateNormals(): Normal recalculation not supported for this (sub)mesh topology!" );
                continue;
            }

            for (I32 i = 0; i < subMesh.indices.size(); i += 3)
            {
                U32 index0 = subMesh.indices[ i + 0 ];
                U32 index1 = subMesh.indices[ i + 1 ];
                U32 index2 = subMesh.indices[ i + 2 ];

                auto vert0 = m_vertices[ index0 ];
                auto vert1 = m_vertices[ index1 ];
                auto vert2 = m_vertices[ index2 ];

                auto edge0 = vert1 - vert0;
                auto edge1 = vert2 - vert0;

                auto normal = edge0.cross( edge1 );

                normals[ index0 ] += normal;
                normals[ index1 ] += normal;
                normals[ index2 ] += normal;
            }
        }

        for (I32 i = 0; i < normals.size(); i++)
            normals[i].normalize();

        setNormals( normals );
    }

    //----------------------------------------------------------------------
    void IMesh::recalculateTangents( bool invertBinormal )
    {
        ArrayList<Math::Vec3> tangents( m_vertices.size(), Math::Vec3( 0.0f ) );

        // Calculate normals
        for (auto& subMesh : m_subMeshes)
        {
            if (subMesh.topology != MeshTopology::Triangles)
            {
                LOG_WARN_RENDERING( "IMesh::recalculateTangents(): Tangent recalculation not supported for this (sub)mesh topology!" );
                continue;
            }

            for (I32 i = 0; i < subMesh.indices.size(); i += 3)
            {
                U32 index0 = subMesh.indices[i + 0];
                U32 index1 = subMesh.indices[i + 1];
                U32 index2 = subMesh.indices[i + 2];

                auto vert0 = m_vertices[index0];
                auto vert1 = m_vertices[index1];
                auto vert2 = m_vertices[index2];

                auto edge0 = vert1 - vert0;
                auto edge1 = vert2 - vert0;

                F32 deltaU1 = m_uvs0[index1].x - m_uvs0[index0].x;
                F32 deltaV1 = m_uvs0[index1].y - m_uvs0[index0].y;
                F32 deltaU2 = m_uvs0[index2].x - m_uvs0[index0].x;
                F32 deltaV2 = m_uvs0[index2].y - m_uvs0[index0].y;

                F32 f = 1.0f / (deltaU1*deltaV2 - deltaU2*deltaV1);

                Math::Vec3 tangent;
                tangent.x = f * (deltaV2*edge0.x - deltaV1*edge1.x);
                tangent.y = f * (deltaV2*edge0.y - deltaV1*edge1.y);
                tangent.z = f * (deltaV2*edge0.z - deltaV1*edge1.z);

                tangents[index0] += tangent;
                tangents[index1] += tangent;
                tangents[index2] += tangent;
            }
        }

        for (I32 i = 0; i < tangents.size(); i++)
            tangents[i].normalize();

        // Now add the w component
        ArrayList<Math::Vec4> tangentsVec4( tangents.size(), Math::Vec4(0.0f) );
        for (I32 i = 0; i < tangents.size(); i++)
            tangentsVec4[i] = { tangents[i].x, tangents[i].y, tangents[i].z, invertBinormal ? -1.0f : 1.0f };

        setTangents( tangentsVec4 );
    }

    //----------------------------------------------------------------------
    // PROTECTED
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    // PRIVATE
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    IMesh::SubMesh& IMesh::_AddSubMesh( const ArrayList<U32>& indices, MeshTopology topology, U32 baseVertex )
    {
        SubMesh sm;
        sm.indices      = indices;
        sm.baseVertex   = baseVertex;
        sm.topology     = topology;
        sm.indexCount   = (U32)indices.size();

        if ( indices.size() > 65535 )
            sm.indexFormat = IndexFormat::U32;

        m_subMeshes.push_back( sm );
        return m_subMeshes.back();
    }

    //----------------------------------------------------------------------
    void IMesh::_RecalculateBounds()
    {
        for (auto& vert : m_vertices)
        {
            m_bounds.getMin() = vert.minVec( m_bounds.getMin() );
            m_bounds.getMax() = vert.maxVec( m_bounds.getMax() );
        }
    }

} // End namespaces