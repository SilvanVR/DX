#include "particle_system.h"
/**********************************************************************
    class: ParticleSystem

    author: S. Hau
    date: July 10, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/Components/transform.h"
#include "Graphics/command_buffer.h"
#include "Core/mesh_generator.h"
#include "OS/FileSystem/file.h"
#include "Core/locator.h"
#include "Math/random.h"
#include "camera.h"

namespace Components {

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
        : m_material( material )
    {
        m_maxParticleCount = m_currentParticleCount = 5000;

        m_particleSystem = Core::MeshGenerator::CreateCubeUV();
        m_particleSystem->setBufferUsage( Graphics::BufferUsage::Frequently );

        play();
    }

    //----------------------------------------------------------------------
    void ParticleSystem::tick( Time::Seconds delta )
    {
        if ( m_clock.tick( delta ) )
        {
            _KillParticles();
            _SpawnParticles();
            _UpdateParticles();
            _SortParticles( m_sortMode );
        }
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void ParticleSystem::play()
    {
        // @TODO: Kill all particles
        m_clock.setTime( 0_ms );

        auto& vertLayout = m_material->getShader()->getVertexLayout();

        //ArrayList<Math::Vec2> uvs(m_maxParticles);
        //for (U32 i = 0; i < m_maxParticles; i++)
        //    uvs[i] = { Math::Random::Float(1.0f), Math::Random::Float(1.0f) };
        //m_particleSystem->setUVs(uvs);

        ArrayList<Math::Vec3> positions(m_maxParticleCount);
        ArrayList<Color> colors(m_maxParticleCount);
        for (U32 i = 0; i < m_maxParticleCount; i++)
        {
            positions[i] = Math::Vec3{ Math::Random::Float(-1.0f, 1.0f), Math::Random::Float(-1.0f,1.0f), Math::Random::Float(-1.0f,1.0f) } * 50.0f;
            colors[i] = Math::Random::Color();
        }
        m_particleSystem->setNormals(positions);
        m_particleSystem->setColors(colors);
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void ParticleSystem::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        // Draw instanced mesh with appropriate material
        auto modelMatrix = transform->getWorldMatrix( lerp );
        cmd.drawMeshInstanced( m_particleSystem, m_material, modelMatrix, m_maxParticleCount);
    }

    //----------------------------------------------------------------------
    bool ParticleSystem::cull( const Graphics::Camera& camera )
    {
        if ( m_particleSystem == nullptr || (m_maxParticleCount == 0) )
            return false;
        return true;
        
        //auto modelMatrix = getGameObject()->getTransform()->getWorldMatrix();
        //return camera.cull( m_particleSystem->getBounds(), modelMatrix );
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_KillParticles()
    {

    }

    //----------------------------------------------------------------------
    void ParticleSystem::_SpawnParticles()
    {

    }

    //----------------------------------------------------------------------
    void ParticleSystem::_UpdateParticles()
    {

    }

    //----------------------------------------------------------------------
    void ParticleSystem::_SortParticles( PSSortMode sortMode )
    {
        switch (sortMode)
        {
        case PSSortMode::ByDistance:
        {
            break;
        }
        case PSSortMode::None: break;
        }
    }

}