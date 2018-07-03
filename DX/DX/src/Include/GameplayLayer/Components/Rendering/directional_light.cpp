#include "directional_light.h"
/**********************************************************************
    class: DirectionalLight (directional_light.h)
    
    author: S. Hau
    date: May 14, 2018
**********************************************************************/

#include "Graphics/command_buffer.h"
#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/i_scene.h"
#include "i_render_component.hpp"
#include "Graphics/camera.h"
#include "Core/locator.h"
#include "camera.h"
#include "Math/math_utils.h"
#include <limits>

namespace Components {

    #define DEPTH_STENCIL_FORMAT    Graphics::DepthFormat::D24S8
    #define Z_NEAR_OFFSET           8
    #define Z_FAR                   15
    #define DEBUG_FRUSTUM           1

    //----------------------------------------------------------------------
    DirectionalLight::DirectionalLight( F32 intensity, Color color, bool shadowsEnabled )
        : ILightComponent( new Graphics::DirectionalLight( intensity, color ) )
    {
        m_dirLight = dynamic_cast<Graphics::DirectionalLight*>( m_light.get() );

        if (shadowsEnabled)
            _CreateShadowMap( CONFIG.getShadowMapQuality() );
    }

    //----------------------------------------------------------------------
    void DirectionalLight::_CreateShadowMap( Graphics::ShadowMapQuality quality )
    {
        U32 shadowMapSize = 0;
        switch (quality)
        {
            case Graphics::ShadowMapQuality::Low:     shadowMapSize = 512;  break;
            case Graphics::ShadowMapQuality::Medium:  shadowMapSize = 1024; break;
            case Graphics::ShadowMapQuality::High:    shadowMapSize = 2048; break;
            case Graphics::ShadowMapQuality::Insane:  shadowMapSize = 4096; break;
        }

        if (shadowMapSize > 0)
        {
            // Create shadowmap
            auto shadowMap = RESOURCES.createRenderBuffer();
            shadowMap->create( shadowMapSize, shadowMapSize, DEPTH_STENCIL_FORMAT );
            shadowMap->setAnisoLevel( 1 );
            shadowMap->setFilter( Graphics::TextureFilter::Point );
            m_light->setShadowMap( shadowMap );

            // Create rendertexture
            auto rt = RESOURCES.createRenderTexture();
            rt->create( nullptr, shadowMap );

            // Configure camera
            m_camera.reset( new Graphics::Camera( -10, 10, -10, 10, -Z_NEAR_OFFSET, Z_FAR ) );
            m_camera->setRenderTarget( rt, false );
            m_camera->setReplacementShader( ASSETS.getShadowMapShader(), TAG_SHADOW_PASS );
        }
    }

    //----------------------------------------------------------------------
    void DirectionalLight::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        m_dirLight->setDirection( transform->rotation.getForward() );

