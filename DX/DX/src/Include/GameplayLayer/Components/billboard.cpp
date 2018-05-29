#include "billboard.h"
/**********************************************************************
    class: Billboard (billboard.cpp)

    author: S. Hau
    date: May 23, 2018
**********************************************************************/

#include "Assets/mesh_generator.h"
#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/i_scene.h"
#include "Rendering/mesh_renderer.h"
#include "Core/locator.h"

namespace Components {

    //----------------------------------------------------------------------
    void Billboard::addedToGameObject(GameObject* go)
    {
        m_billboardGO = go->getScene()->createGameObject( "Billboard" );

        // Create billboard shader
        m_billboardShader = ASSETS.getShader( "/shaders/billboard.shader" );

        // Create billboard material
        m_billboardMaterial = RESOURCES.createMaterial( m_billboardShader );
        m_billboardMaterial->setTexture( "tex", m_billboardTexture );

        // Create billboard mesh
        m_billboardMesh = Assets::MeshGenerator::CreatePlane( m_scale * m_billboardTexture->getAspectRatio(), m_scale );

        // Create gameobject which renders the billboard
        auto mr = m_billboardGO->addComponent<Components::MeshRenderer>();
        mr->setMesh( m_billboardMesh );
        mr->setMaterial( m_billboardMaterial );
    }

    //----------------------------------------------------------------------
    void Billboard::tick( Time::Seconds delta )
    {
        // I tried parenting but the billboard should not inherit the scale/rotation.
        m_billboardGO->getTransform()->position = getGameObject()->getTransform()->position;
        m_billboardGO->getTransform()->lookAt( SCENE.getMainCamera()->getGameObject()->getTransform()->position );
    }

}