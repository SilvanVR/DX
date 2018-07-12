#pragma once
/**********************************************************************
    class: ParticleSystem

    author: S. Hau
    date: July 10, 2018
**********************************************************************/

#include "i_render_component.hpp"
#include "Graphics/i_mesh.h"
#include "Graphics/i_material.h"
#include "Time/clock.h"

namespace Components {

    //----------------------------------------------------------------------
    enum class PSSortMode
    {
        None,
        ByDistance, // Sort by distance to main camera
    };

    //**********************************************************************
    class ParticleSystem : public IRenderComponent
    {
    public:
        ParticleSystem(const MaterialPtr& material);
        ParticleSystem(const OS::Path& path);

        //----------------------------------------------------------------------
        U32                 getCurrentParticleCount()   const { return m_currentParticleCount; }
        U32                 getMaxParticleCount()       const { return m_maxParticleCount; }
        const Time::Clock&  getClock()                  const { return m_clock; }
        Time::Clock&        getClock()                        { return m_clock; }

        void setMesh        (const MeshPtr& mesh)       { m_particleSystem = mesh; }
        void setMaterial    (const MaterialPtr& mat)    { m_material = mat; }

        //----------------------------------------------------------------------
        // Begins playing this particle system from the beginning
        //----------------------------------------------------------------------
        void play();

        //----------------------------------------------------------------------
        void tick(Time::Seconds delta) override;

    private:
        MeshPtr     m_particleSystem;
        MaterialPtr m_material;
        U32         m_maxParticleCount = 100;
        U32         m_currentParticleCount = 0;
        Time::Clock m_clock{ 5000_ms };

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Graphics::Camera& camera) override;

        NULL_COPY_AND_ASSIGN(ParticleSystem)
    };

}