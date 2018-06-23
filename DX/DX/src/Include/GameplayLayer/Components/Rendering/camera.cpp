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
#include "Core/layer_defines.hpp"
#include "GameplayLayer/i_scene.h"


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
    void Camera::render( F32 lerp, const IScene& scene )
    {
        // Update camera 
        auto transform = getGameObject()->getTransform();
        auto modelMatrix = transform->getWorldMatrix( lerp );
        m_camera.setModelMatrix( modelMatrix );
        _UpdateCullingPlanes( m_camera.getViewProjectionMatrix() );

        Graphics::CommandBuffer cmd;

        // Set camera
        cmd.setCamera( &m_camera );

        // Record commands for every rendering component
        for ( auto& renderer : scene.getComponentManager().getRenderer() )
        {
            if ( not renderer->isActive() )
                continue;

            // Check if layer matches
            bool layerMatch = ( m_cullingMask & renderer->getGameObject()->getLayers() ).isAnyBitSet();
            if ( not layerMatch )
                continue;

            // Check if component is visible
            bool isVisible = renderer->cull( *this );
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

        // Inject an command which blits last rendered buffer to the screen/render target
        cmd.blit( PREVIOUS_BUFFER, m_camera.isRenderingToScreen() ? SCREEN_BUFFER : getRenderTarget(), RESOURCES.getPostProcessMaterial() );

        // Add an end camera command
        cmd.endCamera( &m_camera );

        // Submit command buffer to render engine
        Locator::getRenderer().dispatch( cmd );
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

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

    //----------------------------------------------------------------------
    bool Camera::cull( const Math::AABB& aabb, const DirectX::XMMATRIX& modelMatrix ) const
    {
        // Cull against frustum by transforming vertices from AABB into clip space
        auto mvp = modelMatrix * getViewProjectionMatrix();
        auto aabbCorners = aabb.getCorners();

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

    //----------------------------------------------------------------------
    bool Camera::cull( const Math::Vec3& pos, F32 radius ) const
    {
        for ( U32 i = 0; i < m_planes.size(); i++ )
        {
            if ( (m_planes[i].x * pos.x) + (m_planes[i].y * pos.y) + (m_planes[i].z * pos.z) + m_planes[i].w <= -radius )
                return false;
        }
        return true;
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

    //----------------------------------------------------------------------
    void Camera::_UpdateCullingPlanes( const DirectX::XMMATRIX& viewProjection )
    {
        Math::Vec4 row0, row1, row2, row3;
        DirectX::XMStoreFloat4( &row0, viewProjection.r[0] );
        DirectX::XMStoreFloat4( &row1, viewProjection.r[1] );
        DirectX::XMStoreFloat4( &row2, viewProjection.r[2] );
        DirectX::XMStoreFloat4( &row3, viewProjection.r[3] );

        m_planes[LEFT].x = row0.w + row0.x;
        m_planes[LEFT].y = row1.w + row1.x;
        m_planes[LEFT].z = row2.w + row2.x;
        m_planes[LEFT].w = row3.w + row3.x;

        m_planes[RIGHT].x = row0.w - row0.x;
        m_planes[RIGHT].y = row1.w - row1.x;
        m_planes[RIGHT].z = row2.w - row2.x;
        m_planes[RIGHT].w = row3.w - row3.x;

        m_planes[TOP].x = row0.w - row0.y;
        m_planes[TOP].y = row1.w - row1.y;
        m_planes[TOP].z = row2.w - row2.y;
        m_planes[TOP].w = row3.w - row3.y;

        m_planes[BOTTOM].x = row0.w + row0.y;
        m_planes[BOTTOM].y = row1.w + row1.y;
        m_planes[BOTTOM].z = row2.w + row2.y;
        m_planes[BOTTOM].w = row3.w + row3.y;

        m_planes[BACK].x = row0.w + row0.z;
        m_planes[BACK].y = row1.w + row1.z;
        m_planes[BACK].z = row2.w + row2.z;
        m_planes[BACK].w = row3.w + row3.z;

        m_planes[FRONT].x = row0.w - row0.z;
        m_planes[FRONT].y = row1.w - row1.z;
        m_planes[FRONT].z = row2.w - row2.z;
        m_planes[FRONT].w = row3.w - row3.z;

        for ( U32 i = 0; i < m_planes.size(); i++ )
        {
            F32 length = sqrtf( m_planes[i].x * m_planes[i].x + m_planes[i].y * m_planes[i].y + m_planes[i].z * m_planes[i].z );
            m_planes[i] /= length;
        }
    }

}