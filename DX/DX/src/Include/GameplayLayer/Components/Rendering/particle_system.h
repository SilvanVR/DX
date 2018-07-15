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
#include "Math/random.h"

namespace Components {

    //----------------------------------------------------------------------
    enum class PSValueMode
    {
        Constant,
        RandomBetweenTwoConstants
    };

    //----------------------------------------------------------------------
    template <typename T>
    struct Constant
    {
        Constant(const T& val) : val{ val } {}
        T operator() () { return val; }
        T val;
    };

    //----------------------------------------------------------------------
    template <typename T>
    struct RandomBetweenTwoConstants
    {
        RandomBetweenTwoConstants(const T& min, const T& max) : min{ min }, max{ max } {}
        T operator() (){ return Math::Random::value<T>( min, max ); }
        T min;
        T max;
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
        void setSpawnLifetimeFnc(const std::function<F32()>& fnc) { m_spawnLifeTimeFnc = fnc; }
        void setSpawnColorFnc(const std::function<Color()>& fnc) { m_spawnColorFnc = fnc; }

        //----------------------------------------------------------------------
        // Begins playing this particle system from the beginning.
        //----------------------------------------------------------------------
        void play();

        //----------------------------------------------------------------------
        // Pauses this particle system.
        //----------------------------------------------------------------------
        void pause() { m_clock.setTickModifier( 0.0f ); }

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

        //----------------------------------------------------------------------
        std::function<F32()>    m_spawnLifeTimeFnc = Constant<F32>{ 2.0f };
        std::function<Color()>  m_spawnColorFnc = Constant<Color>{ Color::WHITE };

        //----------------------------------------------------------------------
        void _SpawnParticles(Time::Seconds delta);
        void _SpawnParticle(U32 particleIndex);
        void _UpdateParticles(Time::Seconds delta);
        void _AlignParticles(PSParticleAlignment alignment);
        void _SortParticles(PSSortMode sortMode);
        void _UpdateMesh();

        void _RecalculateBounds();

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