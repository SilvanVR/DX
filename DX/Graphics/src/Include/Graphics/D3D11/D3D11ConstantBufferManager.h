#pragma once
/**********************************************************************
    class: ConstantBufferManager (D3D11ConstantBufferManager.h)

    author: S. Hau
    date: May 12, 2018

    Creates and manages different constant buffers e.g. the per-object/camera
    buffer. It creates those buffers whenever such a buffer is first encountered
    in a shader.
**********************************************************************/

#include "D3D11Structs.hpp"
#include "Pipeline/Buffers/D3D11MappedConstantBuffer.h"

#define GLOBAL_BUFFER_KEYWORD "global"
#define OBJECT_BUFFER_KEYWORD "object"
#define CAMERA_BUFFER_KEYWORD "camera"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class ConstantBufferManager
    {
    public:
        //----------------------------------------------------------------------
        inline static MappedConstantBuffer& getObjectBuffer() { return *s_pConstantBufferObject; }
        inline static MappedConstantBuffer& getCameraBuffer() { return *s_pConstantBufferCamera; }
        inline static MappedConstantBuffer& getGlobalBuffer() { return *s_pConstantBufferGlobal; }

        inline static bool hasObjectBuffer() { return s_pConstantBufferObject != nullptr; }
        inline static bool hasCameraBuffer() { return s_pConstantBufferCamera != nullptr; }
        inline static bool hasGlobalBuffer() { return s_pConstantBufferGlobal != nullptr; }

        //----------------------------------------------------------------------
        static void ReflectConstantBuffers(const HashMap<StringID, ConstantBufferInfo>& constantBuffers);

        //----------------------------------------------------------------------
        static void Destroy();

    private:
        static MappedConstantBuffer* s_pConstantBufferGlobal;
        static MappedConstantBuffer* s_pConstantBufferObject;
        static MappedConstantBuffer* s_pConstantBufferCamera;

        //----------------------------------------------------------------------
        ConstantBufferManager()                                                 = delete;
        ConstantBufferManager(const ConstantBufferManager& other)               = delete;
        ConstantBufferManager& operator = (const ConstantBufferManager& other)  = delete;
        ConstantBufferManager(ConstantBufferManager&& other)                    = delete;
        ConstantBufferManager& operator = (ConstantBufferManager&& other)       = delete;
    };



} } // End namespaces