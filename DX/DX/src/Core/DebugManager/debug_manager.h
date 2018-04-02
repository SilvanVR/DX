#pragma once
/**********************************************************************
    class: DebugManager (debug_manager.h)

    author: S. Hau
    date: April 2, 2018
**********************************************************************/

#include "SubSystem/i_subsystem.hpp"
#include "Graphics/command_buffer.h"

namespace Core { namespace Debug {

    //**********************************************************************
    class DebugManager : public ISubSystem
    {
    public:
        DebugManager() = default;
        ~DebugManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void OnTick(Time::Seconds delta) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        void drawLine(const Math::Vec3& start, const Math::Vec3& end, Color color, Time::Seconds duration, bool depthTest = true);
        void drawRay(const Math::Vec3& start, const Math::Vec3& direction, Color color, Time::Seconds duration, bool depthTest = true);

        //----------------------------------------------------------------------
        // Draws a box in wireframe.
        //----------------------------------------------------------------------
        void drawBox(const Math::Vec3& min, const Math::Vec3& max, Color color, Time::Seconds duration, bool depthTest = true);

        //----------------------------------------------------------------------
        // Draws a sphere in wireframe.
        //----------------------------------------------------------------------
        void drawSphere(const Math::Vec3& center, F32 radius, Color color, Time::Seconds duration, bool depthTest = true);

    private:
        // Stores rendering commands for drawing the entire debug stuff
        Graphics::CommandBuffer m_commandBuffer;

        // Wireframe shader with + without depth testing
        Graphics::Shader*       m_colorShaderWireframe = nullptr;
        Graphics::Shader*       m_colorShaderWireframeNoDepthTest = nullptr;

        // Corresponding materials for the shaders above
        Graphics::Material*     m_colorMaterial = nullptr;
        Graphics::Material*     m_colorMaterialNoDepthTest = nullptr;

        // Creates a new mesh for each new debug drawing request
        struct MeshInfo
        {
            Graphics::Mesh* mesh;
            Time::Seconds   duration;
            bool            depthTest;
        };
        ArrayList<MeshInfo> m_currentMeshes;

        //----------------------------------------------------------------------
        void _OnSceneChanged();
        void _UpdateCommandBuffer();

        //----------------------------------------------------------------------
        DebugManager(const DebugManager& other)               = delete;
        DebugManager& operator = (const DebugManager& other)  = delete;
        DebugManager(DebugManager&& other)                    = delete;
        DebugManager& operator = (DebugManager&& other)       = delete;
    };


} } // End namespaces