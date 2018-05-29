#pragma once
/**********************************************************************
    class: ConstantBufferManager (D3D11ConstantBufferManager.h)

    author: S. Hau
    date: May 12, 2018

    Creates and manages different constant buffers e.g. the per-object/camera
    buffer. It creates those buffers whenever such a buffer is first encountered
    in a shader.
**********************************************************************/

#include "shader_resources.hpp"
#include "Pipeline/Buffers/D3D11MappedConstantBuffer.h"

#define GLOBAL_BUFFER_KEYWORD "global"
#define OBJECT_BUFFER_KEYWORD "object"
#define CAMERA_BUFFER_KEYWORD "camera"
#define LIGHT_BUFFER_KEYWORD  "light"

namespace Graphics { namespace D3D11 {

    //**********************************************************************
    class ConstantBufferManager
    {
    public:
        //----------------------------------------------------------------------
        inline static MappedConstantBuffer& getObjectBuffer() { return *s_pBufferObject; }
        inline static MappedConstantBuffer& getCameraBuffer() { return *s_pBufferCamera; }
        inline static MappedConstantBuffer& getGlobalBuffer() { return *s_pBufferGlobal; }
        inline static MappedConstantBuffer& getLightBuffer()  { return *s_pBufferLights; }

        inline static bool hasObjectBuffer() { return s_pBufferObject != nullptr; }
        inline static bool hasCameraBuffer() { return s_pBufferCamera != nullptr; }
        inline static bool hasGlobalBuffer() { return s_pBufferGlobal != nullptr; }
        inline static bool hasLightBuffer()  { return s_pBufferLights != nullptr; }

        //----------------------------------------------------------------------
        static void ReflectConstantBuffers(const ArrayList<ShaderUniformBufferDeclaration>& constantBuffers);

        //----------------------------------------------------------------------
        static void Destroy();

    private:
        static MappedConstantBuffer* s_pBufferGlobal;
        static MappedConstantBuffer* s_pBufferObject;
        static MappedConstantBuffer* s_pBufferCamera;
        static MappedConstantBuffer* s_pBufferLights;

        //----------------------------------------------------------------------
        ConstantBufferManager()                                                 = delete;
        ConstantBufferManager(const ConstantBufferManager& other)               = delete;
        ConstantBufferManager& operator = (const ConstantBufferManager& other)  = delete;
        ConstantBufferManager(ConstantBufferManager&& other)                    = delete;
        ConstantBufferManager& operator = (ConstantBufferManager&& other)       = delete;
    };



} } // End namespaces