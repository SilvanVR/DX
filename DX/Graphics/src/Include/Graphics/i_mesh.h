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
#include "vertex_layout.hpp"

namespace Graphics {

    class IShader;

    //**********************************************************************
    class IMesh
    {
    public:
        IMesh() = default;
        virtual ~IMesh() {}

        //----------------------------------------------------------------------
        // Destroys all buffers on the gpu. This should be called before
        // calling setVertices() when you want to dynamically generate a mesh
        // every frame with different amounts of vertices.
        //----------------------------------------------------------------------
        virtual void clear() = 0;

        //----------------------------------------------------------------------
        // Sets the vertices for this mesh. If a vertex buffer was not created,
        // it will be created to fit the amount of data given. Otherwise, the gpu
        // buffer will just be updated. Note that this is a slow operation.
        //----------------------------------------------------------------------
        virtual void setVertices(const ArrayList<Math::Vec3>& vertices) = 0;

        //----------------------------------------------------------------------
        // Set the index-buffer for this mesh. Note that this is a slow operation.
        // @Params:
        // "indices": Indices describing this submesh.
        // "subMesh": SubMesh index. Submesh must exist or index is the correct next one.
        // "topology": MeshTopology used for rendering.
        // "baseVertex": Vertex-Offset added to vertex-buffer.
        //----------------------------------------------------------------------
        virtual void setIndices(const ArrayList<U32>& indices, U32 subMesh = 0, 
                                MeshTopology topology = MeshTopology::Triangles, U32 baseVertex = 0) = 0;

        //----------------------------------------------------------------------
        // Set the color-buffer for this mesh.
        //----------------------------------------------------------------------
        virtual void setColors(const ArrayList<Color>& colors) = 0;

        //----------------------------------------------------------------------
        // Set the uv-buffer for this mesh.
        //----------------------------------------------------------------------
        virtual void setUVs(const ArrayList<Math::Vec2>& uvs) = 0;

        //----------------------------------------------------------------------
        // @Return: Buffer usage, which determines if it can be updated or not.
        //----------------------------------------------------------------------
        BufferUsage getBufferUsage()    const { return m_bufferUsage; }

        //----------------------------------------------------------------------
        // Change the buffer usage for this mesh. All existing buffers gets 
        // recreated, keep that in mind!
        //----------------------------------------------------------------------
        void        setBufferUsage(BufferUsage usage) { m_bufferUsage = usage; _RecreateBuffers(); }

        //----------------------------------------------------------------------
        const ArrayList<Math::Vec3>&    getVertices()       const { return m_vertices; }
        const ArrayList<Color>&         getColors()         const { return m_vertexColors; }
        const ArrayList<Math::Vec2>&    getUVs0()           const { return m_uvs0; }
        U32                             getVertexCount()    const { return static_cast<U32>( m_vertices.size() ); }
        U16                             getSubMeshCount()   const { return static_cast<U32>( m_subMeshes.size() ); }
        bool                            isImmutable()       const { return m_bufferUsage == BufferUsage::Immutable; }

        const ArrayList<U32>&           getIndices(U32 subMesh = 0)  const { return m_subMeshes[subMesh].indices; }
        U32                             getIndexCount(U32 subMesh)   const { return static_cast<U32>( getIndices( subMesh ).size() ); }
        IndexFormat                     getIndexFormat(U32 subMesh)  const { return m_subMeshes[subMesh].indexFormat; }
        U32                             getBaseVertex(U32 subMesh)   const { return m_subMeshes[subMesh].baseVertex; }
        bool                            hasSubMesh(U32 subMesh)      const { return subMesh < getSubMeshCount(); }
        MeshTopology                    getMeshTopology(U32 subMesh) const { return m_subMeshes[subMesh].topology; }

    protected:
        ArrayList<Math::Vec3>   m_vertices;
        ArrayList<Color>        m_vertexColors;
        ArrayList<Math::Vec2>   m_uvs0;
        BufferUsage             m_bufferUsage = BufferUsage::Immutable;

        struct SubMesh
        {
            U32                 baseVertex  = 0;
            IndexFormat         indexFormat = IndexFormat::U16;
            MeshTopology        topology    = MeshTopology::Triangles;
            ArrayList<U32>      indices;
        };
        ArrayList<SubMesh>      m_subMeshes;

        //----------------------------------------------------------------------
        // Recreate all existing buffers. Called when the buffer-usage changes.
        //----------------------------------------------------------------------
        virtual void _RecreateBuffers() = 0;

        //----------------------------------------------------------------------
        // Add a new submesh to the list of submeshes. The appropriate index-
        // format is automatically determined, based on the number of indices.
        // @Return:
        // The newly created submesh struct.
        //----------------------------------------------------------------------
        SubMesh& AddSubMesh( const ArrayList<U32>& indices, MeshTopology topology, U32 baseVertex )
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

    private:
        //----------------------------------------------------------------------
        // Binds this mesh to the pipeline. Subsequent drawcalls render this mesh.
        //----------------------------------------------------------------------
        virtual void bind(const VertexLayout& vertLayout, U32 subMesh = 0) = 0;

        //----------------------------------------------------------------------
        IMesh(const IMesh& other)               = delete;
        IMesh& operator = (const IMesh& other)  = delete;
        IMesh(IMesh&& other)                    = delete;
        IMesh& operator = (IMesh&& other)       = delete;
    };

    using Mesh = IMesh;

} // End namespaces

//**********************************************************************