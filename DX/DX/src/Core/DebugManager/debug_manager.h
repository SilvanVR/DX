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
        void drawCube(const Math::Vec3& min, const Math::Vec3& max, Color color, Time::Seconds duration, bool depthTest = true);

        //----------------------------------------------------------------------
        // Draws a sphere in wireframe.
        //----------------------------------------------------------------------
        void drawSphere(const Math::Vec3& center, F32 radius, Color color, Time::Seconds duration, bool depthTest = true);

        //----------------------------------------------------------------------
        // Draws a frustum in wireframe.
        //----------------------------------------------------------------------
        void drawFrustum(const Math::Vec3& pos, const Math::Vec3& up, const Math::Vec3& right, const Math::Vec3& forward,
                         F32 fovAngleYRad, F32 zNear, F32 zFar, F32 aspectRatio,
                         Color color, Time::Seconds duration, bool depthTest = true);

    private:
        // Stores rendering commands for drawing the entire debug stuff
        Graphics::CommandBuffer m_commandBuffer;

        // Wireframe shader with + without depth testing
        ShaderPtr               m_colorShaderWireframe = nullptr;
        ShaderPtr               m_colorShaderWireframeNoDepthTest = nullptr;

        // Corresponding materials for the shaders above
        MaterialPtr             m_colorMaterial = nullptr;
        MaterialPtr             m_colorMaterialNoDepthTest = nullptr;

        // Creates a new mesh for each new debug drawing request
        struct MeshInfo
        {
            MeshPtr         mesh;
            Time::Seconds   duration;
            bool            depthTest;
        };
        ArrayList<MeshInfo> m_currentMeshes;

        //----------------------------------------------------------------------
        void _OnSceneChanged();
        void _UpdateCommandBuffer();
        inline void _AddMesh(MeshPtr mesh, Time::Seconds duration, bool depthTest);

        //----------------------------------------------------------------------
        DebugManager(const DebugManager& other)               = delete;
        DebugManager& operator = (const DebugManager& other)  = delete;
        DebugManager(DebugManager&& other)                    = delete;
        DebugManager& operator = (DebugManager&& other)       = delete;
    };


} } // End namespaces