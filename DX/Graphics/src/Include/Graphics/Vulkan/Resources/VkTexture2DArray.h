#pragma once
/**********************************************************************
    class: Texture2DArray

    author: S. Hau
    date: August 20, 2018
**********************************************************************/

#include "i_texture2d_array.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Texture2DArray : public Graphics::ITexture2DArray
    {
    public:
        Texture2DArray() = default;
        ~Texture2DArray() = default;

        //----------------------------------------------------------------------
        // ITexture2DArray Interface
        //----------------------------------------------------------------------
        void create(U32 width, U32 height, U32 depth, TextureFormat format, bool generateMips) override;
        void apply(bool updateMips, bool keepPixelsInRAM) override;

    private:
        bool m_generateMips;
        bool m_keepPixelsInRAM;
        bool m_gpuUpToDate = false;

        //----------------------------------------------------------------------
        // ITexture Interface
        //----------------------------------------------------------------------
        void _UpdateSampler() override {}
        void bind(ShaderType shaderType, U32 bindSlot) override {}

        //----------------------------------------------------------------------
        void _CreateTextureArray();
        void _CreateShaderResourveView();

        //----------------------------------------------------------------------
        void _PushToGPU();

        NULL_COPY_AND_ASSIGN(Texture2DArray)
    };

} } // End namespaces