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
        void reset();

        // <------------------------ GPU COMMANDS ----------------------------->
        const ArrayList<std::unique_ptr<GPUCommandBase>>& getGPUCommands() const { return m_gpuCommands; }
        void drawMesh(Mesh* mesh, Material* material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex);
        void setRenderTarget(Graphics::RenderTexture* renderTarget);
        void clearRenderTarget(const Color& clearColor);
        void setCameraPerspective(const DirectX::XMMATRIX& view, F32 fov, F32 zNear, F32 zFar);
        void setCameraOrtho(const DirectX::XMMATRIX& view, F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);
        void setViewport(const Graphics::ViewportRect& viewport);

    private:
        ArrayList<std::unique_ptr<GPUCommandBase>> m_gpuCommands;

        //----------------------------------------------------------------------
        CommandBuffer(const CommandBuffer& other)               = delete;
        CommandBuffer& operator = (const CommandBuffer& other)  = delete;
        CommandBuffer(CommandBuffer&& other)                    = delete;
        CommandBuffer& operator = (CommandBuffer&& other)       = delete;
    };

} // End namespaces