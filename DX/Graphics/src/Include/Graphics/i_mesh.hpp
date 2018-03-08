#pragma once
/**********************************************************************
    class: Mesh (mesh.h)

    author: S. Hau
    date: March 8, 2018

    Interface for a Mesh class. A Mesh Represents one contiguous Mesh,
    which can be rendered with a material.
**********************************************************************/

namespace Graphics {

    //----------------------------------------------------------------------
    enum class EMeshType
    {
        MESH,
        INDEXED
    };

    //**********************************************************************
    class Mesh
    {
    public:
        Mesh(EMeshType type = EMeshType::MESH) : m_type( type ) {}
        virtual ~Mesh() {}

        //----------------------------------------------------------------------
        // Binds this mesh to the pipeline. Subsequent drawcalls render this mesh.
        //----------------------------------------------------------------------
        virtual void bind() = 0;

        //----------------------------------------------------------------------
        EMeshType getMeshType() const { return m_type; }

    private:
        EMeshType m_type = EMeshType::MESH;

        //----------------------------------------------------------------------
        Mesh(const Mesh& other)               = delete;
        Mesh& operator = (const Mesh& other)  = delete;
        Mesh(Mesh&& other)                    = delete;
        Mesh& operator = (Mesh&& other)       = delete;
    };

    //**********************************************************************
    class IndexedMesh : public Mesh
    {
    public:
        IndexedMesh(U32 numIndices) : Mesh( EMeshType::INDEXED ), m_numIndices( numIndices ) {}
        virtual ~IndexedMesh() {}

        //----------------------------------------------------------------------
        U32 numIndices() const { return m_numIndices; }

    private:
        U32 m_numIndices = 0;

        //----------------------------------------------------------------------
        IndexedMesh(const IndexedMesh& other)               = delete;
        IndexedMesh& operator = (const IndexedMesh& other)  = delete;
        IndexedMesh(IndexedMesh&& other)                    = delete;
        IndexedMesh& operator = (IndexedMesh&& other)       = delete;
    };

} // End namespaces