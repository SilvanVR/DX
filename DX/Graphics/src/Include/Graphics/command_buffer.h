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
        // Sorts all commands by their respective order in the type enum.
        //----------------------------------------------------------------------
        void sortCommands();

        //----------------------------------------------------------------------
        // Sort the draw commands in the most efficient way:
        //  - All drawLight() commands will come first
        //  - All drawMesh() commands are sorted first by material (less state changes)
        //    > second by camera distance if the material has possibly transparent rendercalls (determine by shaderqueue)
        //  - It assumes every draw command is subsequently
        //----------------------------------------------------------------------
        void sortDrawCommands(const Math::Vec3& camPos);

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
        void drawMesh(const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex);
        void setCamera(Camera* camera);
        void endCamera(Camera* camera);
        //void copyTexture(const TexturePtr& srcTex, const TexturePtr& dstTex);
        //void copyTexture(const TexturePtr& srcTex, I32 srcElement, I32 srcMip, const TexturePtr& dstTex, I32 dstElement, I32 dstMip);
        void drawLight(const Light* light);
        void setRenderTarget(const RenderTexturePtr& target);
        void drawFullscreenQuad(const MaterialPtr& material);
        void renderCubemap(const CubemapPtr& cubemap, const MaterialPtr& material, I32 dstMip = 0);
        void blit(const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material = nullptr);
        void setScissor(const Math::Rect& rect);
        void setCameraMatrix(StringID name, const DirectX::XMMATRIX& matrix);

    private:
        ArrayList<std::shared_ptr<GPUCommandBase>> m_gpuCommands;
    };

} // End namespaces