        cmd.drawLight( m_dirLight );
    }

    //----------------------------------------------------------------------
    void DirectionalLight::renderShadowMap( const IScene& scene, F32 lerp )
    {
        // Adapt position of the camera frustum
        auto mainCamera = SCENE.getMainCamera();
        auto mainCameraTransform = mainCamera->getGameObject()->getTransform();
        auto transform = getGameObject()->getTransform();

        // Calculate frustum corners in view space
        auto mainCameraFrustumCornersWS = Math::CalculateFrustumCorners( mainCameraTransform->position, 
                                                                         mainCameraTransform->rotation, 
                                                                         mainCamera->getFOV(), 
                                                                         mainCamera->getZNear(), Z_FAR, mainCamera->getAspectRatio() );

        // Transform frustum corners in light space and retrieve min/max for the frustum
        auto worldToLight = DirectX::XMMatrixInverse( nullptr, transform->getWorldMatrix() );

        F32 minX = std::numeric_limits<F32>::max();
        F32 maxX = std::numeric_limits<F32>::lowest();
        F32 minY = std::numeric_limits<F32>::max();
        F32 maxY = std::numeric_limits<F32>::lowest();
        F32 minZ = std::numeric_limits<F32>::max();
        F32 maxZ = std::numeric_limits<F32>::lowest();
        for (auto i = 0; i < mainCameraFrustumCornersWS.size(); i++)
        {
            auto corner = DirectX::XMLoadFloat3( &mainCameraFrustumCornersWS[i] );
            auto cornerLS = DirectX::XMVector4Transform( corner, worldToLight );

            Math::Vec3 cornerLightSpace;
            DirectX::XMStoreFloat3( &cornerLightSpace, cornerLS );

            minX = std::min( minX, cornerLightSpace.x );
            maxX = std::max( maxX, cornerLightSpace.x );
            minY = std::min( minY, cornerLightSpace.y );
            maxY = std::max( maxY, cornerLightSpace.y );
            minZ = std::min( minZ, cornerLightSpace.z );
            maxZ = std::max( maxZ, cornerLightSpace.z );
        }

        F32 shadowMapWidth = (F32)m_light->getShadowMap()->getWidth();


        F32 shadowLengthX = maxX - minX;
        F32 worldTexelSizeX = (shadowLengthX / shadowMapWidth);
        minX = worldTexelSizeX * std::floor(minX / worldTexelSizeX);
        maxX = worldTexelSizeX * std::floor(maxX / worldTexelSizeX);

        F32 shadowLengthY = maxY - minY;
        F32 worldTexelSizeY = (shadowLengthY / shadowMapWidth);
        minY = worldTexelSizeY * std::floor(minY / worldTexelSizeY);
        maxY = worldTexelSizeY * std::floor(maxY / worldTexelSizeY);

        F32 shadowDistance = maxZ - minZ;
        m_dirLight->setShadowRange( Z_FAR );

        //LOG(TS(shadowLengthX) + ", " + TS(shadowLengthY) + ", " + TS(shadowLengthZ) );
        //lightSpaceCameraPos.x = worldTexelSize * std::floor(lightSpaceCameraPos.x / worldTexelSize);
        //lightSpaceCameraPos.y = worldTexelSize * std::floor(lightSpaceCameraPos.y / worldTexelSize);

        m_camera->setOrthoParams( minX, maxX, minY, maxY, minZ - Z_NEAR_OFFSET, maxZ );

#if DEBUG_FRUSTUM
        DEBUG.drawFrustum({}, transform->rotation, m_camera->getLeft(), m_camera->getRight(), 
                                                    m_camera->getBottom(), m_camera->getTop(), 
                                                    m_camera->getZNear(), m_camera->getZFar(), Color::BLUE, 0);

        DEBUG.drawFrustum(mainCameraTransform->position, mainCameraTransform->rotation.getForward(), mainCameraTransform->rotation.getUp(), 
                          mainCamera->getFOV(), mainCamera->getZNear(), Z_FAR, mainCamera->getAspectRatio(), Color::GREEN, 0);

        // Snapshot of current frustums
        if (KEYBOARD.wasKeyReleased(Key::F))
        {
            DEBUG.drawFrustum({}, transform->rotation, m_camera->getLeft(), m_camera->getRight(), 
                                                       m_camera->getBottom(), m_camera->getTop(), 
                                                       m_camera->getZNear(), m_camera->getZFar(), Color::BLUE, 1000);

            DEBUG.drawFrustum(mainCameraTransform->position, mainCameraTransform->rotation.getForward(), mainCameraTransform->rotation.getUp(), 
                              mainCamera->getFOV(), mainCamera->getZNear(), Z_FAR, mainCamera->getAspectRatio(), Color::GREEN, 1000);
        }
#endif

        //// Snap the camera-position to texel-size to fix "Shadow-Swimming" coming from the fitting-algorithm
        //F32 worldTexelSize = (shadowDistance / m_light->getShadowMap()->getWidth());

        //auto lightSpaceCameraPos = transform->rotation.conjugate() * tempPosition;
        //lightSpaceCameraPos.x = worldTexelSize * std::floor(lightSpaceCameraPos.x / worldTexelSize);
        //lightSpaceCameraPos.y = worldTexelSize * std::floor(lightSpaceCameraPos.y / worldTexelSize);

        //transform->position = transform->rotation * lightSpaceCameraPos;

        ILightComponent::renderShadowMap( scene, lerp );
    }


} // End namespaces