#include "skybox.h"
/**********************************************************************
    class: Skybox (skybox.cpp)

    author: S. Hau
    date: April 1, 2018
**********************************************************************/

#include "Assets/mesh_generator.h"
#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/i_scene.h"
#include "Rendering/mesh_renderer.h"
#include "Core/locator.h"

namespace Components {

    //----------------------------------------------------------------------
    void Skybox::addedToGameObject(GameObject* go)
    {
        auto m_gameObject = go->getScene()->createGameObject( "Skybox" );

        // Create skybox shader
        m_skyboxShader = ASSETS.getShader( "/shaders/skybox.shader" );

        // Create skybox material
        m_skyboxMaterial = RESOURCES.createMaterial( m_skyboxShader );
        m_skyboxMaterial->setTexture( SID( "Cubemap" ), m_cubemap );

        // Create skybox mesh (make it as large as possible so it wont get culled)
        m_skyboxMesh = Assets::MeshGenerator::CreateCube(100000);

        // Create gameobject which renders the skybox
        auto mr = m_gameObject->addComponent<Components::MeshRenderer>();
        mr->setMesh( m_skyboxMesh );
        mr->setMaterial( m_skyboxMaterial );
    }

}