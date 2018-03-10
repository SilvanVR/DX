#pragma once
/**********************************************************************
    class: IMesh (i_mesh.h)

    author: S. Hau
    date: March 8, 2018

    Interface for a IMesh class. A IMesh represents one mesh, which
    can consists of several submeshes.
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
        virtual void bind() = 0;

        //----------------------------------------------------------------------
        // Sets the vertices for this mesh. If a vertex buffer was not created,
        // it will be created to fit the amount of data given. Otherwise, the gpu
        // buffer will just be updated. Note that this is a slow operation.
        //----------------------------------------------------------------------
        virtual void setVertices(const ArrayList<Math::Vec3>& vertices) = 0;

        //----------------------------------------------------------------------
        // Set the index-buffer for this mesh.
        //----------------------------------------------------------------------
        virtual void setTriangles(const ArrayList<U32>& indices) = 0;

        //----------------------------------------------------------------------
        // Set the color-buffer for this mesh.
        //----------------------------------------------------------------------
        virtual void setColors(const ArrayList<Color>& colors) = 0;

        //----------------------------------------------------------------------
        const ArrayList<Math::Vec3>&    getVertices()       const { return m_vertices; }
        const ArrayList<U32>&           getIndices()        const { return m_indices; }
        const ArrayList<Color>&         getColors()         const { return m_vertexColors; }
        U32                             numIndices()        const { return static_cast<U32>( m_indices.size() ); }
        U32                             numVertices()       const { return static_cast<U32>( m_vertices.size() ); }
        U16                             numSubMeshes()      const { return m_subMeshCount; }
        IndexFormat                     getIndexFormat()    const { return m_indexFormat; }

    protected:
        ArrayList<Math::Vec3>   m_vertices;
        ArrayList<U32>          m_indices;
        ArrayList<Color>        m_vertexColors;
        U16                     m_subMeshCount  = 1;
        IndexFormat             m_indexFormat = IndexFormat::U16;

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