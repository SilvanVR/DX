#pragma once
/**********************************************************************
    class: IMesh (i_mesh.h)

    author: S. Hau
    date: March 8, 2018

    Interface for a Mesh class. It represents one mesh, which
    consists of geometry data like positions, normals, colors. The
    actual pipeline from a material will then fetch this data and
    render this mesh.
**********************************************************************/

#include "enums.hpp"

namespace Graphics {

    //**********************************************************************
    class IMesh
    {
    public:
        IMesh() = default;
        virtual ~IMesh() {}

        //----------------------------------------------------------------------
        // Destroy the vertex- and indices buffers. This should be called before
        // calling setVertices() when you want to dynamically generate a mesh
        // every frame with different amounts of vertices.
        //----------------------------------------------------------------------
        virtual void clear() = 0;

        //----------------------------------------------------------------------
        // Binds this IMesh to the pipeline. Subsequent drawcalls render this IMesh.
        //----------------------------------------------------------------------
        virtual void bind(U32 subMesh = 0) = 0;

        //----------------------------------------------------------------------
        // Sets the vertices for this mesh. If a vertex buffer was not created,
        // it will be created to fit the amount of data given. Otherwise, the gpu
        // buffer will just be updated. Note that this is a slow operation.
        //----------------------------------------------------------------------
        virtual void setVertices(const ArrayList<Math::Vec3>& vertices) = 0;

        //----------------------------------------------------------------------
        // Set the index-buffer for this mesh.
        //----------------------------------------------------------------------
        virtual void setTriangles(const ArrayList<U32>& indices, U32 subMesh = 0, U32 baseVertex = 0) = 0;

        //----------------------------------------------------------------------
        // Set the color-buffer for this mesh.
        //----------------------------------------------------------------------
        virtual void setColors(const ArrayList<Color>& colors) = 0;

        //----------------------------------------------------------------------
        const ArrayList<Math::Vec3>&    getVertices()       const { return m_vertices; }
        const ArrayList<Color>&         getColors()         const { return m_vertexColors; }
        U32                             getVertexCount()    const { return static_cast<U32>( m_vertices.size() ); }
        U16                             getSubMeshCount()   const { return static_cast<U32>( m_subMeshes.size() ); }

        const ArrayList<U32>&           getIndices(U32 subMesh)     const { return m_subMeshes[subMesh].indices; }
        U32                             getIndexCount(U32 subMesh)  const { return static_cast<U32>( getIndices( subMesh ).size() ); }
        IndexFormat                     getIndexFormat(U32 subMesh) const { return m_subMeshes[subMesh].indexFormat; }
        U32                             getBaseVertex(U32 subMesh)  const { return m_subMeshes[subMesh].baseVertex; }
        bool                            hasSubMesh(U32 subMesh)     const { return subMesh < getSubMeshCount(); }

    protected:
        ArrayList<Math::Vec3>   m_vertices;
        ArrayList<Color>        m_vertexColors;

        struct SubMesh
        {
            U32                 baseVertex = 0;
            ArrayList<U32>      indices;
            IndexFormat         indexFormat   = IndexFormat::U16;
        };
        ArrayList<SubMesh>      m_subMeshes;


        //----------------------------------------------------------------------
        // Add a new submesh to the list of submeshes. The appropriate index-
        // format is automatically determined, based on the number of indices.
        // @Return:
        // The newly created submesh struct.
        //----------------------------------------------------------------------
        SubMesh& AddSubMesh( const ArrayList<U32>& indices, U32 baseVertex )
        {
            SubMesh sm;
            sm.indices      = indices;
            sm.baseVertex   = baseVertex;

            if ( indices.size() > 65535 )
                sm.indexFormat = IndexFormat::U32;

            m_subMeshes.push_back( std::move( sm ) );
            return m_subMeshes.back();
        }

    private:
        //----------------------------------------------------------------------
        IMesh(const IMesh& other)               = delete;
        IMesh& operator = (const IMesh& other)  = delete;
        IMesh(IMesh&& other)                    = delete;
        IMesh& operator = (IMesh&& other)       = delete;
    };

    using Mesh = IMesh;

} // End namespaces

//**********************************************************************