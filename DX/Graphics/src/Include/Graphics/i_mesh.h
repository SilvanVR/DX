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
#include "Math/aabb.h"

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
        void clear();

        //----------------------------------------------------------------------
        // Sets the vertices for this mesh. If a vertex buffer was not created,
        // it will be created to fit the amount of data given. Otherwise, the gpu
        // buffer will just be updated. Note that this is a slow operation.
        //----------------------------------------------------------------------
        void setVertices(const ArrayList<Math::Vec3>& vertices);

        //----------------------------------------------------------------------
        // Set the index-buffer for this mesh. Note that this is a slow operation.
        // @Params:
        // "indices": Indices describing this submesh.
        // "subMesh": SubMesh index. Submesh must exist or index is the correct next one.
        // "topology": MeshTopology used for rendering.
        // "baseVertex": Vertex-Offset added to vertex-buffer.
        //----------------------------------------------------------------------
        void setIndices(const ArrayList<U32>& indices, U32 subMesh = 0, 
                        MeshTopology topology = MeshTopology::Triangles, U32 baseVertex = 0);

        //----------------------------------------------------------------------
        // Set the color-buffer for this mesh.
        //----------------------------------------------------------------------
        void setColors(const ArrayList<Color>& colors);

        //----------------------------------------------------------------------
        // Set the uv-buffer for this mesh.
        //----------------------------------------------------------------------
        void setUVs(const ArrayList<Math::Vec2>& uvs);

        //----------------------------------------------------------------------
        // Sets the normals for this mesh. If a normal buffer was not created,
        // it will be created to fit the amount of data given. Otherwise, the gpu
        // buffer will just be updated. Note that this is a slow operation.
        //----------------------------------------------------------------------
        void setNormals(const ArrayList<Math::Vec3>& normals);

        //----------------------------------------------------------------------
        // Sets the tangents for this mesh. If a buffer was not created,
        // it will be created to fit the amount of data given. Otherwise, the gpu
        // buffer will just be updated. Note that this is a slow operation.
        //----------------------------------------------------------------------
        void setTangents(const ArrayList<Math::Vec4>& tangents);

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
        // Recalculates the normals from the vertices
        //----------------------------------------------------------------------
        void recalculateNormals();

        //----------------------------------------------------------------------
        // Recalculates the tangents from the vertices and uvs
        // @Params:
        //  "invertBinormal": If set to true the w component of the tangent will be set to -1 instead of 1.
        //----------------------------------------------------------------------
        void recalculateTangents(bool invertBinormal = false);

        //----------------------------------------------------------------------
        // Set the mesh bounding box manually. Note that setVertices() override the bounds,
        // so this call makes only sense if you want to have a custom bounding box after the vertex data are set.
        //----------------------------------------------------------------------
        void setBounds(const Math::AABB& bounds) { m_bounds = bounds; }

        //----------------------------------------------------------------------
        const ArrayList<Math::Vec3>&    getVertices()       const { return m_vertices; }
        const ArrayList<Color>&         getColors()         const { return m_colors; }
        const ArrayList<Math::Vec2>&    getUVs0()           const { return m_uvs0; }
        const ArrayList<Math::Vec3>&    getNormals()        const { return m_normals; }
        const ArrayList<Math::Vec4>&    getTangents()       const { return m_tangents; }
        U32                             getVertexCount()    const { return static_cast<U32>( m_vertices.size() ); }
        U16                             getSubMeshCount()   const { return static_cast<U32>( m_subMeshes.size() ); }
        bool                            isImmutable()       const { return m_bufferUsage == BufferUsage::Immutable; }

        const ArrayList<U32>&           getIndices(U32 subMesh = 0)  const { return m_subMeshes[subMesh].indices; }
        U32                             getIndexCount(U32 subMesh)   const { return static_cast<U32>( getIndices( subMesh ).size() ); }
        IndexFormat                     getIndexFormat(U32 subMesh)  const { return m_subMeshes[subMesh].indexFormat; }
        U32                             getBaseVertex(U32 subMesh)   const { return m_subMeshes[subMesh].baseVertex; }
        bool                            hasSubMesh(U32 subMesh)      const { return subMesh < getSubMeshCount(); }
        MeshTopology                    getMeshTopology(U32 subMesh) const { return m_subMeshes[subMesh].topology; }
        const Math::AABB&               getBounds()                  const { return m_bounds; }

    protected:
        ArrayList<Math::Vec3>   m_vertices;
        ArrayList<Color>        m_colors;
        ArrayList<Math::Vec2>   m_uvs0;
        ArrayList<Math::Vec3>   m_normals;
        ArrayList<Math::Vec4>   m_tangents;
        BufferUsage             m_bufferUsage = BufferUsage::Immutable;
        Math::AABB              m_bounds;

        struct SubMesh
        {
            U32                 baseVertex  = 0;
            IndexFormat         indexFormat = IndexFormat::U16;
            MeshTopology        topology    = MeshTopology::Triangles;
            ArrayList<U32>      indices;
        };
        ArrayList<SubMesh>      m_subMeshes;

        // Buffer updates are queued. This way its safe to update buffers on different threads.
        enum class MeshBufferType
        {
            Vertex,
            Color,
            TexCoord,
            Normal,
            Index,
            Tangent
        };
        struct BufferUpdateInformation
        {
            MeshBufferType  type;
            U32             index = 0; // Only used for some types e.g. submesh index for index-buffer updates
        };
        std::queue<BufferUpdateInformation> m_queuedBufferUpdates;

        //----------------------------------------------------------------------
        // Recreate all existing buffers. Called when the buffer-usage changes.
        //----------------------------------------------------------------------
        virtual void _RecreateBuffers() = 0;

        //----------------------------------------------------------------------
        // Overrides for an API dependant mesh class
        //----------------------------------------------------------------------
        virtual void _Clear() = 0;
        virtual void _CreateVertexBuffer(const ArrayList<Math::Vec3>& vertices) = 0;
        virtual void _CreateIndexBuffer(const SubMesh& subMesh, I32 index) = 0;
        virtual void _CreateColorBuffer(const ArrayList<Color>& colors) = 0;
        virtual void _CreateUVBuffer(const ArrayList<Math::Vec2>& uvs) = 0;
        virtual void _CreateNormalBuffer(const ArrayList<Math::Vec3>& normals) = 0;
        virtual void _CreateTangentBuffer(const ArrayList<Math::Vec4>& tangents) = 0;

    private:
        //----------------------------------------------------------------------
        // Binds this mesh to the pipeline. Subsequent drawcalls render this mesh.
        //----------------------------------------------------------------------
        virtual void bind(const VertexLayout& vertLayout, U32 subMesh = 0) = 0;

        //----------------------------------------------------------------------
        // Add a new submesh to the list of submeshes. The appropriate index-
        // format is automatically determined, based on the number of indices.
        // @Return:
        // The newly created submesh struct.
        //----------------------------------------------------------------------
        SubMesh& _AddSubMesh( const ArrayList<U32>& indices, MeshTopology topology, U32 baseVertex );

        //----------------------------------------------------------------------
        // Recalculate the AABB for this mesh
        //----------------------------------------------------------------------
        void _RecalculateBounds();

        //----------------------------------------------------------------------
        IMesh(const IMesh& other)               = delete;
        IMesh& operator = (const IMesh& other)  = delete;
        IMesh(IMesh&& other)                    = delete;
        IMesh& operator = (IMesh&& other)       = delete;
    };

    using Mesh = IMesh;

} // End namespaces

using MeshPtr = std::shared_ptr<Graphics::IMesh>;