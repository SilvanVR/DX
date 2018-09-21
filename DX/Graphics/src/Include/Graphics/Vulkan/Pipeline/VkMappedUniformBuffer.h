#pragma once
/**********************************************************************
    class: MappedUniformBuffer

    author: S. Hau
    date: August 29, 2018
**********************************************************************/

#include "VkBuffer.h"
#include "../../shader_resources.hpp"

namespace Graphics { namespace Vulkan {

    //**********************************************************************
    // This class encapsulates a uniform buffer from a shader buffer description,
    // so you can update properties by name and flush the whole data in one go to the gpu.
    //**********************************************************************
    class MappedUniformBuffer
    {
    public:
        MappedUniformBuffer(const ShaderUniformBufferDeclaration& bufferInfo, BufferUsage usage);
        ~MappedUniformBuffer() { _FreeBuffers(); }

        //----------------------------------------------------------------------
        bool                                  gpuIsUpToDate() const { return m_gpuUpToDate; }
        const ShaderUniformBufferDeclaration& getBufferInfo() const { return m_bufferInfo; }

        //----------------------------------------------------------------------
        // Update the given uniform 'name' with the given data. !! ONLY ON THE CPU !!
        // @Return:
        //  False if uniform with the given name does not exist.
        //----------------------------------------------------------------------
        bool update(StringID name, const void* data);

        //----------------------------------------------------------------------
        // This updates the gpu uniform buffer directly with the given raw data.
        //----------------------------------------------------------------------
        void update(const void* data, U32 sizeInBytes);

        //----------------------------------------------------------------------
        // Sends the whole buffer on the cpu to the gpu. (This does nothing if gpu data is up to date)
        //---------------------------------------------------------------------
        void flush();

        //----------------------------------------------------------------------
        // Bind the uniform buffer to the device context. Might flush it before
        // if the data on the gpu is not up to date.
        //----------------------------------------------------------------------
        void bind();

    private:
        ShaderUniformBufferDeclaration  m_bufferInfo;
        Byte*                           m_CPUBuffer     = nullptr;
        UniformBuffer                   m_GPUBuffer;
        bool                            m_gpuUpToDate   = false;

        //----------------------------------------------------------------------
        void _FreeBuffers();

        NULL_COPY_AND_ASSIGN(MappedUniformBuffer)
    };


    //**********************************************************************
    // Same class as MappedUniformBuffer but with the additional feature that
    // it can be updated and binded more than once per frame.
    //**********************************************************************
    class CachedMappedUniformBuffer
    {
    public:
        CachedMappedUniformBuffer(const ShaderUniformBufferDeclaration& bufferInfo, BufferUsage usage)
            : m_bufferDecl(bufferInfo), m_bufferUsage(usage) {}
        ~CachedMappedUniformBuffer();

        //----------------------------------------------------------------------
        // Should be called at the begining of a frame to signal a new frame.
        //----------------------------------------------------------------------
        void newFrame();

        //----------------------------------------------------------------------
        // Should be called to signal that a new buffer will be record this frame
        //----------------------------------------------------------------------
        void beginBuffer();

        //----------------------------------------------------------------------
        // Update the given uniform 'name' with the given data. !! ONLY ON THE CPU !!
        // @Return:
        //  False if uniform with the given name does not exist.
        //----------------------------------------------------------------------
        bool update(StringID name, const void* data);

        //----------------------------------------------------------------------
        // Bind the uniform buffer to the device context. Might flush it before
        // if the data on the gpu is not up to date.
        //----------------------------------------------------------------------
        void bind();

    private:
        ArrayList<Vulkan::MappedUniformBuffer*> m_mappedUBOs;
        ShaderUniformBufferDeclaration m_bufferDecl;
        BufferUsage m_bufferUsage;
        I32 m_bufferIndex = 0;

        NULL_COPY_AND_ASSIGN(CachedMappedUniformBuffer)
    };


} } // End namespaces