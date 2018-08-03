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

namespace Components {

    #define DEBUG_FRUSTUM           0
    #define KEY_DEBUG_SNAPSHOT      Key::F

    //----------------------------------------------------------------------
    DirectionalLight::DirectionalLight( F32 intensity, Color color, Graphics::ShadowType shadowType, const ArrayList<F32>& splitRangesWorldSpace )
        : ILightComponent( new Graphics::DirectionalLight( intensity, color ) )
    {
        m_dirLight = dynamic_cast<Graphics::DirectionalLight*>( m_light.get() );
        m_dirLight->setShadowType( shadowType );
        m_dirLight->setCSMSplitRanges( splitRangesWorldSpace );

        if (shadowType != Graphics::ShadowType::None)
            _CreateShadowMap( CONFIG.getShadowMapQuality() );
    }

    //----------------------------------------------------------------------
    DirectionalLight::DirectionalLight( F32 intensity, Color color, Graphics::ShadowType shadowType )
        : DirectionalLight( intensity, color, shadowType, { 5.0f, 30.0f, 60.0f, 200.0f } )
    {
    }

    //----------------------------------------------------------------------
    void DirectionalLight::_CreateShadowMap( Graphics::ShadowMapQuality quality )
    {
        U32 shadowMapSize = 0;
        Graphics::DepthFormat depthFormat;
        Graphics::TextureFormat tempRTFormat; // Used for CSM, must be compatible with "depthFormat"
        switch (quality)
        {
            case Graphics::ShadowMapQuality::Low:
                shadowMapSize = 512;
                depthFormat   = Graphics::DepthFormat::D16; 
                tempRTFormat  = Graphics::TextureFormat::R16; break;
            case Graphics::ShadowMapQuality::Medium:
                shadowMapSize = 1024; 
                depthFormat   = Graphics::DepthFormat::D16;
                tempRTFormat  = Graphics::TextureFormat::R16; break;
            case Graphics::ShadowMapQuality::High:
                shadowMapSize = 2048; 
                depthFormat   = Graphics::DepthFormat::D32;
                tempRTFormat  = Graphics::TextureFormat::RFloat; break;
            case Graphics::ShadowMapQuality::Insane:
                shadowMapSize = 4096; 
                depthFormat   = Graphics::DepthFormat::D32;
                tempRTFormat  = Graphics::TextureFormat::RFloat; break;
        }

        if (shadowMapSize > 0)
        {
            RenderBufferPtr renderBuffer;
            switch ( m_dirLight->getShadowType() )
            {
            case Graphics::ShadowType::Hard:
            case Graphics::ShadowType::Soft:
            {
                // Create shadowmap
                auto shadowMap = RESOURCES.createRenderBuffer();
                shadowMap->create( shadowMapSize, shadowMapSize, depthFormat );
                shadowMap->setAnisoLevel( 1 );
                shadowMap->setFilter( Graphics::TextureFilter::Point );
                shadowMap->setClampMode( Graphics::TextureAddressMode::Clamp );
                m_light->setShadowMap( shadowMap );

                renderBuffer = shadowMap;
                break;
            }
            case Graphics::ShadowType::CSM:
            case Graphics::ShadowType::CSMSoft:
            {
                U32 splitCount = (U32)m_dirLight->getCSMSplits().size();
                ASSERT(splitCount <= Locator::getRenderer().getLimits().maxCascades);

                // Create shadowmap
                auto shadowMap = RESOURCES.createTexture2DArray( shadowMapSize, shadowMapSize, splitCount,
                                                                 tempRTFormat, false );
                shadowMap->setAnisoLevel( 1 );
                shadowMap->setFilter( Graphics::TextureFilter::Point );
                shadowMap->setClampMode( Graphics::TextureAddressMode::Clamp );
                m_dirLight->setShadowMap( shadowMap );

                // Create shadowmap (rendertarget, which gets copied to each slice)
                auto shadowMapRender = RESOURCES.createRenderBuffer();
                shadowMapRender->create( shadowMapSize, shadowMapSize, depthFormat );
                renderBuffer = shadowMapRender;
                break;
            }
            default:
                ASSERT( "Unknown shadow type. Can't create a shadowmap!" );
            }

            // Create rendertexture
            auto rt = RESOURCES.createRenderTexture();
            rt->create( nullptr, renderBuffer );

            m_camera.reset( new Graphics::Camera( -10, 10, -10, 10, 0, 10 ) );
            m_camera->setReplacementShader( ASSETS.getShadowMapShader(), TAG_SHADOW_PASS );
            m_camera->setRenderTarget( rt, Graphics::CameraFlagNone );
        }
    }

