#pragma once
/**********************************************************************
    class: Mesh

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_mesh.h"
#include "Vulkan/Vulkan.hpp"
#include "../Pipeline/VkBuffer.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Mesh : public IMesh
    {
    public:
        Mesh() = default;
        ~Mesh() { _Clear(); }

    private:
        HashMap<StringID, RingBuffer*>  m_vertexBuffers;
        ArrayList<RingBuffer*>          m_indexBuffers;

        //----------------------------------------------------------------------
        // IMesh Interface
        //----------------------------------------------------------------------
        void _Clear() override;
        void _CreateIndexBuffer(const SubMesh& subMesh, I32 index) override;
        void _DestroyIndexBuffer(I32 index) override;

        void _CreateBuffer(StringID name, const VertexStreamBase& vs) override;
        void _DestroyBuffer(StringID name) override;

        //----------------------------------------------------------------------
        // IMesh Interface
        //----------------------------------------------------------------------
        void bind(const VertexLayout& vertLayout, U32 subMesh = 0) override;
        void _RecreateBuffers();

        //----------------------------------------------------------------------
        inline void _SetTopology(U32 subMesh);
        inline void _BindVertexBuffer(const VertexLayout& vertLayout);
        inline void _BindIndexBuffer(U32 subMesh);
        inline void _UpdateIndexBuffer(U32 index);

        NULL_COPY_AND_ASSIGN(Mesh)
    };


} } // End namespaces