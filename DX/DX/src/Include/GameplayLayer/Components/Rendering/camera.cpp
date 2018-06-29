#include "camera.h"
/**********************************************************************
    class: Camera (camera.cpp)

    author: S. Hau
    date: March 4, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "i_render_component.hpp"
#include "Math/math_utils.h"
#include "Core/locator.h"
#include "GameplayLayer/i_scene.h"
#include "i_light_component.hpp"


namespace Components {

    #define BUFFER_FORMAT_LDR       Graphics::TextureFormat::RGBA32
    #define BUFFER_FORMAT_HDR       Graphics::TextureFormat::RGBAFloat
    #define DEPTH_STENCIL_FORMAT    Graphics::DepthFormat::D32

    //----------------------------------------------------------------------
    Camera::Camera( const RenderTexturePtr& rt, F32 fovAngleYInDegree, F32 zNear, F32 zFar )
        : m_camera( fovAngleYInDegree, zNear, zFar ), m_cullingMask( LAYER_ALL )
    {
        setRenderTarget( rt, true );
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 fovAngleYInDegree, F32 zNear, F32 zFar, Graphics::MSAASamples numSamples, bool hdr )
        : m_camera( fovAngleYInDegree, zNear, zFar ), m_cullingMask( LAYER_ALL ), m_hdr( hdr )
    {
        _CreateRenderTarget( numSamples );
    }

    //----------------------------------------------------------------------
    Camera::Camera( F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar, Graphics::MSAASamples numSamples, bool hdr )
        : m_camera( left, right, bottom, top, zNear, zFar ), m_cullingMask( LAYER_ALL ), m_hdr( hdr )
    {
        _CreateRenderTarget( numSamples );
    }

    //----------------------------------------------------------------------
    void Camera::_CreateRenderTarget( Graphics::MSAASamples sampleCount )
    {
        auto& window = Locator::getWindow();
        auto rt = RESOURCES.createRenderTexture( window.getWidth(), window.getHeight(), 
                                                 DEPTH_STENCIL_FORMAT, m_hdr ? BUFFER_FORMAT_HDR : BUFFER_FORMAT_LDR, 
                                                 sampleCount, true );
        rt->setDynamicScreenScale( true, 1.0f );

        setRenderTarget( rt, true );
    }

    //----------------------------------------------------------------------
    void Camera::render( const IScene& scene, F32 lerp )
    {
        // Update camera 
        auto transform = getGameObject()->getTransform();
        auto modelMatrix = transform->getWorldMatrix( lerp );
        m_camera.setModelMatrix( modelMatrix );

        Graphics::CommandBuffer cmd;

        // Set camera
        cmd.setCamera( &m_camera );

        // Record commands for every light component
        ArrayList<ILightComponent*> visibleLights;
        for ( auto& light : scene.getComponentManager().getLights() )
        {
            if ( not light->isActive() )
                continue;

            // Check if layer matches
            bool layerMatch = m_cullingMask & light->getGameObject()->getLayerMask();
            if ( not layerMatch )
                continue;

            // Check if component is visible
            bool isVisible = light->cull( m_camera );
            if (isVisible)
            {
                light->recordGraphicsCommands( cmd, lerp );
                visibleLights.push_back( light );
            }
        }

        // Render shadowmap if enabled for a visible light
        if (visibleLights.size() > 0)
        {
            for (auto& light : visibleLights)
                if ( light->shadowsEnabled() )
                    light->renderShadowMap( scene, lerp );
        }

        // Record commands for every rendering component
        for ( auto& renderer : scene.getComponentManager().getRenderer() )
        {
            if ( not renderer->isActive() )
                continue;

            // Check if layer matches
            bool layerMatch = m_cullingMask & renderer->getGameObject()->getLayerMask();
            if ( not layerMatch )
                continue;

            // Check if component is visible
            bool isVisible = renderer->cull( m_camera );
            if (isVisible)
                renderer->recordGraphicsCommands( cmd, lerp );
        }

        // Merge all geometry commands
        for (auto& additionalCmd : m_additionalCommandBuffers[CameraEvent::Geometry])
            cmd.merge( *additionalCmd );

        // Sort all draw commands
        cmd.sortDrawCommands( transform->position );

        // Merge all post process commands
        for (auto& additionalCmd : m_additionalCommandBuffers[CameraEvent::PostProcess])
            cmd.merge( *additionalCmd );

        // Merge all gui commands
        for (auto& additionalCmd : m_additionalCommandBuffers[CameraEvent::Overlay])
            cmd.merge( *additionalCmd );

        // Inject an command which blits last rendered buffer to the screen/render target if we
        // have at least one post processing command buffer attached or we are rendering to the screen.
        if( m_camera.isRenderingToScreen() )
            cmd.blit( PREVIOUS_BUFFER, SCREEN_BUFFER, ASSETS.getPostProcessMaterial() );
        else if (m_additionalCommandBuffers[CameraEvent::PostProcess].size() > 0)
            cmd.blit( PREVIOUS_BUFFER, getRenderTarget(), ASSETS.getPostProcessMaterial() );

        // Add an end camera command
        cmd.endCamera( &m_camera );

        // Submit command buffers to render engine
        Locator::getRenderer().dispatch( cmd );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void Camera::addCommandBuffer(Graphics::CommandBuffer* cmd, CameraEvent evt )
    { 
        m_additionalCommandBuffers[evt].push_back( cmd );
    }

    //----------------------------------------------------------------------
    void Camera::removeCommandBuffer(Graphics::CommandBuffer* cmd) 
    { 
        for (auto& pair : m_additionalCommandBuffers)
            pair.second.erase( std::remove( pair.second.begin(), pair.second.end(), cmd ) );
    }

    //----------------------------------------------------------------------
    void Camera::setHDRRendering( bool enabled )
    {
        if (m_hdr == enabled)
            return;

        m_hdr = enabled;
        getRenderTarget()->recreate( m_hdr ? BUFFER_FORMAT_HDR : BUFFER_FORMAT_LDR );
    }

    //----------------------------------------------------------------------
    void Camera::setSuperSampling( F32 screenResMod )
    {
        if (getRenderTarget()->getDynamicScaleFactor() == screenResMod)
            return;
        getRenderTarget()->setDynamicScreenScale( true, screenResMod );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //struct RenderQueueSort
    //{
    //    bool operator()(const MaterialPtr& lhs, const MaterialPtr& rhs) const
    //    {
    //        return lhs->getShader()->getRenderQueue() < rhs->getShader()->getRenderQueue();
    //    }
    //};

    ////----------------------------------------------------------------------
    //void Camera::_SortRenderCommands( const Graphics::CommandBuffer& cmd, const Math::Vec3& cameraPos )
    //{
    //    auto commands = cmd.getGPUCommands();

    //    // Sort rendering commands by material, which are sorted by their renderqueue
    //    std::map<MaterialPtr, ArrayList<std::shared_ptr<Graphics::GPUC_DrawMesh>>, RenderQueueSort> sortByMaterials;
    //    for (auto& command : cmd.getGPUCommands())
    //    {
    //        switch (command->getType())
    //        {
    //        case Graphics::GPUCommand::DRAW_MESH:
    //        {
    //            auto c = std::dynamic_pointer_cast<Graphics::GPUC_DrawMesh>( command );
    //            sortByMaterials[c->material].push_back( c );
    //            break;
    //        }
    //        case Graphics::GPUCommand::DRAW_LIGHT:
    //        {
    //            // Add command immediately
    //            m_commandBuffer.getGPUCommands().push_back( command );
    //            break;
    //        }
    //        }
    //    }

    //    // Now sort drawcalls by camera distance
    //    ArrayList<std::shared_ptr<Graphics::GPUC_DrawMesh>> transparentDrawcalls;
    //    for (auto& pair : sortByMaterials)
    //    {
    //        auto& material = pair.first;
    //        auto& drawCalls = pair.second;

    //        // Sort by back-to-front if renderqueue > 3000
    //        if (material->getShader()->getRenderQueue() >= (I32)Graphics::RenderQueue::BackToFrontBoundary)
    //        {
    //            // Must be inserted in a separate list, because ALL transparent drawcalls have to be sorted
    //            transparentDrawcalls.insert( transparentDrawcalls.end(), drawCalls.begin(), drawCalls.end() );
    //        }
    //        else
    //        {
    //            // Front-To-Back if desired.

    //            // Now add drawcall to real command buffer
    //            for (auto& drawCall : drawCalls)
    //                m_commandBuffer.getGPUCommands().push_back( drawCall );
    //        }
    //    }

    //    // Sort all transparent draw-calls back-to-front
    //    std::sort( transparentDrawcalls.begin(), transparentDrawcalls.end(), [=](const std::shared_ptr<Graphics::GPUC_DrawMesh>& d1, const std::shared_ptr<Graphics::GPUC_DrawMesh>& d2) {
    //        auto pos = d1->modelMatrix.r[3];
    //        auto pos2 = d2->modelMatrix.r[3];

    //        auto camPos = DirectX::XMLoadFloat3( &cameraPos );
    //        auto distance1 = DirectX::XMVector4LengthSq( DirectX::XMVectorSubtract( camPos, pos ) );
    //        auto distance2 = DirectX::XMVector4LengthSq( DirectX::XMVectorSubtract( camPos, pos2 ) );

    //        return DirectX::XMVector4Greater( distance1, distance2 );
    //    } );

    //    // Add sorted transparent drawcalls to command buffer
    //    m_commandBuffer.getGPUCommands().insert( m_commandBuffer.getGPUCommands().end(), transparentDrawcalls.begin(), transparentDrawcalls.end() );
    //}


}