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
            LOG_WARN( "MeshRenderer-Component requires a transform for a game-object, but the attached game-object has none!" );
    }

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

        // Cull mesh against frustum by transforming vertices from AABB into clip space
        auto modelMatrix = getGameObject()->getTransform()->getWorldMatrix();
        auto mvp = modelMatrix * camera.getViewProjectionMatrix();

        auto aabbCorners = m_mesh->getBounds().getCorners();

        std::array<DirectX::XMVECTOR, 8> cornersClipSpace;
        for (I32 i = 0; i < cornersClipSpace.size(); i++)
        {
            auto corner = DirectX::XMVectorSet( aabbCorners[i].x, aabbCorners[i].y, aabbCorners[i].z, 1.0f );
            cornersClipSpace[i] = DirectX::XMVector4Transform( corner, mvp );
        }

        I32 c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0;
        for (I32 i = 0; i < cornersClipSpace.size(); i++)
        {
            Math::Vec4 cornerClipSpace;
            DirectX::XMStoreFloat4( &cornerClipSpace, cornersClipSpace[i] );
            if (cornerClipSpace.x < -cornerClipSpace.w) c1++;
            if (cornerClipSpace.x >  cornerClipSpace.w) c2++;
            if (cornerClipSpace.y < -cornerClipSpace.w) c3++;
            if (cornerClipSpace.y >  cornerClipSpace.w) c4++;
            if (cornerClipSpace.z < -cornerClipSpace.w) c5++;
            if (cornerClipSpace.z >  cornerClipSpace.w) c6++;
        }

        if (c1 == 8 || c2 == 8 || c3 == 8 || c4 == 8 || c5 == 8 || c6 == 8)
            return false;

        return true;
    }
}