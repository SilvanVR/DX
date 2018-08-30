#pragma once
/**********************************************************************
    class: Cubemap

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_cubemap.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Cubemap : public ICubemap
    {
    public:
        Cubemap() = default;
        ~Cubemap() = default;

        //----------------------------------------------------------------------
        // ICubemap Interface
        //----------------------------------------------------------------------
        void create(I32 size, TextureFormat format, Mips mips) override;
        void apply(bool updateMips, bool keepPixelsInRAM) override;

    private:
        bool m_generateMips;
        bool m_keepPixelsInRAM;
        bool m_gpuUpToDate = false;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override {}
        void bind(const ShaderResourceDeclaration& res) override {}

        //----------------------------------------------------------------------
        void _CreateTexture(Mips mips);
        void _CreateShaderResourceView();
        void _PushToGPU();

        NULL_COPY_AND_ASSIGN(Cubemap)
    };

} } // End namespaces