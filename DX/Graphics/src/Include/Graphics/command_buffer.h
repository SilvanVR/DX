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
#include "Memory/Allocators/stack_allocator.h"

namespace Graphics {

    //**********************************************************************
    class CommandBuffer
    {
        static const U32 COMMAND_BUFFER_INITIAL_CAPACITY = 64;
        static const U32 INITIAL_STORAGE_SIZE = 1024 * 1024;

    public:
        CommandBuffer();
        ~CommandBuffer();
        CommandBuffer(const CommandBuffer& other);
        CommandBuffer(CommandBuffer&& other);

        //----------------------------------------------------------------------
        bool isEmpty()     const { return sizeInBytes() == 0; }
        Size sizeInBytes() const { return m_storage.size(); }

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
        const ArrayList<GPUCommandBase*>& getGPUCommands() const { return m_gpuCommands; }
        void drawMesh(const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex);
        void drawMeshInstanced(const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 instanceCount);
        void drawMeshSkinned(const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex, const ArrayList<DirectX::XMMATRIX>& matrixPalette);
        void setCamera(const Camera& camera);
        void endCamera();
        void copyTexture(const TexturePtr& srcTex, const TexturePtr& dstTex);
        void copyTexture(const TexturePtr& srcTex, I32 srcElement, I32 srcMip, const TexturePtr& dstTex, I32 dstElement, I32 dstMip);
        void drawLight(const Light* light);
        void setRenderTarget(const RenderTexturePtr& target);
        void drawFullscreenQuad(const MaterialPtr& material);
        void renderCubemap(const CubemapPtr& cubemap, const MaterialPtr& material, I32 dstMip = 0);
        void blit(const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material = nullptr);
        void setScissor(const Math::Rect& rect);
        void setCameraMatrix(CameraMember member, const DirectX::XMMATRIX& matrix);
        void beginTimeQuery(StringID name);
        void endTimeQuery(StringID name);

    private:
        ArrayList<Byte> m_storage;
        ArrayList<GPUCommandBase*> m_gpuCommands;

        template <typename T>
        inline void* _GetStorageDestination() { return _GetStorageDestination(sizeof(T), alignof(T)); }
        inline void* _GetStorageDestination(Size size, Size alignment);
    };

} // End namespaces