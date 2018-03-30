#include "i_mesh.h"
/**********************************************************************
    class: IMesh (i_mesh.cpp)

    author: S. Hau
    date: March 30, 2018
**********************************************************************/

namespace Graphics {

    //----------------------------------------------------------------------
    void IMesh::setVertices( const ArrayList<Math::Vec3>& vertices )
    {
        bool hasBuffer = not m_vertices.empty();

        m_vertices = vertices;
        if ( not hasBuffer )
        {
            _CreateVertexBuffer();
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
    }

    //----------------------------------------------------------------------
    void IMesh::setUVs( const ArrayList<Math::Vec2>& uvs )
    {
        bool hasBuffer = not m_uvs0.empty();

        m_uvs0 = uvs;
        if ( not hasBuffer )
        {
            _CreateUVBuffer();
        }
        else
        {
            ASSERT( (m_uvs0.size() == m_uvs0.size() &&
                    "IMesh::setUVs(): The amount of uvs given must be the number of uvs already present! "
                    "Otherwise call clear() before, so the gpu buffer will be recreated.") );
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );
            m_queuedBufferUpdates.push({ MeshBufferType::TexCoord });
        }
    }

    //----------------------------------------------------------------------
    //SubMesh& IMesh::AddSubMesh( const ArrayList<U32>& indices, MeshTopology topology, U32 baseVertex )
    //{
    //    SubMesh sm;
    //    sm.indices      = indices;
    //    sm.baseVertex   = baseVertex;
    //    sm.topology     = topology;

    //    if ( indices.size() > 65535 )
    //        sm.indexFormat = IndexFormat::U32;

    //    m_subMeshes.push_back( std::move( sm ) );
    //    return m_subMeshes.back();
    //}

} // End namespaces