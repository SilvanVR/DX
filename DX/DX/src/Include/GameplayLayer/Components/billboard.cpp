#include "billboard.h"
/**********************************************************************
    class: Billboard (billboard.cpp)

    author: S. Hau
    date: May 23, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "Rendering/mesh_renderer.h"
#include "GameplayLayer/i_scene.h"
#include "Core/locator.h"

namespace Components {

    //----------------------------------------------------------------------
    void Billboard::addedToGameObject(GameObject* go)
    {
        // Create billboard shader
        auto billboardShader = ASSETS.getShader( "/engine/shaders/geometry_billboard.shader" );
        if ( not billboardShader )
        {
            LOG_WARN( "Components::Billboard: Billboard shader does not exist, therefore this component will not work." );
            return;
        }

        // Create billboard material
        auto billboardMaterial = RESOURCES.createMaterial( billboardShader );
        billboardMaterial->setTexture( "tex", m_billboardTexture );

        // Create billboard mesh (use uv coords as size params)
        F32 ar = m_billboardTexture->getAspectRatio();
        auto billboardMesh = RESOURCES.createMesh();
        billboardMesh->setVertices( { Math::Vec3( 0, 0, 0 ) } );
        billboardMesh->setIndices( { 0 }, 0, Graphics::MeshTopology::Points );
        billboardMesh->setUVs( { Math::Vec2( m_scale * ar, m_scale ) } );

        // Set custom bounds, otherwise the billboard will be culled by the single point
        F32 halfWidth = m_scale / 2 * ar;
        Math::AABB customBounds( Math::Vec3( -halfWidth, -m_scale / 2, -halfWidth ), Math::Vec3( halfWidth, m_scale / 2, halfWidth ) );
        billboardMesh->setBounds( customBounds );

        // Create gameobject which renders the billboard
        m_billboardGO = go->getScene()->createGameObject( "Billboard" );
        go->getTransform()->addChild( m_billboardGO->getTransform(), false );
        auto mr = m_billboardGO->addComponent<Components::MeshRenderer>( billboardMesh, billboardMaterial );
        mr->setCastShadows( false );
    }

}