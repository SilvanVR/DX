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
    // Base class for different vertex streams.
    //**********************************************************************
    class VertexStreamBase
    {
    public:
        VertexStreamBase() = default;
        virtual ~VertexStreamBase() = default;

        //----------------------------------------------------------------------
        virtual const void* data() const = 0;
        virtual U32         bufferSize() const = 0;
        virtual U32         size() const = 0;

        //----------------------------------------------------------------------
        // @Return: Whether this vertex-stream was updated.
        //----------------------------------------------------------------------
        bool wasUpdated() { bool wasUpdated = m_wasUpdated; m_wasUpdated = false; return wasUpdated; }

    protected:
        void _SetWasUpdated() { m_wasUpdated = true; }

    private:
        bool m_wasUpdated = false;

        NULL_COPY_AND_ASSIGN(VertexStreamBase)
    };

    //**********************************************************************
    // Class for vertex-streams of different types for efficent updating.
    //**********************************************************************
    template <typename T>
    class VertexStream : public VertexStreamBase
    {
    public:
        VertexStream(U32 maxObjects) : m_data(maxObjects) {}
        VertexStream(const ArrayList<T>& data) : m_data{ data } {}

        T&          operator[] (I32 i)          { _SetWasUpdated(); return m_data[i]; }
        const T&    operator[] (I32 i) const    { return m_data[i]; }

        //----------------------------------------------------------------------
        // Returns the underlying data-container.
        //----------------------------------------------------------------------
        const ArrayList<T>& get() const { return m_data; }

        //----------------------------------------------------------------------
        // Resizes the underlying data container to fit the given amount.
        //----------------------------------------------------------------------
        void resize(U32 newSize) { m_data.resize(newSize); }

        // Allows for-each loop
        using iterator = typename std::vector<T>::iterator;
        iterator begin()    { _SetWasUpdated(); return m_data.begin(); }
        iterator end()      { _SetWasUpdated(); return m_data.end(); }

        //----------------------------------------------------------------------
        // VertexStreamBase Interface
        //----------------------------------------------------------------------
        const void* data()      const override { return m_data.data(); }
        U32         bufferSize()const override { return static_cast<U32>( m_data.size() ) * sizeof(T); }
        U32         size()      const override { return static_cast<U32>( m_data.size() ); }

    private:
        ArrayList<T> m_data;

        NULL_COPY_AND_ASSIGN(VertexStream)
    };


    //**********************************************************************
    class IMesh
    {
    public:
        IMesh() = default;
        virtual ~IMesh();

        //----------------------------------------------------------------------
        // Destroys all buffers on the gpu. This can be called every frame, but is very expensive.
        //----------------------------------------------------------------------
        void clear();

        //----------------------------------------------------------------------
        // Sets the vertices for this mesh. Note that this is a slow operation.
        //----------------------------------------------------------------------
        VertexStream<Math::Vec3>& setVertices(const ArrayList<Math::Vec3>& vertices);

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
        // Set the color-buffer for this mesh.
        //----------------------------------------------------------------------
        VertexStream<Math::Vec4>& setColors(const ArrayList<Color>& colors);

        //----------------------------------------------------------------------
        // Set the uv-buffer for this mesh.
        //----------------------------------------------------------------------
        VertexStream<Math::Vec2>& setUVs(const ArrayList<Math::Vec2>& uvs);

        //----------------------------------------------------------------------
        // Sets the normals for this mesh.
        //----------------------------------------------------------------------
        VertexStream<Math::Vec3>& setNormals(const ArrayList<Math::Vec3>& normals);

        //----------------------------------------------------------------------
        // Sets the tangents for this mesh.
        //----------------------------------------------------------------------
        VertexStream<Math::Vec4>& setTangents(const ArrayList<Math::Vec4>& tangents);

        //----------------------------------------------------------------------
        // @Return: Buffer usage, which determines if it can be updated or not.
        //----------------------------------------------------------------------
        BufferUsage getBufferUsage() const { return m_bufferUsage; }

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
        // Creates a new vertex stream, returns a reference to it and deletes the old one if present.
        // @Params:
        //  "name": The name of the vertex stream
        //  "maxObjects": The maximum amount of objects for this vertex stream.
        //----------------------------------------------------------------------
        template<typename T>
        VertexStream<T>& createVertexStream(StringID name, U32 maxObjects = 1)
        {
            auto vs = new VertexStream<T>(maxObjects < 1 ? 1 : maxObjects);
            _SetVertexStream(name, vs);
            return *vs;
        }

        //----------------------------------------------------------------------
        // Creates a new vertex stream and returns a reference to it.
        // @Params:
        //  "name": The name of the vertex stream
        //  "data": The initial data for this vertex stream.
        //----------------------------------------------------------------------
        template<typename T>
        VertexStream<T>& createVertexStream(StringID name, const ArrayList<T>& data)
        {
            auto vs = new VertexStream<T>(data);
            _SetVertexStream(name, vs);
            return *vs;
        }

        //----------------------------------------------------------------------
        // @Return: Vertex-Stream with the given name. Nullptr if not present.
        //----------------------------------------------------------------------
        template<typename T>
        VertexStream<T>& getVertexStream(StringID name)
        {
            auto stream = dynamic_cast<VertexStream<T>*>(m_vertexStreams[name]);
            ASSERT( stream && "Stream does not exist. This should never happen!" );
            return *stream; 
        }

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
        VertexStream<Math::Vec3>&       getPositionStream() { return getVertexStream<Math::Vec3>(SID_VERTEX_POSITION); }
        VertexStream<Math::Vec4>&       getColorStream()    { return getVertexStream<Math::Vec4>(SID_VERTEX_COLOR); }
        VertexStream<Math::Vec2>&       getUVStream()       { return getVertexStream<Math::Vec2>(SID_VERTEX_UV); }
        VertexStream<Math::Vec3>&       getNormalStream()   { return getVertexStream<Math::Vec3>(SID_VERTEX_NORMAL); }
        VertexStream<Math::Vec4>&       getTangentStream()  { return getVertexStream<Math::Vec4>(SID_VERTEX_TANGENT); }


    protected:
        HashMap<StringID, VertexStreamBase*>    m_vertexStreams;
        BufferUsage                             m_bufferUsage = BufferUsage::Immutable;
        Math::AABB                              m_bounds;

        struct SubMesh
        {
            U32                 baseVertex  = 0;
            IndexFormat         indexFormat = IndexFormat::U16;
            MeshTopology        topology    = MeshTopology::Triangles;
            ArrayList<U32>      indices;
            U32                 indexCount; // Real index-count. "Indices" can contain more indices e.g. when the submesh index-buffer was updated with less indices than before
        };
        ArrayList<SubMesh>      m_subMeshes;

        // Buffer updates are queued. This way its safe to update buffers on different threads.
        std::queue<U32> m_queuedIndexBufferUpdates;

        //----------------------------------------------------------------------
        // Recreate all existing buffers. Called when the buffer-usage changes.
        //----------------------------------------------------------------------
        virtual void _RecreateBuffers() = 0;

        //----------------------------------------------------------------------
        // Overrides for an API dependant mesh class
        //----------------------------------------------------------------------
        virtual void _Clear() = 0;

        virtual void _CreateBuffer(StringID name, const VertexStreamBase& vs) = 0;
        virtual void _DestroyBuffer(StringID name) = 0;

        virtual void _CreateIndexBuffer(const SubMesh& subMesh, I32 index) = 0;
        virtual void _DestroyIndexBuffer(I32 index) = 0;

    private:
        void _SetVertexStream(StringID name, VertexStreamBase* vs);

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