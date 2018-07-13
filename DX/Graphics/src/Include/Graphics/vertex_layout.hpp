#pragma once
/**********************************************************************
    class: VertexLayout (vertex_layout.h)

    author: S. Hau
    date: December 25, 2017

    Stores information about the vertex-layout for an shader.
    The mesh class needs this to know what buffers to bind.
**********************************************************************/

namespace Graphics {

    // Description for one vertex input
    struct InputLayoutDescription
    {
        StringID name;
        U32 sizeInBytes;
        bool instanced = false;
    };

    //**********************************************************************
    class VertexLayout
    {
    public:
        VertexLayout() = default;
        ~VertexLayout() = default;

        //----------------------------------------------------------------------
        const ArrayList<InputLayoutDescription>& getLayoutDescription() const { return m_layoutDescription; }
        ArrayList<InputLayoutDescription>& getLayoutDescription() { return m_layoutDescription; }
        bool isEmpty() const { return m_layoutDescription.empty(); }

        //----------------------------------------------------------------------
        // Add a new layout description to this vertex-layout
        //----------------------------------------------------------------------
        void add(InputLayoutDescription inputDesc){ m_layoutDescription.emplace_back( inputDesc ); }

        //----------------------------------------------------------------------
        // Clear this vertex layout
        //----------------------------------------------------------------------
        void clear() { m_layoutDescription.clear(); }

    private:
        ArrayList<InputLayoutDescription> m_layoutDescription;

        NULL_COPY_AND_ASSIGN(VertexLayout)
    };

} // End namespaces