#pragma once
/**********************************************************************
    class: VertexShader

    author: S. Hau
    date: August 17, 2018
**********************************************************************/

#include "VkShaderBase.h"
#include "vertex_layout.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class VertexShader : public ShaderBase
    {
    public:
        VertexShader() : ShaderBase(ShaderType::Vertex) {}
        ~VertexShader() = default;

        //----------------------------------------------------------------------
        const VertexLayout& getVertexLayout() const { return m_vertexLayout; }

    private:
        VertexLayout m_vertexLayout;

        //----------------------------------------------------------------------
        virtual void _ShaderReflection(const ArrayList<uint32_t>& spv) override;

        NULL_COPY_AND_ASSIGN(VertexShader)
    };

} } // End namespaces