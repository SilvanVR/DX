#pragma once
/**********************************************************************
    class: CommandBuffer (command_buffer.h)

    author: S. Hau
    date: December 19, 2017

    - Consists of arbitrary GPU commands
    - Can be passed to the renderer, who transform these calls to api
      dependant calls (and possibly do optimizations e.g. batch stuff)
**********************************************************************/

#include "gpu_commands.hpp"

namespace Graphics {

    #define COMMAND_BUFFER_INITIAL_CAPACITY 128

    //**********************************************************************
    class CommandBuffer
    {
    public:
        CommandBuffer();
        ~CommandBuffer() = default;

        //----------------------------------------------------------------------
        // Add all commands from the given cmd into this one
        //----------------------------------------------------------------------
        void merge(const CommandBuffer& cmd);

        //----------------------------------------------------------------------
        // Clears all commands in this command buffer.
        //----------------------------------------------------------------------
        void reset();

        // <------------------------ GPU COMMANDS ----------------------------->
        const ArrayList<std::shared_ptr<GPUCommandBase>>& getGPUCommands() const { return m_gpuCommands; }
        ArrayList<std::shared_ptr<GPUCommandBase>>& getGPUCommands() { return m_gpuCommands; }
        void drawMesh(MeshPtr mesh, MaterialPtr material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex);
        void setRenderTarget(RenderTexturePtr renderTarget);
        void clearRenderTarget(const Color& clearColor);
        void setCameraPerspective(const DirectX::XMMATRIX& view, F32 fov, F32 zNear, F32 zFar);
        void setCameraOrtho(const DirectX::XMMATRIX& view, F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        void setViewport(const Graphics::ViewportRect& viewport);
        void copyTexture(ITexture* srcTex, ITexture* dstTex);
        void copyTexture(ITexture* srcTex, I32 srcElement, I32 srcMip, ITexture* dstTex, I32 dstElement, I32 dstMip);
        void setGlobalFloat(StringID name, F32 value);
        void setGlobalInt(StringID name, I32 value);
        void setGlobalVector4(StringID name, const Math::Vec4& vec4);
        void setGlobalColor(StringID name, Color color);
        void setGlobalMatrix(StringID name, const DirectX::XMMATRIX& matrix);

    private:
        ArrayList<std::shared_ptr<GPUCommandBase>> m_gpuCommands;

        //----------------------------------------------------------------------
        CommandBuffer(const CommandBuffer& other)               = delete;
        CommandBuffer& operator = (const CommandBuffer& other)  = delete;
        CommandBuffer(CommandBuffer&& other)                    = delete;
        CommandBuffer& operator = (CommandBuffer&& other)       = delete;
    };

} // End namespaces