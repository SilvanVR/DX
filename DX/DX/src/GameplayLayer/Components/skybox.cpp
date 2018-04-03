#include "skybox.h"
/**********************************************************************
    class: Skybox (skybox.cpp)

    author: S. Hau
    date: April 1, 2018
**********************************************************************/

#include "Assets/MeshGenerator/mesh_generator.h"
#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/i_scene.h"
#include "Rendering/mesh_renderer.h"
#include "locator.h"

namespace Components {

    // Draw skybox at last with depth testing enabled
    #define SHADER_PRIORITY 10000

    //----------------------------------------------------------------------
    void Skybox::AddedToGameObject(GameObject* go)
    {
        auto m_gameObject = go->getScene()->createGameObject( "Skybox" );

        // Create skybox shader
        auto m_skyboxShader = RESOURCES.createShader( "Skybox", "/shaders/skyboxVS.hlsl", "/shaders/skyboxPS.hlsl" );
        m_skyboxShader->setRasterizationState( { Graphics::FillMode::Solid, Graphics::CullMode::Front } );
        m_skyboxShader->setPriority( SHADER_PRIORITY );

        // Create skybox material
        auto m_skyboxMaterial = RESOURCES.createMaterial();
        m_skyboxMaterial->setShader( m_skyboxShader );
        m_skyboxMaterial->setTexture( SID( "Cubemap" ), m_cubemap );
        m_skyboxMaterial->setColor( SID( "tintColor" ), Color::WHITE );

        // Create skybox mesh
        auto mesh = Assets::MeshGenerator::CreateCube();

        // Create gameobject which renders the skybox
        auto mr = m_gameObject->addComponent<Components::MeshRenderer>();
        mr->setMesh( mesh );
        mr->setMaterial( m_skyboxMaterial );
    }

}