#include "i_mesh.h"
/**********************************************************************
    class: IMesh (i_mesh.cpp)

    author: S. Hau
    date: March 30, 2018
**********************************************************************/

#include "Logging/logging.h"

namespace Graphics {

    const StringID SID_VERTEX_POSITION   = SID("POSITION");
    const StringID SID_VERTEX_COLOR      = SID("COLOR");
    const StringID SID_VERTEX_UV         = SID("TEXCOORD");
    const StringID SID_VERTEX_NORMAL     = SID("NORMAL");
    const StringID SID_VERTEX_TANGENT    = SID("TANGENT");

    //----------------------------------------------------------------------
    IMesh::~IMesh()
    {
        for (auto&[name, vsStream] : m_vertexStreams)
            SAFE_DELETE( vsStream );
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void IMesh::clear()
    {
        for (auto& [name, vsStream] : m_vertexStreams)
            SAFE_DELETE( vsStream );
        m_vertexStreams.clear();
        m_subMeshes.clear();
        _Clear();
    }

    //----------------------------------------------------------------------
    void IMesh::_SetVertexStream( StringID name, VertexStreamBase* vs )
    {
        SAFE_DELETE( m_vertexStreams[name] );
        m_vertexStreams[name] = vs;
        _DestroyBuffer( name );
        _CreateBuffer( name, *vs );
    }

    //----------------------------------------------------------------------
    const ArrayList<Math::Vec3>& IMesh::getVertexPositions() const
    {
        ASSERT( hasVertexStream(SID_VERTEX_POSITION) && "Mesh hasn't this stream." );

        auto& vsBase = m_vertexStreams.at( SID_VERTEX_POSITION );
        VertexStream<Math::Vec3>* vs = dynamic_cast<VertexStream<Math::Vec3>*>( vsBase );
        if (not vs)
            LOG_ERROR_RENDERING( "IMesh::getVertexPositions(): Something went horribly wrong. Investigate this!" );
        return vs->get();
    }

    //----------------------------------------------------------------------
    const ArrayList<Math::Vec2>& IMesh::getUVs() const
    {
        ASSERT( hasVertexStream( SID_VERTEX_UV ) && "Mesh hasn't this stream." );

        auto& vsBase = m_vertexStreams.at( SID_VERTEX_UV );
        VertexStream<Math::Vec2>* vs = dynamic_cast<VertexStream<Math::Vec2>*>( vsBase );
        if (not vs)
            LOG_ERROR_RENDERING( "IMesh::getUVs(): Something went horribly wrong. Investigate this!" );
        return vs->get();
    }

    //----------------------------------------------------------------------
    const ArrayList<Math::Vec3>& IMesh::getNormals() const
    {
        ASSERT( hasVertexStream( SID_VERTEX_NORMAL ) && "Mesh hasn't this stream." );

        auto& vsBase = m_vertexStreams.at( SID_VERTEX_NORMAL );
        VertexStream<Math::Vec3>* vs = dynamic_cast<VertexStream<Math::Vec3>*>( vsBase );
        if (not vs)
            LOG_ERROR_RENDERING( "IMesh::getNormals(): Something went horribly wrong. Investigate this!" );
        return vs->get();
    }

    //----------------------------------------------------------------------
    const ArrayList<Math::Vec4>& IMesh::getTangents() const
    {
        ASSERT( hasVertexStream( SID_VERTEX_TANGENT ) && "Mesh hasn't this stream." );

        auto& vsBase = m_vertexStreams.at( SID_VERTEX_TANGENT );
        VertexStream<Math::Vec4>* vs = dynamic_cast<VertexStream<Math::Vec4>*>( vsBase );
        if (not vs)
            LOG_ERROR_RENDERING( "IMesh::getTangents(): Something went horribly wrong. Investigate this!" );
        return vs->get();
    }

    //----------------------------------------------------------------------
    VertexStream<Math::Vec3>& IMesh::setVertices( const ArrayList<Math::Vec3>& vertices )
    {
        if ( hasVertexStream( SID_VERTEX_POSITION ) )
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

        auto& vsStream = createVertexStream<Math::Vec3>( SID_VERTEX_POSITION, vertices );
        _RecalculateBounds( vsStream.get() );
        return vsStream;
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

            m_queuedIndexBufferUpdates.push( subMeshIndex );
        }
    }

    //----------------------------------------------------------------------
    VertexStream<Math::Vec4>& IMesh::setColors( const ArrayList<Color>& colors )
    {
        if ( hasVertexStream( SID_VERTEX_COLOR ) )
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

        ArrayList<Math::Vec4> colorsNormalized( colors.size() );
        for (U32 i = 0; i < colors.size(); i++)
        {
            auto normalized = colors[i].normalized();
            colorsNormalized[i] = { normalized[0], normalized[1], normalized[2], normalized[3] };
        }

        return createVertexStream<Math::Vec4>( SID_VERTEX_COLOR, colorsNormalized );
    }

    //----------------------------------------------------------------------
    VertexStream<Math::Vec2>& IMesh::setUVs( const ArrayList<Math::Vec2>& uvs )
    {
        if ( hasVertexStream( SID_VERTEX_UV ) )
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

        return createVertexStream<Math::Vec2>( SID_VERTEX_UV, uvs );
    }

    //----------------------------------------------------------------------
    VertexStream<Math::Vec3>& IMesh::setNormals( const ArrayList<Math::Vec3>& normals )
    {
        if ( hasVertexStream( SID_VERTEX_NORMAL ) )
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

        return createVertexStream<Math::Vec3>( SID_VERTEX_NORMAL, normals );
    }

    //----------------------------------------------------------------------
    VertexStream<Math::Vec4>& IMesh::setTangents( const ArrayList<Math::Vec4>& tangents )
    {
        if ( hasVertexStream( SID_VERTEX_TANGENT ) )
            ASSERT( not isImmutable() && "Mesh is immutable! It can't be updated. "
                    "Either change the buffer usage via setBufferUsage() or call clear() to reset the whole mesh." );

        return createVertexStream<Math::Vec4>( SID_VERTEX_TANGENT, tangents);
    }

    //----------------------------------------------------------------------
    void IMesh::recalculateNormals()
    {
        const auto& vertices = getVertexPositions();
        ArrayList<Math::Vec3> normals( vertices.size(), Math::Vec3( 0.0f ) );

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

                auto vert0 = vertices[ index0 ];
                auto vert1 = vertices[ index1 ];
                auto vert2 = vertices[ index2 ];

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
        const auto& vertices = getVertexPositions();
        ArrayList<Math::Vec3> tangents( vertices.size(), Math::Vec3( 0.0f ) );

        // Calculate normals
        const auto& uvs = getUVs();
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

                auto vert0 = vertices[index0];
                auto vert1 = vertices[index1];
                auto vert2 = vertices[index2];

                auto edge0 = vert1 - vert0;
                auto edge1 = vert2 - vert0;

                F32 deltaU1 = uvs[index1].x - uvs[index0].x;
                F32 deltaV1 = uvs[index1].y - uvs[index0].y;
                F32 deltaU2 = uvs[index2].x - uvs[index0].x;
                F32 deltaV2 = uvs[index2].y - uvs[index0].y;

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
    void IMesh::_RecalculateBounds( const ArrayList<Math::Vec3>& vertexPositions )
    {
        for (auto& vert : vertexPositions)
        {
            m_bounds.getMin() = vert.minVec( m_bounds.getMin() );
            m_bounds.getMax() = vert.maxVec( m_bounds.getMax() );
        }
    }

} // End namespaces