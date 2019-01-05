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

        // Apply given material to all submeshes
        for (I32 i = 0; i < m_materials.size(); i++)
            setMaterial( material, i );
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
            m_materials.resize( std::max( (I32)m_mesh->getSubMeshCount(), 1 ) );
            for ( I32 i = 0; i < m_materials.size(); i++ )
                if (m_materials[i] == nullptr)
                    m_materials[i] = ASSETS.getErrorMaterial();
        }
    }

    //----------------------------------------------------------------------
    void MeshRenderer::setMaterial( const MaterialPtr& m, U32 subMeshIndex )
    { 
        ASSERT( subMeshIndex < m_materials.size() && "MeshRenderer::setMaterial(): INVALID INDEX." );
        m_materials[subMeshIndex] = (m == nullptr ? ASSETS.getErrorMaterial() : m);
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void MeshRenderer::recordGraphicsCommands( Graphics::CommandBuffer& cmd )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        // Draw submesh with appropriate material
        auto modelMatrix = transform->getWorldMatrix();
        for (I32 i = 0; i < m_mesh->getSubMeshCount(); i++)
            cmd.drawMesh( m_mesh, m_materials[i], modelMatrix, i );
    }

    //----------------------------------------------------------------------
    bool MeshRenderer::cull( const Graphics::Camera& camera )
    {
        if ( m_mesh == nullptr )
            return false;

        auto modelMatrix = getGameObject()->getTransform()->getWorldMatrix();
        return camera.cull( m_mesh->getBounds(), modelMatrix );
    }
}