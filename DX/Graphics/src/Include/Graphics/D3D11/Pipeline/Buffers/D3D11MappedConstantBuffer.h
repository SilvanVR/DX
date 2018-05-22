#pragma once
/**********************************************************************
    class: MappedConstantBuffer (D3D11MappedConstantBuffer.h)

    author: S. Hau
    date: May 13, 2018
**********************************************************************/

#include "D3D11Buffers.h"
#include "D3D11/D3D11Structs.hpp"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    // This class encapsulates a constant buffer from a shader buffer description,
    // so you can update properties by name and flush the whole data in one go to the gpu.
    //**********************************************************************
    class MappedConstantBuffer
    {
    public:
        MappedConstantBuffer() = default;
        MappedConstantBuffer(const ConstantBufferInfo& bufferInfo, BufferUsage usage);
        ~MappedConstantBuffer() { _FreeBuffers(); }

        //----------------------------------------------------------------------
        inline bool                        gpuIsUpToDate() const { return m_gpuUpToDate; }
        inline const ConstantBufferInfo&   getBufferInfo() const { return m_bufferInfo; }

        //----------------------------------------------------------------------
        // Update the given uniform 'name' with the given data. !! ONLY ON THE CPU !!
        // @Return:
        //  False if uniform with the given name does not exist.
        //----------------------------------------------------------------------
        bool update(StringID name, const void* data);

        //----------------------------------------------------------------------
        // Update the given uniform 'name' with the given data.
        // This updates the uniform buffer directly on the gpu.
        //----------------------------------------------------------------------
        void update(const void* data, Size sizeInBytes);

        //----------------------------------------------------------------------
        // Sends the whole buffer on the cpu to the gpu. (This does nothing if gpu data is up to date)
        //---------------------------------------------------------------------
        void flush();

        //----------------------------------------------------------------------
        // Bind the constant buffer to the device context. Might flush it before
        // if the data on the gpu is not up to date.
        // @Params:
        // "shaderType": The shader to bind the constant buffer.
        //----------------------------------------------------------------------
        void bind(ShaderType shaderType);

        //----------------------------------------------------------------------
        // Marks the gpu buffer as no longer up-to-date.
        //----------------------------------------------------------------------
        void invalidate() { m_gpuUpToDate = false; }

    private:
        ConstantBufferInfo  m_bufferInfo;
        Byte*               m_CPUBuffer     = nullptr;
        ConstantBuffer*     m_GPUBuffer     = nullptr;
        bool                m_gpuUpToDate   = false;

        //----------------------------------------------------------------------
        void _FreeBuffers();
    };

} } // End namespaces