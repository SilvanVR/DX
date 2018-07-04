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
            shadowMap->setClampMode( Graphics::TextureAddressMode::Clamp );
            m_light->setShadowMap( shadowMap );

            // Create rendertexture
            auto rt = RESOURCES.createRenderTexture();
            rt->create( nullptr, shadowMap );

            // Configure camera
            m_camera.reset( new Graphics::Camera( -10, 10, -10, 10, 0, m_dirLight->getShadowRange() ) );
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
        // Adapt view frustum so it follows the main camera around
        _AdaptOrthographicViewFrustum();

        ILightComponent::renderShadowMap( scene, lerp );
    }

    //----------------------------------------------------------------------
    void DirectionalLight::_AdaptOrthographicViewFrustum()
    {
        // Adapt position of the camera frustum
        auto mainCamera = SCENE.getMainCamera();
        auto mainCameraTransform = mainCamera->getGameObject()->getTransform();
        auto transform = getGameObject()->getTransform();

        // Calculate frustum corners in world space
        auto mainCameraFrustumCornersWS = Math::CalculateFrustumCorners( mainCameraTransform->position, 
                                                                         mainCameraTransform->rotation, 
                                                                         mainCamera->getFOV(), 
                                                                         mainCamera->getZNear(), m_dirLight->getShadowRange(), 
                                                                         mainCamera->getAspectRatio() );

        // Transform frustum corners in light space and calculate the center from the sphere
        auto worldToLight = DirectX::XMMatrixInverse( nullptr, transform->getWorldMatrix() );

        Math::Vec3 sphereCenter{ 0, 0, 0 };
        for (auto i = 0; i < mainCameraFrustumCornersWS.size(); i++)
        {
            auto corner = DirectX::XMLoadFloat3( &mainCameraFrustumCornersWS[i] );
            auto cornerV = DirectX::XMVector4Transform( corner, worldToLight );

            Math::Vec3 cornerLightSpace;
            DirectX::XMStoreFloat3( &cornerLightSpace, cornerV );

            sphereCenter += cornerLightSpace;
        }

        sphereCenter *= (1.0f / 8.0f);

        // Get the min and max bounds in lightspace. (The radius is rather ad-hoc but works pretty well!)
        F32 radius = (mainCameraFrustumCornersWS[0] - mainCameraFrustumCornersWS[7]).magnitude() * 0.5f;
        auto min = sphereCenter - Math::Vec3{ radius, radius, radius };
        auto max = sphereCenter + Math::Vec3{ radius, radius, radius };

        // Snap the orthographic frustum to texel-size, otherwise we will have a very noticeable artifact (shadow shimmering)
        F32 shadowMapWidth = (F32)m_dirLight->getShadowMap()->getWidth();
        F32 worldTexelSize = (2.0f*radius / shadowMapWidth);

        min.x = worldTexelSize * std::floor( min.x / worldTexelSize );
        max.x = worldTexelSize * std::floor( max.x / worldTexelSize );
        min.y = worldTexelSize * std::floor( min.y / worldTexelSize );
        max.y = worldTexelSize * std::floor( max.y / worldTexelSize );

        m_camera->setOrthoParams( min.x, max.x, min.y, max.y, min.z, max.z );

#if DEBUG_FRUSTUM
        DEBUG.drawFrustum({}, transform->rotation, m_camera->getLeft(), m_camera->getRight(), 
                                                    m_camera->getBottom(), m_camera->getTop(), 
                                                    m_camera->getZNear(), m_camera->getZFar(), Color::BLUE, 0);

        DEBUG.drawFrustum(mainCameraTransform->position, mainCameraTransform->rotation.getForward(), mainCameraTransform->rotation.getUp(), 
                          mainCamera->getFOV(), mainCamera->getZNear(), m_dirLight->getShadowRange(), mainCamera->getAspectRatio(), Color::GREEN, 0);

        // Snapshot of current frustums
        if (KEYBOARD.wasKeyReleased(Key::F))
        {
            DEBUG.drawFrustum({}, transform->rotation, m_camera->getLeft(), m_camera->getRight(), 
                                                       m_camera->getBottom(), m_camera->getTop(), 
                                                       m_camera->getZNear(), m_camera->getZFar(), Color::BLUE, 10);

            DEBUG.drawFrustum(mainCameraTransform->position, mainCameraTransform->rotation.getForward(), mainCameraTransform->rotation.getUp(), 
                              mainCamera->getFOV(), mainCamera->getZNear(), m_dirLight->getShadowRange(), mainCamera->getAspectRatio(), Color::GREEN, 10);
        }
#endif
    }

} // End namespaces