#pragma once
/**********************************************************************
    class: VertexLayout (vertex_layout.h)

    author: S. Hau
    date: December 25, 2017
**********************************************************************/

namespace Graphics {

    //**********************************************************************
    class VertexLayout
    {
    public:
        VertexLayout() = default;
        ~VertexLayout() = default;

    private:

        //----------------------------------------------------------------------
        VertexLayout(const VertexLayout& other)               = delete;
        VertexLayout& operator = (const VertexLayout& other)  = delete;
        VertexLayout(VertexLayout&& other)                    = delete;
        VertexLayout& operator = (VertexLayout&& other)       = delete;
    };

} // End namespaces