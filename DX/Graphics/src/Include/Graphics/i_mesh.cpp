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
        m_subMeshes.clear();
        _Clear();
    }

    //----------------------------------------------------------------------
    void IMesh::setVertices( const ArrayList<Math::Vec3>& vertices )
    {
        bool hasBuffer = not m_vertices.empty();

        m_vertices = vertices;
        if ( not hasBuffer )
        {
            _CreateVertexBuffer( vertices );
        }
        else
        {
            ASSERT( (m_vertices.size() == vertices.size() &&
                    "IMesh::setVertices(): The amount of vertices given must be the number of vertices already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated.") );
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );
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
            auto& subMesh = m_subMeshes[subMeshIndex];
            ASSERT( (subMesh.indices.size() == indices.size() &&
                    "IMesh::setIndices(): The amount of indices given must be the number of indices already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated.") );
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

            subMesh.indices      = indices;
            subMesh.baseVertex   = baseVertex;
            subMesh.topology     = topology;

            m_queuedBufferUpdates.push({ MeshBufferType::Index, subMeshIndex });
        }
    }

    //----------------------------------------------------------------------
    void IMesh::setColors( const ArrayList<Color>& colors )
    {
        bool hasBuffer = not m_colors.empty();

        m_colors = colors;
        if ( not hasBuffer )
        {
            _CreateColorBuffer( m_colors );
        }
        else
        {
            ASSERT( (m_colors.size() == colors.size() &&
                    "IMesh::setColors(): The amount of colors given must be the number of colors already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated.") );
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );
            m_queuedBufferUpdates.push({ MeshBufferType::Color });
        }
    }

    //----------------------------------------------------------------------
    void IMesh::setUVs( const ArrayList<Math::Vec2>& uvs )
    {
        bool hasBuffer = not m_uvs0.empty();

        m_uvs0 = uvs;
        if ( not hasBuffer )
        {
            _CreateUVBuffer( uvs );
        }
        else
        {
            ASSERT( (m_uvs0.size() == uvs.size() &&
                    "IMesh::setUVs(): The amount of uvs given must be the number of uvs already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated.") );
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );
            m_queuedBufferUpdates.push({ MeshBufferType::TexCoord });
        }
    }

    //----------------------------------------------------------------------
    void IMesh::setNormals( const ArrayList<Math::Vec3>& normals )
    {
        bool hasBuffer = not m_normals.empty();

        m_normals = normals;
        if ( not hasBuffer )
        {
            _CreateNormalBuffer( m_normals );
        }
        else
        {
            ASSERT( (m_normals.size() == normals.size() &&
                    "IMesh::setNormals(): The amount of normals given must be the number of normals already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated.") );
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );
            m_queuedBufferUpdates.push({ MeshBufferType::Normal });
        }
    }

    //----------------------------------------------------------------------
    void IMesh::recalculateNormals()
    {
        ArrayList<Math::Vec3> normals( m_vertices.size(), Math::Vec3( 0.0f ) );


        // Calcualte normals
        for (auto& subMesh : m_subMeshes)
        {
            if (subMesh.topology != MeshTopology::Triangles)
            {
                LOG_WARN_RENDERING( "IMesh::recalculateNormals(): Normal recalculation not supported for this (sub)mesh topology!" );
                continue;
            }

            for (I32 i = 0; i < subMesh.indices.size(); i = i + 3)
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

        if ( indices.size() > 65535 )
            sm.indexFormat = IndexFormat::U32;

        m_subMeshes.push_back( std::move( sm ) );
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