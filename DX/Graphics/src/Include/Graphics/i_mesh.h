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
    
    extern const StringID SID_VERTEX_POSITION;
    extern const StringID SID_VERTEX_COLOR;
    extern const StringID SID_VERTEX_UV;
    extern const StringID SID_VERTEX_NORMAL;
    extern const StringID SID_VERTEX_TANGENT;

    //**********************************************************************
    class VertexStreamBase
    {
    public:
        VertexStreamBase() = default;
        virtual ~VertexStreamBase() = default;

        virtual const void* data() const = 0;
        virtual U32         dataSize() const = 0;
        virtual U32         size() const = 0;

        bool wasUpdated() const { return m_wasUpdated; }
        void setWasUpdated(bool b) { m_wasUpdated = b; }

    private:
        bool m_wasUpdated = false;

        NULL_COPY_AND_ASSIGN(VertexStreamBase)
    };

    //**********************************************************************
    template <typename T>
    class VertexStream : public VertexStreamBase
    {
    public:
        VertexStream(U32 maxObjects) : m_data(maxObjects) {}
        VertexStream(const ArrayList<T>& data) : m_data{ data } {}

        T&          operator[] (I32 i)          { setWasUpdated(true); return m_data[i]; }
        const T&    operator[] (I32 i) const    { return m_data[i]; }

        const void* data()      const override { return m_data.data(); }
        U32         dataSize()  const override { return static_cast<U32>( m_data.size() ) * sizeof(T); }
        U32         size()      const override { return static_cast<U32>( m_data.size() ); }

        // Allows for-each loop
        using iterator = typename std::vector<T>::iterator;
        iterator begin()    { setWasUpdated(true); return m_data.begin(); }
        iterator end()      { setWasUpdated(true); return m_data.end(); }

        const ArrayList<T>& getList() const { return m_data; }

    private:
        ArrayList<T> m_data;
    };

    using VertexStreamPtr = std::shared_ptr<VertexStreamBase>;

    //**********************************************************************
    class IMesh
    {
    public:
        IMesh() = default;
        virtual ~IMesh() {}

        //----------------------------------------------------------------------
        // Destroys all buffers on the gpu. This can be called every frame, but
        // is very expensive. Note that all buffers dynamically grow, but to free
        // up space its useful to call this function occasionally.
        //----------------------------------------------------------------------
        void clear();

        //----------------------------------------------------------------------
        // Add a new vertex stream. This destroys the old buffer and creates a
        // new one, so use this function only once and update per stream object if needed.
        // @Params:
        //  "name": The name of the vertex stream. Must be equal to the name in the shader.
        //  "vs": The vertex-stream object over which vertex-data can be updated efficiently.
        //----------------------------------------------------------------------
        void setVertexStream(StringID name, const VertexStreamPtr& vs);

        //----------------------------------------------------------------------
        // Sets the vertices for this mesh. If a vertex buffer was not created,
        // it will be created to fit the amount of data given. Otherwise, the gpu
        // buffer will just be updated (and grows if necessary). Note that this is a slow operation.
        //----------------------------------------------------------------------
        void setVertices(const ArrayList<Math::Vec3>& vertices);

        //----------------------------------------------------------------------
        // Set the index-buffer for this mesh. The buffer grows if necessary. Note that this is a slow operation.
        // @Params:
        // "indices": Indices describing this submesh.
        // "subMesh": SubMesh index. Submesh must exist or index is the correct next one.
        // "topology": MeshTopology used for rendering.
        // "baseVertex": Vertex-Offset added to vertex-buffer.
        //----------------------------------------------------------------------
        void setIndices(const ArrayList<U32>& indices, U32 subMesh = 0, 
                        MeshTopology topology = MeshTopology::Triangles, U32 baseVertex = 0);

        //----------------------------------------------------------------------
        // Set the color-buffer for this mesh. The buffer grows if necessary.
        //----------------------------------------------------------------------
        void setColors(const ArrayList<Color>& colors);

        //----------------------------------------------------------------------
        // Set the uv-buffer for this mesh. The buffer grows if necessary.
        //----------------------------------------------------------------------
        void setUVs(const ArrayList<Math::Vec2>& uvs);

        //----------------------------------------------------------------------
        // Sets the normals for this mesh. The buffer grows if necessary.
        //----------------------------------------------------------------------
        void setNormals(const ArrayList<Math::Vec3>& normals);

        //----------------------------------------------------------------------
        // Sets the tangents for this mesh. The buffer grows if necessary.
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
        void setBufferUsage(BufferUsage usage) { m_bufferUsage = usage; _RecreateBuffers(); }

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
        U16                             getSubMeshCount()   const { return static_cast<U32>( m_subMeshes.size() ); }
        bool                            isImmutable()       const { return m_bufferUsage == BufferUsage::Immutable; }

        U32                             getVertexCount()             const { return (U32)getVertexPositions().size(); }
        const ArrayList<U32>&           getIndices(U32 subMesh = 0)  const { return m_subMeshes[subMesh].indices; }
        U32                             getIndexCount(U32 subMesh)   const { return m_subMeshes[subMesh].indexCount; }
        IndexFormat                     getIndexFormat(U32 subMesh)  const { return m_subMeshes[subMesh].indexFormat; }
        U32                             getBaseVertex(U32 subMesh)   const { return m_subMeshes[subMesh].baseVertex; }
        bool                            hasSubMesh(U32 subMesh)      const { return subMesh < getSubMeshCount(); }
        MeshTopology                    getMeshTopology(U32 subMesh) const { return m_subMeshes[subMesh].topology; }
        const Math::AABB&               getBounds()                  const { return m_bounds; }
        const ArrayList<Math::Vec3>&    getVertexPositions() const;
        const ArrayList<Math::Vec2>&    getUVs() const;
        const ArrayList<Math::Vec3>&    getNormals() const;
        const ArrayList<Math::Vec4>&    getTangents() const;
        bool                            hasVertexStream(StringID name) const { return m_vertexStreams.find(name) != m_vertexStreams.end(); }

        template<typename T>
        std::shared_ptr<VertexStream<T>> getVertexStream(StringID name) { return std::dynamic_pointer_cast<VertexStream<T>>( m_vertexStreams[name] ); }

    protected:
        HashMap<StringID, VertexStreamPtr>  m_vertexStreams;
        BufferUsage                         m_bufferUsage = BufferUsage::Immutable;
        Math::AABB                          m_bounds;

        struct SubMesh
        {
            U32                 baseVertex  = 0;
            IndexFormat         indexFormat = IndexFormat::U16;
            MeshTopology        topology    = MeshTopology::Triangles;
            ArrayList<U32>      indices;
            U32                 indexCount; // Real index-count. "Indices" can contain more indices e.g. when the submesh index-buffer was updated with less indices than before
        };
        ArrayList<SubMesh>      m_subMeshes;

        //----------------------------------------------------------------------
        // Recreate all existing buffers. Called when the buffer-usage changes.
        //----------------------------------------------------------------------
        virtual void _RecreateBuffers() = 0;

        //----------------------------------------------------------------------
        // Overrides for an API dependant mesh class
        //----------------------------------------------------------------------
        virtual void _Clear() = 0;

        virtual void _CreateBuffer(StringID name, const VertexStreamPtr& vs) = 0;
        virtual void _DestroyBuffer(StringID name) = 0;

        virtual void _CreateIndexBuffer(const SubMesh& subMesh, I32 index) = 0;
        virtual void _DestroyIndexBuffer(I32 index) = 0;

        // Buffer updates are queued. This way its safe to update buffers on different threads.
        std::queue<U32> m_queuedIndexBufferUpdates;

    private:
        //----------------------------------------------------------------------
        // Binds this mesh to the pipeline. Subsequent drawcalls render this mesh.
        //----------------------------------------------------------------------
        friend class D3D11Renderer;
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
        void _RecalculateBounds(const ArrayList<Math::Vec3>& vertexPositions);

        NULL_COPY_AND_ASSIGN(IMesh)
    };

    using Mesh = IMesh;

} // End namespaces

using MeshPtr = std::shared_ptr<Graphics::IMesh>;