    //----------------------------------------------------------------------
    void DirectionalLight::tick( Time::Seconds d )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        m_dirLight->setDirection( transform->rotation.getForward() );
    }

    //----------------------------------------------------------------------
    void DirectionalLight::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        cmd.drawLight( m_dirLight );
    }

    //----------------------------------------------------------------------
    void DirectionalLight::renderShadowMap( const IScene& scene, F32 lerp )
    {
        auto mainCamera = SCENE.getMainCamera();

        switch (m_dirLight->getShadowType())
        {
        case Graphics::ShadowType::Hard:
        case Graphics::ShadowType::Soft:
            // Adapt view frustum so it follows the main camera around
            _AdaptOrthographicViewFrustum( mainCamera, mainCamera->getZNear(), m_dirLight->getShadowRange() );
            ILightComponent::renderShadowMap( scene, lerp );
            break;
        case Graphics::ShadowType::CSM:
        case Graphics::ShadowType::CSMSoft:
        {
            Graphics::CommandBuffer cmd;

            auto& splits = m_dirLight->getCSMSplits();
            for (auto cascade = 0; cascade < splits.size(); ++cascade)
            {
                // Adapt orthographic frustum for this cascade
                F32 zNear = mainCamera->getZNear();
                if (cascade != 0) // First cascade starts at zNear
                    zNear = splits[cascade-1].range;
                F32 zFar = splits[cascade].range;

                _AdaptOrthographicViewFrustum( mainCamera, zNear, zFar );

                // Record commands
                auto transform = getGameObject()->getTransform();
                auto modelMatrix = transform->getWorldMatrix( lerp );
                m_camera->setModelMatrix( modelMatrix );

                // Set light-view projection for this cascade
                m_dirLight->setCSMShadowViewProjection( cascade, m_camera->getViewProjectionMatrix() );

                // Set camera and record commands for every rendering component
                cmd.setCamera( *m_camera );
                for ( auto& renderer : scene.getComponentManager().getRenderer() )
                {
                    if ( not renderer->isActive() || not renderer->isCastingShadows() )
                        continue;

                    // Check if component is visible
                    bool isVisible = renderer->cull( *m_camera );
                    if (isVisible)
                        renderer->recordGraphicsCommands( cmd, lerp );
                }
                cmd.endCamera();

                // Copy rendering into appropriate array slice
                cmd.copyTexture( m_camera->getRenderTarget()->getBuffer(), 0, 0, m_dirLight->getShadowMap(), cascade, 0 );
            }
            Locator::getRenderer().dispatch( cmd );
            break;
        }
        default:
            ASSERT( "This should never happen!" );
        }
    }

    //----------------------------------------------------------------------
    void DirectionalLight::_AdaptOrthographicViewFrustum( Components::Camera* mainCamera, F32 zNear, F32 zFar )
    {
        // Calculate frustum corners in world space
        auto mainCameraTransform = mainCamera->getGameObject()->getTransform();
        auto mainCameraFrustumCornersWS = Math::CalculateFrustumCorners( mainCameraTransform->position, 
                                                                         mainCameraTransform->rotation, 
                                                                         mainCamera->getFOV(), 
                                                                         zNear, zFar,
                                                                         mainCamera->getAspectRatio() );

        // Transform frustum corners in light space and calculate the center from the sphere
        auto transform = getGameObject()->getTransform();
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
        sphereCenter *= ( 1.0f / (F32)mainCameraFrustumCornersWS.size() );

        // Get the min and max bounds in lightspace. (The radius is rather ad-hoc but works pretty well!)
        F32 radius = (mainCameraFrustumCornersWS[0] - mainCameraFrustumCornersWS[7]).magnitude() * 0.5f;
        auto min = sphereCenter - Math::Vec3{ radius, radius, radius };
        auto max = sphereCenter + Math::Vec3{ radius, radius, radius };

        // Snap the orthographic frustum to texel-size, otherwise we will have a very noticeable artifact (shadow shimmering)
        F32 shadowMapWidth = (F32)m_dirLight->getShadowMap()->getWidth();
        F32 diameter = 2.0f * radius;
        F32 worldTexelSize = (diameter / shadowMapWidth);

        min.x = worldTexelSize * std::floor( min.x / worldTexelSize );
        max.x = worldTexelSize * std::floor( max.x / worldTexelSize );
        min.y = worldTexelSize * std::floor( min.y / worldTexelSize );
        max.y = worldTexelSize * std::floor( max.y / worldTexelSize );

        m_camera->setOrthoParams( min.x, max.x, min.y, max.y, min.z, max.z );

#if DEBUG_FRUSTUM
        static const Time::Seconds snapShotTime = 10;
        static Time::Seconds snapShotTimeCounter;

        if (snapShotTimeCounter > Time::Seconds(0))
        {
            DEBUG.drawFrustum({}, transform->rotation, m_camera->getLeft(), m_camera->getRight(), 
                                                        m_camera->getBottom(), m_camera->getTop(), 
                                                        m_camera->getZNear(), m_camera->getZFar(), Color::BLUE, 0);

            DEBUG.drawFrustum(mainCameraTransform->position, mainCameraTransform->rotation.getForward(), mainCameraTransform->rotation.getUp(), 
                              mainCamera->getFOV(), zNear, zFar, mainCamera->getAspectRatio(), Color::GREEN, 0);

            snapShotTimeCounter -= static_cast<Time::Seconds>( PROFILER.getUpdateDelta() );
        }

        // Snapshot of current frustums
        if (KEYBOARD.wasKeyReleased(KEY_DEBUG_SNAPSHOT))
        {
            snapShotTimeCounter = snapShotTime;
            DEBUG.drawFrustum( {}, transform->rotation, m_camera->getLeft(), m_camera->getRight(), 
                                                       m_camera->getBottom(), m_camera->getTop(), 
                                                       m_camera->getZNear(), m_camera->getZFar(), Color::BLUE, snapShotTime );

            DEBUG.drawFrustum( mainCameraTransform->position, mainCameraTransform->rotation.getForward(), mainCameraTransform->rotation.getUp(), 
                               mainCamera->getFOV(), zNear, zFar, mainCamera->getAspectRatio(), Color::GREEN, snapShotTime );

            //DEBUG.drawSphere( sphereCenter, radius, Color::VIOLET, snapShotTime );
            //DEBUG.drawLine( mainCameraTransform->position, sphereCenter, Color::GREEN, snapShotTime );
        }
#endif
    }

} // End namespaces