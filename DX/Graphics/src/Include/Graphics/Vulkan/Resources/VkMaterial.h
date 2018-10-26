#pragma once
/**********************************************************************
    class: Material

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_material.h"
#include "../Pipeline/VkMappedUniformBuffer.h"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Material : public IMaterial
    {
    public:
        Material() = default;
        ~Material() { _DestroyUniformBuffers(); }

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------
        void _SetInt(StringID name, I32 val)                            override { _UpdateUniformBuffer(name, &val); }
        void _SetFloat(StringID name, F32 val)                          override { _UpdateUniformBuffer(name, &val); }
        void _SetVec4(StringID name, const Math::Vec4& vec)             override { _UpdateUniformBuffer(name, &vec); }
        void _SetMatrix(StringID name, const DirectX::XMMATRIX& matrix) override { _UpdateUniformBuffer(name, &matrix); }
        void _SetData(StringID name, const void* data)                  override { _UpdateUniformBuffer(name, data); };

    private:
        // Contains the material data in a contiguous block of memory. Will be empty if not used for a shader.
        MappedUniformBuffer* m_materialDataVS = nullptr;
        MappedUniformBuffer* m_materialDataFS = nullptr;
        MappedUniformBuffer* m_materialDataGS = nullptr;

        //----------------------------------------------------------------------
        // IMaterial Interface
        //----------------------------------------------------------------------
        void bind() override;
        void _ChangedShader() override;

        //----------------------------------------------------------------------
        void _DestroyUniformBuffers();
        void _CreateUniformBuffers();
        void _UpdateUniformBuffer(StringID name, const void* pData);

        NULL_COPY_AND_ASSIGN(Material)
    };

} } // End namespaces