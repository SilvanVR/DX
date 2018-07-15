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

    struct Particle
    {
        Time::Seconds lifetime = 0;
        Math::Vec3 position{ 0, 0, 0 };
        Math::Quat rotation{ Math::Quat::IDENTITY };
        Math::Vec3 scale{ 1, 1, 1 };
        Math::Vec4 color{ 1, 1, 1, 1 };
    };

    //**********************************************************************
    class ParticleSystem : public IRenderComponent
    {
    public:
        ParticleSystem(const MaterialPtr& material);
        ParticleSystem(const OS::Path& path);

        //----------------------------------------------------------------------
        const MaterialPtr&  getMaterial()               const { return m_material; }
        const MeshPtr&      getMesh()                   const { return m_particleMesh; }
        U32                 getCurrentParticleCount()   const { return m_currentParticleCount; }
        U32                 getMaxParticleCount()       const { return m_maxParticleCount; }
        U32                 getEmissionRate()           const { return m_emissionRate; }
        const Time::Clock&  getClock()                  const { return m_clock; }
        F32                 getGravity()                const { return m_gravity;}
        PSSortMode          getSortMode()               const { return m_sortMode; }
        PSParticleAlignment getParticleAlignment()      const { return m_particleAlignment; }
        Time::Clock&        getClock()                        { return m_clock; }

        void setMesh                (const MeshPtr& mesh)           { m_particleMesh = mesh; }
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

    private:
        MeshPtr             m_particleMesh;
        MaterialPtr         m_material;
        U32                 m_maxParticleCount = 100;
        U32                 m_currentParticleCount = 0;
        U32                 m_emissionRate = 10;
        F32                 m_gravity = 0.0f;
        Time::Clock         m_clock{ 5000_ms };
        PSSortMode          m_sortMode = PSSortMode::None;
        PSParticleAlignment m_particleAlignment = PSParticleAlignment::None;
        F32                 m_accumulatedSpawnTime = 0.0f;
        ArrayList<Particle> m_particles;

        void _SpawnParticles(Time::Seconds delta);
        void _UpdateParticles(Time::Seconds delta);
        void _AlignParticles(PSParticleAlignment alignment);
        void _SortParticles(PSSortMode sortMode);
        void _UpdateMesh();

        //----------------------------------------------------------------------
        // IComponent Interface
        //----------------------------------------------------------------------
        void tick(Time::Seconds delta) override;

        //----------------------------------------------------------------------
        // IRendererComponent Interface
        //----------------------------------------------------------------------
        void recordGraphicsCommands(Graphics::CommandBuffer& cmd, F32 lerp) override;
        bool cull(const Graphics::Camera& camera) override;

        NULL_COPY_AND_ASSIGN(ParticleSystem)
    };

}