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
    enum class PSValueSetting
    {
        Constant,
        RandomBetweenTwoConstants,
        Curve
    };

    //----------------------------------------------------------------------
    enum class PSSortMode
    {
        None,
        ByDistance, // Sort by distance to main camera
    };

    //----------------------------------------------------------------------
    enum class PSParticleAlignment
    {
        None,
        View, // Aligned to main camera
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
        U32                 getEmissionRate()           const { return m_emissionRate; }
        const Time::Clock&  getClock()                  const { return m_clock; }
        F32                 getGravity()                const { return m_gravity;}
        PSSortMode          getSortMode()               const { return m_sortMode; }
        PSParticleAlignment getParticleAlignment()      const { return m_particleAlignment; }
        Time::Clock&        getClock()                        { return m_clock; }

        void setMesh                (const MeshPtr& mesh)           { m_particleSystem = mesh; }
        void setMaterial            (const MaterialPtr& mat)        { m_material = mat; }
        void setMaxParticleCount    (U32 maxParticles)              { m_maxParticleCount = maxParticles; play(); }
        void setEmissionRate        (U32 emissionRate)              { m_emissionRate = emissionRate; }
        void setGravity             (F32 gravity)                   { m_gravity = gravity; }
        void setSortMode            (PSSortMode sortMode)           { m_sortMode = sortMode; }
        void setParticleAlignment   (PSParticleAlignment alignment) { m_particleAlignment = alignment; }

        //----------------------------------------------------------------------
        // Begins playing this particle system from the beginning.
        //----------------------------------------------------------------------
        void play();

        //----------------------------------------------------------------------
        void tick(Time::Seconds delta) override;

    private:
        MeshPtr             m_particleSystem;
        MaterialPtr         m_material;
        U32                 m_maxParticleCount = 100;
        U32                 m_currentParticleCount = 0;
        U32                 m_emissionRate = 10;
        F32                 m_gravity = 0.0f;
        Time::Clock         m_clock{ 5000_ms };
        PSSortMode          m_sortMode = PSSortMode::None;
        PSParticleAlignment m_particleAlignment = PSParticleAlignment::None;

        void _KillParticles();
        void _SpawnParticles();
        void _UpdateParticles();
        void _SortParticles(PSSortMode sortMode);

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Graphics::Camera& camera) override;

        NULL_COPY_AND_ASSIGN(ParticleSystem)
    };

}