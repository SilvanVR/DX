#include "particle_system.h"
/**********************************************************************
    class: ParticleSystem

    author: S. Hau
    date: July 10, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "../transform.h"
#include "Core/locator.h"
#include "camera.h"

namespace Components {

    //----------------------------------------------------------------------
    ParticleSystem::ParticleSystem( const ParticleSystemPtr& ps )
        : m_particleSystem( ps )
    {

    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void ParticleSystem::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        // Draw submesh with appropriate material
        auto modelMatrix = transform->getWorldMatrix( lerp );
        //for (I32 i = 0; i < m_mesh->getSubMeshCount(); i++)
        //    cmd.drawMesh( m_mesh, m_materials[i], modelMatrix, i );
    }

    //----------------------------------------------------------------------
    bool ParticleSystem::cull( const Graphics::Camera& camera )
    {
        if ( m_particleSystem == nullptr )
            return false;

        auto modelMatrix = getGameObject()->getTransform()->getWorldMatrix();
        return camera.cull( m_particleSystem->getBounds(), modelMatrix );
    }
}