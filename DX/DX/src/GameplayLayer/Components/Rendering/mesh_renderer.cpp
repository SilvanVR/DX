#include "mesh_renderer.h"
/**********************************************************************
    class: MeshRenderer (mesh_renderer.cpp)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "GameplayLayer/Components/transform.h"
#include "locator.h"

namespace Components {

    //----------------------------------------------------------------------
    MeshRenderer::MeshRenderer( MeshPtr mesh, MaterialPtr material )
    {
        setMesh( mesh );
        setMaterial( material, 0 );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void MeshRenderer::setMesh( MeshPtr mesh )
    { 
        m_mesh = mesh;
        if (m_mesh != nullptr)
        {
            m_materials.resize( m_mesh->getSubMeshCount() );
            for ( I32 i = 0; i < m_materials.size(); i++ )
                if (m_materials[i] == nullptr)
                    m_materials[i] = RESOURCES.getDefaultMaterial();
        }
    }

    //----------------------------------------------------------------------
    void MeshRenderer::setMaterial( MaterialPtr m, U32 index )
    { 
        ASSERT( index < m_materials.size() && "MeshRenderer::setMaterial(): INVALID INDEX." );
        m_materials[index] = (m == nullptr ? RESOURCES.getDefaultMaterial() : m);
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void MeshRenderer::addedToGameObject( GameObject* go )
    {
        auto transform = go->getComponent<Transform>();
        if ( transform == nullptr )
            WARN( "MeshRenderer-Component requires a transform for a game-object, but the attached game-object has none!" );
    }

    //----------------------------------------------------------------------
    void MeshRenderer::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        if ( m_mesh == nullptr )
            return;

        auto transform = getGameObject()->getComponent<Transform>();
        ASSERT( transform != nullptr );

        // Draw submesh with appropriate material
        auto modelMatrix = transform->getTransformationMatrix();
        for (I32 i = 0; i < m_mesh->getSubMeshCount(); i++)
        {
            ASSERT( m_materials[i] != nullptr );
            cmd.drawMesh( m_mesh, m_materials[i], modelMatrix, i );
        }
    }


}