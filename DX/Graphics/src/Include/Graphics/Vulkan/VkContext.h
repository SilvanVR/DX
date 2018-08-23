#pragma once
/**********************************************************************
    class: Context

    author: S. Hau
    date: August 22, 2018
**********************************************************************/

#include "VkPlatform.h"
#include "../forward_declarations.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    class Context
    {
        static const I32 NUM_FRAME_DATA = 2;
    public:
        Context() = default;

        void setClearColor(Color color){ m_clearColor = color; }
        void setClearDepthStencil(F32 depth, U8 stencil){ m_depth = depth; m_stencil = stencil; }
        void OMSetRenderTarget(VkImageView color, VkImageView depth = VK_NULL_HANDLE){ m_colorView = color; m_depthView = depth; }

    private:
        Color   m_clearColor;
        F32     m_depth;
        U8      m_stencil;

        VkImageView m_colorView = VK_NULL_HANDLE;
        VkImageView m_depthView = VK_NULL_HANDLE;

        struct FrameData
        {
            VkFence         fence; // Signaled when this struct is ready to be reused
            VkSemaphore     semPresentComplete;
            VkSemaphore     semRenderingFinished;
            VkCommandBuffer cmd;
        };
        std::array<FrameData, NUM_FRAME_DATA> m_frameData;

        friend class VkRenderer;
        void Init();
        void Shutdown();

        NULL_COPY_AND_ASSIGN(Context)
    };

} } // End namespaces