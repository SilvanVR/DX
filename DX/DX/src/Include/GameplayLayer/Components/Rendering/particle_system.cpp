#include "particle_system.h"
/**********************************************************************
    class: ParticleSystem

    author: S. Hau
    date: July 10, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/Components/transform.h"
#include "Graphics/command_buffer.h"
#include "GameplayLayer/i_scene.h"
#include "Core/mesh_generator.h"
#include "OS/FileSystem/file.h"
#include "Core/locator.h"
#include "Math/random.h"
#include "camera.h"

namespace Components {

    static const StringID SHADER_NAME_MODEL_MATRIX = SID("MODEL");

    //----------------------------------------------------------------------
    ParticleSystem::ParticleSystem( const OS::Path& path )
    {
        try
        {
           OS::TextFile psFile( path, OS::EFileMode::READ );
        }
        catch (const std::runtime_error& e)
        {
            LOG_WARN( "ParticleSystem: Failed to open '" + path.toString() + "'. Reason: " + e.what() );
        }
    }

    //----------------------------------------------------------------------
    ParticleSystem::ParticleSystem( const MaterialPtr& material )
        : m_material{ material }
    {
        m_particleMesh = Core::MeshGenerator::CreatePlane();
        setParticleAlignment( Components::PSParticleAlignment::View );
        setSortMode( Components::PSSortMode::ByDistance );
        play();
    }

    //----------------------------------------------------------------------
    void ParticleSystem::tick( Time::Seconds delta )
    {
        bool isRunning = m_clock.tick( delta );
        if (isRunning)
        {
            auto clockDelta = m_clock.getDelta();
            _SpawnParticles( clockDelta );
            _UpdateParticles( clockDelta );
        }
        else
        {
            // Clock is has exceeded his duration, but remaining particles must still be updated
            _UpdateParticles( delta * m_clock.getTickModifier() );
        }

        _AlignParticles( m_particleAlignment );
        _SortParticles( m_sortMode );
        _UpdateMesh();
    }

    //----------------------------------------------------------------------
    void ParticleSystem::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        // Draw instanced mesh with appropriate material
        auto modelMatrix = transform->getWorldMatrix( lerp );
        cmd.drawMeshInstanced( m_particleMesh, m_material, modelMatrix, m_currentParticleCount );
    }

    //----------------------------------------------------------------------
    bool ParticleSystem::cull( const Graphics::Camera& camera )
    {
        if ( m_particleMesh == nullptr || (m_maxParticleCount == 0) || (m_currentParticleCount == 0) )
            return false;
        return true;
        
        //auto modelMatrix = getGameObject()->getTransform()->getWorldMatrix();
        //return camera.cull( m_particleSystem->getBounds(), modelMatrix );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void ParticleSystem::play()
    {
        m_currentParticleCount = 0;
        m_accumulatedSpawnTime = 0.0f;
        m_clock.setTickModifier( 1.0f );
        m_clock.setTime( 0_ms );
        m_particles.resize( m_maxParticleCount );

        auto& vertLayout = m_material->getShader()->getVertexLayout();

        m_particleMesh->setBufferUsage(Graphics::BufferUsage::Frequently);
        auto& colorStream = m_particleMesh->createVertexStream<Math::Vec4>(Graphics::SID_VERTEX_COLOR, m_maxParticleCount);
        auto& modelMatrixStream = m_particleMesh->createVertexStream<DirectX::XMMATRIX>(SHADER_NAME_MODEL_MATRIX, m_maxParticleCount);
        //for (U32 i = 0; i < m_maxParticleCount; i++)
        //{
        //    F32 scale = Math::Random::Float(0.1f, 2.0f);
        //    DirectX::XMVECTOR s{ scale, scale, scale };
        //    DirectX::XMVECTOR r{ DirectX::XMQuaternionRotationRollPitchYaw(Math::Random::Float(0, 6.28f),Math::Random::Float(0, 6.28f), Math::Random::Float(0, 6.28f) ) };
        //    DirectX::XMVECTOR p{ Math::Random::Float(-1.0f, 1.0f) * 50.0f, Math::Random::Float(-1.0f, 1.0f)* 50.0f, Math::Random::Float(-1.0f, 1.0f)* 50.0f };
        //    modelMatrixStream[i] = DirectX::XMMatrixAffineTransformation( s, DirectX::XMQuaternionIdentity(), r, p );
        //}
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void ParticleSystem::_SpawnParticles( Time::Seconds delta )
    {
        // How much spawns we should do this tick
        F32 spawnsPerTick = m_emissionRate * (F32)delta;
        m_accumulatedSpawnTime += spawnsPerTick;

        while (m_accumulatedSpawnTime > 1.0f && m_currentParticleCount < m_maxParticleCount)
        {
            _SpawnParticle( m_currentParticleCount );
            m_currentParticleCount++;
            m_accumulatedSpawnTime -= 1.0f;
        }
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_SpawnParticle( U32 particleIndex )
    {
        m_particles[particleIndex].remainingLifetime = m_spawnLifeTimeFnc();
        m_particles[particleIndex].color = m_spawnColorFnc().normalized();

        F32 scale = Math::Random::Float(0.5f, 2.0f);
        m_particles[particleIndex].scale = { scale, scale, scale };
        m_particles[particleIndex].rotation = Math::Quat::FromEulerAngles(Math::Random::Int(0, 360), Math::Random::Int(0, 360), Math::Random::Int(0, 360));
        m_particles[particleIndex].position = { Math::Random::Float(-1.0f, 1.0f) * 20.0f, Math::Random::Float(-1.0f, 1.0f)* 20.0f, Math::Random::Float(-1.0f, 1.0f)* 20.0f };
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_UpdateParticles( Time::Seconds delta )
    {
        for (U32 i = 0; i < m_currentParticleCount;)
        {
            m_particles[i].remainingLifetime -= delta;
            m_particles[i].position.y -= m_gravity * (F32)delta;

            if (m_particles[i].remainingLifetime < 0_s)
            {
                // Move last living particle in this space
                m_particles[i] = m_particles[m_currentParticleCount - 1];
                m_currentParticleCount--;
            }
            else
            {
                i++;
            }
        }
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_AlignParticles( PSParticleAlignment alignment )
    {
        switch (alignment)
        {
        case PSParticleAlignment::View:
        {
            // 1. Since the view matrix rotates everything "eyeRot" backwards, to negate it we just add the eyeRot itself
            // 2. To negate the world rotation itself, we just need the conjugate
            auto worldRot = getGameObject()->getTransform()->getWorldRotation();
            auto& eyeRot = SCENE.getMainCamera()->getGameObject()->getTransform()->rotation;
            auto alignedRotation = eyeRot * worldRot.conjugate();
            for (U32 i = 0; i < m_currentParticleCount; i++)
                m_particles[i].rotation = alignedRotation;
            break;
        }
        case PSParticleAlignment::None: break;
        }
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_SortParticles( PSSortMode sortMode )
    {
        switch (sortMode)
        {
        case PSSortMode::ByDistance:
        {
            auto worldMatrix = getGameObject()->getTransform()->getWorldMatrix();
            auto eyePos = DirectX::XMLoadFloat3( &SCENE.getMainCamera()->getGameObject()->getTransform()->position );

            // Sorting particles by distance to camera comes with one caveat:
            // 1.) Floating point precision can cause incorrect ordering when the camera moves around the particle
            //     Solution: Disable Z-Writes
            std::sort( m_particles.begin(), m_particles.begin() + m_currentParticleCount, [worldMatrix, eyePos](const Particle& p1, const Particle& p2) {
                auto vPos1 = DirectX::XMLoadFloat3( &p1.position );
                auto vPos2 = DirectX::XMLoadFloat3( &p2.position );
                auto pos1 = DirectX::XMVector3Transform( vPos1, worldMatrix );
                auto pos2 = DirectX::XMVector3Transform( vPos2, worldMatrix );

                auto distance1 = DirectX::XMVector3LengthSq( DirectX::XMVectorSubtract( eyePos, pos1 ) );
                auto distance2 = DirectX::XMVector3LengthSq( DirectX::XMVectorSubtract( eyePos, pos2 ) );

                return DirectX::XMVector3Greater( distance1, distance2 );
            } );
            break;
        }
        case PSSortMode::None: break;
        }
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_UpdateMesh()
    {
        auto& matrixStream = m_particleMesh->getVertexStream<DirectX::XMMATRIX>( SHADER_NAME_MODEL_MATRIX );
        auto& colorStream = m_particleMesh->getVertexStream<Math::Vec4>( Graphics::SID_VERTEX_COLOR );

        for (U32 i = 0; i < m_currentParticleCount; i++)
        {
            DirectX::XMVECTOR s = DirectX::XMLoadFloat3( &m_particles[i].scale );
            DirectX::XMVECTOR r = DirectX::XMLoadFloat4( &m_particles[i].rotation );
            DirectX::XMVECTOR p = DirectX::XMLoadFloat3( &m_particles[i].position );
            matrixStream[i] = DirectX::XMMatrixAffineTransformation( s, DirectX::XMQuaternionIdentity(), r, p );

            F32 l = (F32)m_particles[i].remainingLifetime;
            m_particles[i].color.w = (l / 1.0f);
            colorStream[i] = m_particles[i].color;
        }
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_RecalculateBounds()
    {
        Math::AABB bounds;
        bounds[0] = {};
        bounds[1] = {};
        m_particleMesh->setBounds( bounds );
    }

}