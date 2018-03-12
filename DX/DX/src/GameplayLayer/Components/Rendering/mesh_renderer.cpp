#include "mesh_renderer.h"
/**********************************************************************
    class: MeshRenderer (mesh_renderer.cpp)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "GameplayLayer/Components/transform.h"

namespace Components {

    //----------------------------------------------------------------------
    MeshRenderer::MeshRenderer( Graphics::Mesh* mesh, Graphics::Material* material )
    {
        setMesh( mesh );
        setMaterial( material, 0 );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void MeshRenderer::setMesh( Graphics::Mesh* mesh )
    { 
        m_mesh = mesh; 
        m_materials.resize( m_mesh->getSubMeshCount() );
        for ( auto& mat : m_materials )
            mat = nullptr; // @TODO: Apply default material
    }

    //----------------------------------------------------------------------
    void MeshRenderer::setMaterial( Graphics::Material* m, U32 index ) 
    { 
        ASSERT( index < m_materials.size() && "MeshRenderer::setMaterial(): INVALID INDEX." );
        m_materials[index] = m; 
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void MeshRenderer::AddedToGameObject( GameObject* go )
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
            cmd.drawMesh( m_mesh, m_materials[i], modelMatrix, i );
    }


}