#include "debug_manager.h"
/**********************************************************************
    class: DebugManager (debug_manager.cpp)

    author: S. Hau
    date: April 2, 2018

    @Consideration:
     - Create one mesh for each material and update only mesh data,
       so everything will be just drawn with one draw-call
**********************************************************************/

#include "locator.h"
#include "Events/event_dispatcher.h"
#include "Core/event_names.hpp"
#include "GameplayLayer/i_scene.h"
#include "Assets/MeshGenerator/mesh_generator.h"

namespace Core { namespace Debug {

    //----------------------------------------------------------------------
    void DebugManager::init()
    {
        Locator::getCoreEngine().subscribe( this );

        // Register to switch scene event
        Events::Event& evt = Events::EventDispatcher::GetEvent( EVENT_SCENE_CHANGED );
        evt.addListener( BIND_THIS_FUNC_0_ARGS( &DebugManager::_OnSceneChanged ) );

        // Create both shaders with / withot depth-test
        m_colorShaderWireframe = RESOURCES.createShader( "DEBUG-DEPTH", "/shaders/colorVS.hlsl", "/shaders/colorPS.hlsl" );
        m_colorShaderWireframe->setRasterizationState( { Graphics::FillMode::Wireframe, Graphics::CullMode::None } );

        m_colorShaderWireframeNoDepthTest = RESOURCES.createShader( "DEBUG-NO DEPTH", "/shaders/colorVS.hlsl", "/shaders/colorPS.hlsl" );
        m_colorShaderWireframeNoDepthTest->setRasterizationState( { Graphics::FillMode::Wireframe, Graphics::CullMode::None } );
        m_colorShaderWireframeNoDepthTest->setDepthStencilState( { false } );

        // Create material from both shaders
        m_colorMaterial = RESOURCES.createMaterial( m_colorShaderWireframe );
        m_colorMaterialNoDepthTest = RESOURCES.createMaterial( m_colorShaderWireframeNoDepthTest );
    }

    //----------------------------------------------------------------------
    void DebugManager::OnTick( Time::Seconds delta )
    {
        bool erasedMesh = false;
        for (auto it = m_currentMeshes.begin(); it != m_currentMeshes.end();)
        {
            it->duration -= delta;
            if (it->duration < 0)
            {
                erasedMesh = true;
                it = m_currentMeshes.erase( it );
                // @TODO: Delete mesh from resource manager
            }
            else
            {
                it++;
            }
        }

        // A mesh was removed, update command buffer list
        if (erasedMesh)
            _UpdateCommandBuffer();
    }

    //----------------------------------------------------------------------
    void DebugManager::shutdown()
    {
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void DebugManager::drawLine( const Math::Vec3& start, const Math::Vec3& end, Color color, Time::Seconds duration, bool depthTest )
    {
        auto mesh = RESOURCES.createMesh();
        mesh->setVertices( { start, end } );
        mesh->setIndices( { 0, 1 }, 0, Graphics::MeshTopology::Lines );
        mesh->setColors( { color, color } );

        MeshInfo meshInfo;
        meshInfo.mesh = mesh;
        meshInfo.duration = duration;
        meshInfo.depthTest = depthTest;
        m_currentMeshes.push_back( meshInfo );

        m_commandBuffer.drawMesh( mesh, depthTest ? m_colorMaterial : m_colorMaterialNoDepthTest, DirectX::XMMatrixIdentity(), 0 );
    }

    //----------------------------------------------------------------------
    void DebugManager::drawRay( const Math::Vec3& start, const Math::Vec3& direction, Color color, Time::Seconds duration, bool depthTest )
    {
        auto mesh = RESOURCES.createMesh();
        mesh->setVertices( { start, start + direction } );
        mesh->setIndices( { 0, 1 }, 0, Graphics::MeshTopology::Lines );
        mesh->setColors( { color, color } );

        MeshInfo meshInfo;
        meshInfo.mesh = mesh;
        meshInfo.duration = duration;
        meshInfo.depthTest = depthTest;
        m_currentMeshes.push_back( meshInfo );

        m_commandBuffer.drawMesh( mesh, depthTest ? m_colorMaterial : m_colorMaterialNoDepthTest, DirectX::XMMatrixIdentity(), 0 );
    }

    //----------------------------------------------------------------------
    void DebugManager::drawBox( const Math::Vec3& min, const Math::Vec3& max, Color color, Time::Seconds duration, bool depthTest )
    {
        // Line for each edge
        ArrayList<Math::Vec3> vertices =
        {
            min,
            Math::Vec3( min.x, min.y, max.z ),
            Math::Vec3( max.x, min.y, min.z ),
            Math::Vec3( min.x, max.y, min.z ),
            max,
            Math::Vec3(max.x, max.y, min.z),
            Math::Vec3(min.x, max.y, max.z),
            Math::Vec3(max.x, min.y, max.z),
        };
        ArrayList<U32> indices = {
            0, 1,
            0, 2,
            0, 3,
            4, 5,
            4, 6,
            4, 7,
            1, 6,
            3, 6,
            2, 7,
            2, 5,
            1, 7,
            3, 5
        };
        ArrayList<Color> colors( indices.size(), color );

        auto mesh = RESOURCES.createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices, 0, Graphics::MeshTopology::Lines );
        mesh->setColors( colors );

        MeshInfo meshInfo;
        meshInfo.mesh       = mesh;
        meshInfo.duration   = duration;
        meshInfo.depthTest  = depthTest;
        m_currentMeshes.push_back( meshInfo );

        m_commandBuffer.drawMesh( mesh, depthTest ? m_colorMaterial : m_colorMaterialNoDepthTest, DirectX::XMMatrixIdentity(), 0 );
    }

    //----------------------------------------------------------------------
    void DebugManager::drawSphere( const Math::Vec3& center, F32 radius, Color color, Time::Seconds duration, bool depthTest )
    {
        auto mesh = Assets::MeshGenerator::CreateUVSphere( center, radius, 30, 30 );
        ArrayList<Color> colors( mesh->getIndexCount( 0 ), color );
        mesh->setColors( colors );

        MeshInfo meshInfo;
        meshInfo.mesh       = mesh;
        meshInfo.duration   = duration;
        meshInfo.depthTest  = depthTest;
        m_currentMeshes.push_back( meshInfo );

        m_commandBuffer.drawMesh( mesh, depthTest ? m_colorMaterial : m_colorMaterialNoDepthTest, DirectX::XMMatrixIdentity(), 0 );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void DebugManager::_UpdateCommandBuffer()
    {
        // Clear all previous commands
        m_commandBuffer.reset();

        // Record new commands
        for (auto& meshInfo : m_currentMeshes)
            m_commandBuffer.drawMesh( meshInfo.mesh, meshInfo.depthTest ? m_colorMaterial : m_colorMaterialNoDepthTest, DirectX::XMMatrixIdentity(), 0 );
    }

    //----------------------------------------------------------------------
    void DebugManager::_OnSceneChanged()
    {
        // Re-Add the command buffer to the new cameras in the scene
        for ( auto& cam : SCENE.getComponentManager().getCameras() )
            cam->addCommandBuffer( &m_commandBuffer );
    }

} } // end namespaces