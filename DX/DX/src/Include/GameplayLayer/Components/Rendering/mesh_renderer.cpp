#include "mesh_renderer.h"
/**********************************************************************
    class: MeshRenderer (mesh_renderer.cpp)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Graphics/command_buffer.h"
#include "../transform.h"
#include "Core/locator.h"
#include "camera.h"

namespace Components {

    //----------------------------------------------------------------------
    MeshRenderer::MeshRenderer( const MeshPtr& mesh, const MaterialPtr& material )
    {
        setMesh( mesh );
        setMaterial( material, 0 );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void MeshRenderer::setMesh( const MeshPtr& mesh )
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
    void MeshRenderer::setMaterial( const MaterialPtr& m, U32 index )
    { 
        ASSERT( index < m_materials.size() && "MeshRenderer::setMaterial(): INVALID INDEX." );
        m_materials[index] = (m == nullptr ? RESOURCES.getDefaultMaterial() : m);
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void MeshRenderer::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        // Draw submesh with appropriate material
        auto modelMatrix = transform->getWorldMatrix( lerp );
        for (I32 i = 0; i < m_mesh->getSubMeshCount(); i++)
        {
            ASSERT( m_materials[i] != nullptr );
            cmd.drawMesh( m_mesh, m_materials[i], modelMatrix, i );
        }
    }

    //----------------------------------------------------------------------
    bool MeshRenderer::cull( const Camera& camera )
    {
        if ( m_mesh == nullptr )
            return false;

        auto modelMatrix = getGameObject()->getTransform()->getWorldMatrix();
        return camera.cull( m_mesh->getBounds(), modelMatrix );
    }
}