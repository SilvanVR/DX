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
#include "Math/math_utils.h"

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
    enum class PSShape
    {
        Box,
        Sphere
    };

    //----------------------------------------------------------------------
    struct ShapeBox
    {
        ShapeBox(const Math::Vec3& min, const Math::Vec3& max) : min{ min }, max{ max } {}
        Math::Vec3 operator() () { return Math::Random::Vec3( min, max ); }
        Math::Vec3 min;
        Math::Vec3 max;
    };

    //----------------------------------------------------------------------
    struct ShapeSphere
    {
        ShapeSphere(const Math::Vec3& center, const F32& radius) : center{ center }, radius{ radius } {}
        Math::Vec3 operator() () { return center + Math::Random::Vec3(-1,1).normalized() * Math::Random::Float(radius); }
        Math::Vec3 center;
        F32 radius;
    };

    //----------------------------------------------------------------------
    // Interpolates between a given set of data. E.g. [0,0], [1,10] -> when called with (0.5f) returns 5
    //----------------------------------------------------------------------
    template <typename T, typename T2>
    struct LinearLerpBetweenValues
    {
        LinearLerpBetweenValues(const HashMap<T, T2>& data) : m_dataMap{ data } { ASSERT(data.size() > 0); }
        T2 operator() (F32 lerp)
        {
            if (m_dataMap.size() == 1)
                return m_dataMap.begin()->second;

            auto it = m_dataMap.begin();
            auto prev = it;

            // Find the correct spot i.e. [0,0.3,0.6,1.0] + lerp=0.5: it=0.6, prev=0.3
            while (it != m_dataMap.end() && it->first <= lerp)
            {
                prev = it;
                ++it;
            }

            // If we reached the end or just return last value
            if (it == m_dataMap.end())
                return prev->second;

            // Calculate contribution between the two values enclosing "lerp"
            F32 l = (lerp - prev->first) / (it->first - prev->first);
            return Math::Lerp( prev->second, it->second, l );
        }

        HashMap<T, T2> m_dataMap;
    };

    //**********************************************************************
    class ParticleSystem : public IRenderComponent
    {
    public:
        ParticleSystem(const MaterialPtr& material, bool playOnStart = true);
        ParticleSystem(const OS::Path& path);

        //----------------------------------------------------------------------
        enum class SortMode
        {
            None,
            ByDistance, // Sort by distance to main camera
        };

        //----------------------------------------------------------------------
        enum class ParticleAlignment
        {
            None,
            View, // Aligned to main camera
        };

        //----------------------------------------------------------------------
        const MaterialPtr&  getMaterial()               const { return m_material; }
        const MeshPtr&      getMesh()                   const { return m_particleMesh; }
        U32                 getCurrentParticleCount()   const { return m_currentParticleCount; }
        U32                 getMaxParticleCount()       const { return m_maxParticleCount; }
        U32                 getEmissionRate()           const { return m_emissionRate; }
        const Time::Clock&  getClock()                  const { return m_clock; }
        F32                 getGravity()                const { return m_gravity;}
        SortMode            getSortMode()               const { return m_sortMode; }
        ParticleAlignment   getParticleAlignment()      const { return m_particleAlignment; }
        Time::Clock&        getClock()                        { return m_clock; }

        void setMesh                (const MeshPtr& mesh)           { m_particleMesh = mesh; play(); }
        void setMaterial            (const MaterialPtr& mat)        { m_material = mat; play(); }
        void setMaxParticleCount    (U32 maxParticles)              { m_maxParticleCount = maxParticles; play(); }
        void setEmissionRate        (U32 emissionRate)              { m_emissionRate = emissionRate; }
        void setGravity             (F32 gravity)                   { m_gravity = gravity; }
        void setSortMode            (SortMode sortMode)           { m_sortMode = sortMode; }
        void setParticleAlignment   (ParticleAlignment alignment) { m_particleAlignment = alignment; }

        //----------------------------------------------------------------------
        void setSpawnLifetimeFnc    (const std::function<F32()>& fnc)       { m_spawnLifeTimeFnc = fnc; }
        void setSpawnColorFnc       (const std::function<Color()>& fnc)     { m_spawnColorFnc = fnc; }
        void setSpawnScaleFnc       (const std::function<F32()>& fnc)       { m_spawnScaleFnc = fnc; }
        void setSpawnPositionFunc   (const std::function<Math::Vec3()> fnc) { m_spawnPositionFnc = fnc; }
        void setSpawnVelocityFunc   (const std::function<Math::Vec3()> fnc) { m_spawnVelocityFnc = fnc; }
        void setSpawnRotationFunc   (const std::function<Math::Quat()> fnc) { m_spawnRotationFnc = fnc; }

        void setLifetimeColorFnc    (const std::function<Color(F32)>& fnc)      { m_lifeTimeColorFnc = fnc; }
        void setLifetimeScaleFnc    (const std::function<F32(F32)>& fnc)        { m_lifeTimeScaleFnc = fnc; }
        void setLifetimeRotationFnc (const std::function<Math::Quat(F32)>& fnc) { m_lifeTimeRotationFnc = fnc; }

        //----------------------------------------------------------------------
        // Begins playing this particle system from the beginning.
        //----------------------------------------------------------------------
        void play();

        //----------------------------------------------------------------------
        void pause() { m_paused = true; }
        void resume() { m_paused = false; }

    private:
        MeshPtr             m_particleMesh;
        MaterialPtr         m_material;
        U32                 m_maxParticleCount = 100;
        U32                 m_currentParticleCount = 0;
        U32                 m_emissionRate = 10;
        F32                 m_gravity = 0.0f;
        Time::Clock         m_clock{ 5000_ms };
        SortMode            m_sortMode = SortMode::None;
        ParticleAlignment   m_particleAlignment = ParticleAlignment::None;
        F32                 m_accumulatedSpawnTime = 0.0f;
        bool                m_paused = false;

        //**********************************************************************
        struct Particle
        {
            Time::Seconds   startLifetime;
            Time::Seconds   remainingLifetime;
            Math::Vec3      position;
            Math::Vec3      spawnScale;
            Math::Vec3      scale;
            Math::Quat      spawnRotation;
            Math::Quat      rotation;
            Color           spawnColor;
            Color           color;
            Math::Vec3      velocity;
        };
        ArrayList<Particle> m_particles;

        //----------------------------------------------------------------------
        std::function<F32()>        m_spawnLifeTimeFnc  = Constant<F32>{ 4.0f };
        std::function<F32()>        m_spawnScaleFnc     = Constant<F32>{ 1.0f };
        std::function<Math::Quat()> m_spawnRotationFnc  = Constant<Math::Quat>{ Math::Quat::IDENTITY };
        std::function<Color()>      m_spawnColorFnc     = Constant<Color>{ Color::WHITE };
        std::function<Math::Vec3()> m_spawnPositionFnc  = ShapeBox{ {-1,-1,-1}, {1,1,1} };
        std::function<Math::Vec3()> m_spawnVelocityFnc  = Constant<Math::Vec3>{ {0,0,0} };

        std::function<Color(F32)>       m_lifeTimeColorFnc = nullptr;
        std::function<F32(F32)>         m_lifeTimeScaleFnc = nullptr;
        std::function<Math::Quat(F32)>  m_lifeTimeRotationFnc = nullptr;

        //----------------------------------------------------------------------
        void _SpawnParticles(Time::Seconds delta);
        void _SpawnParticle(U32 particleIndex);
        void _UpdateParticles(Time::Seconds delta);
        void _AlignParticles(ParticleAlignment alignment);
        void _SortParticles(SortMode sortMode);
        void _UpdateMesh();

        void _LoadFromFile(const OS::Path& path);

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