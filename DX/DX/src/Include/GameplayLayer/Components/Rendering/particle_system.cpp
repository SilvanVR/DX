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
#include "Core/locator.h"
#include "Math/random.h"
#include "camera.h"

namespace Components {

    //----------------------------------------------------------------------
    //ParticleSystem::ParticleSystem( const ParticleSystemPtr& ps )
    //    : m_particleSystem( ps )
    //{
    //}

    //----------------------------------------------------------------------
    ParticleSystem::ParticleSystem( const MaterialPtr& material )
        : m_material( material )
    {
        m_maxParticles = 10000;

        m_particleSystem = Core::MeshGenerator::CreatePlane();
        m_particleSystem->setBufferUsage( Graphics::BufferUsage::Frequently );

        auto& vertLayout = m_material->getShader()->getVertexLayout();

        ArrayList<Math::Vec2> uvs(m_maxParticles);
        for (U32 i = 0; i < m_maxParticles; i++)
            uvs[i] = { Math::Random::Float(1.0f), Math::Random::Float(1.0f) };
        m_particleSystem->setUVs(uvs);
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void ParticleSystem::tick( Time::Seconds delta )
    {
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
        cmd.drawMeshInstanced( m_particleSystem, m_material, modelMatrix, m_maxParticles );
    }

    //----------------------------------------------------------------------
    bool ParticleSystem::cull( const Graphics::Camera& camera )
    {
        return true;
        //if ( m_particleSystem == nullptr )
        //    return false;
        //
        //auto modelMatrix = getGameObject()->getTransform()->getWorldMatrix();
        //return camera.cull( m_particleSystem->getBounds(), modelMatrix );
    }
}