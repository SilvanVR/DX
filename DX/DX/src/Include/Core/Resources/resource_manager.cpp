#include "resource_manager.h"

/**********************************************************************
    class: ResourceManager (resource_manager.cpp)

    author: S. Hau
    date: December 25, 2017

    @Considerations:
      - Different deallocation scheme, e.g. delete after 5sec unused.
**********************************************************************/

#include "Core/locator.h"
#include "Core/mesh_generator.h"
#include "Events/event_dispatcher.h"

#define PRINT_DELETES 0

namespace Core { namespace Resources {

    //----------------------------------------------------------------------
    void ResourceManager::init()
    {
        // Register to resize window event
        Events::Event& evt = Events::EventDispatcher::GetEvent( EVENT_WINDOW_RESIZE );
        m_windowResizeListener = evt.addListener( BIND_THIS_FUNC_0_ARGS( &ResourceManager::_OnWindowSizeChanged ) );
    }

    //----------------------------------------------------------------------
    void ResourceManager::shutdown()
    {
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    MeshPtr ResourceManager::createMesh()
    {
        auto mesh = Locator::getRenderer().createMesh();

        m_meshes.push_back( mesh );

        return MeshPtr( mesh, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteMesh ) );
    }

    //----------------------------------------------------------------------
    MeshPtr ResourceManager::createMesh( const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices )
    {
        auto mesh = createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices );
        return mesh;
    }

    //----------------------------------------------------------------------
    MeshPtr ResourceManager::createMesh( const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices, const ArrayList<Math::Vec2>& uvs )
    {
        auto mesh = createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices );
        mesh->setUVs( uvs );
        return mesh;
    }

    //----------------------------------------------------------------------
    MaterialPtr ResourceManager::createMaterial( const ShaderPtr& shader )
    {
        auto material = Locator::getRenderer().createMaterial();
        if (shader)
            material->setShader( shader );

        m_materials.push_back( material );

        return MaterialPtr( material, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteMaterial ) );
    }

    //----------------------------------------------------------------------
    ShaderPtr ResourceManager::createShader()
    {
        auto shader = Locator::getRenderer().createShader();

        m_shaders.push_back( shader );

        return ShaderPtr( shader, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteShader ) );
    }

    //----------------------------------------------------------------------
    Texture2DPtr ResourceManager::createTexture2D( U32 width, U32 height, Graphics::TextureFormat format, bool generateMips )
    {
        auto texture = Locator::getRenderer().createTexture2D();
        texture->create( width, height, format, generateMips );

        m_textures.push_back( texture );

        return Texture2DPtr( texture, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteTexture ) );
    }

    //----------------------------------------------------------------------
    Texture2DPtr ResourceManager::createTexture2D( U32 width, U32 height, Graphics::TextureFormat format, const void* pData )
    {
        auto texture = Locator::getRenderer().createTexture2D();
        texture->create( width, height, format, pData );

        m_textures.push_back( texture );

        return Texture2DPtr( texture, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteTexture) );
    }

    //----------------------------------------------------------------------
    Texture2DArrayPtr ResourceManager::createTexture2DArray( U32 width, U32 height, U32 depth, Graphics::TextureFormat format, bool generateMips )
    {
        auto texture = Locator::getRenderer().createTexture2DArray();
        texture->create( width, height, depth, format, generateMips );

        m_textures.push_back( texture );

        return Texture2DArrayPtr( texture, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteTexture ) );
    }

    //----------------------------------------------------------------------
    void ResourceManager::setGlobalAnisotropicFiltering( U32 level )
    {
        for ( auto& tex : m_textures )
            tex->setAnisoLevel( level );
    }

    //----------------------------------------------------------------------
    RenderTexturePtr ResourceManager::createRenderTexture()
    {
        auto texture = Locator::getRenderer().createRenderTexture();

        m_renderTextures.push_back( texture );

        return RenderTexturePtr( texture, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteRenderTexture ) );
    }

    //----------------------------------------------------------------------
    RenderTexturePtr ResourceManager::createRenderTexture( U32 width, U32 height, Graphics::TextureFormat format, Graphics::MSAASamples samples )
    {
        auto colorBuffer = createRenderBuffer();
        colorBuffer->create( width, height, format, samples );

        auto renderTexture = createRenderTexture();
        renderTexture->create( colorBuffer );

        return renderTexture;
    }

    //----------------------------------------------------------------------
    RenderTexturePtr ResourceManager::createRenderTexture( U32 width, U32 height, Graphics::TextureFormat format, bool dynamicScale )
    {
        auto renderTexture = createRenderTexture( width, height, format, Graphics::MSAASamples::One );
        renderTexture->setDynamicScreenScale( dynamicScale );

        return renderTexture;
    }

    //----------------------------------------------------------------------
    RenderTexturePtr ResourceManager::createRenderTexture( U32 width, U32 height, Graphics::TextureFormat depth, Graphics::TextureFormat format,
                                                           U32 numBuffers, Graphics::MSAASamples sampleCount, bool dynamicScale )
    {
        ArrayList<RenderBufferPtr> colorBuffers;
        ArrayList<RenderBufferPtr> depthBuffers;
        for (U32 i = 0; i < numBuffers; i++)
        {
            auto colorBuffer = createRenderBuffer();
            colorBuffer->create( width, height, format, sampleCount );
            colorBuffers.push_back( colorBuffer );

            if ( Graphics::IsDepthFormat( depth ) )
            {
                auto depthBuffer = createRenderBuffer();
                depthBuffer->create( width, height, depth, sampleCount );
                depthBuffers.push_back( depthBuffer );
            }
            else
            {
                LOG_WARN( "ResourceManager::createRenderTexture(): Invalid depth format passed in." );
            }
        }

        auto renderTexture = createRenderTexture();
        renderTexture->create( colorBuffers, depthBuffers );
        renderTexture->setDynamicScreenScale( dynamicScale );

        return renderTexture;
    }

    //----------------------------------------------------------------------
    RenderTexturePtr ResourceManager::createRenderTexture( U32 width, U32 height, Graphics::TextureFormat depth, Graphics::TextureFormat format,
                                                           Graphics::MSAASamples sampleCount, bool dynamicScale )
    {
        auto colorBuffer = createRenderBuffer();
        colorBuffer->create( width, height, format, sampleCount );

        RenderBufferPtr depthBuffer = nullptr;
        if ( Graphics::IsDepthFormat( depth ) )
        {
            depthBuffer = createRenderBuffer();
            depthBuffer->create( width, height, depth, sampleCount );
        }
        else
        {
            LOG_WARN( "ResourceManager::createRenderTexture(): Invalid depth format passed in." );
        }

        auto renderTexture = createRenderTexture();
        renderTexture->create( colorBuffer, depthBuffer );

        renderTexture->setDynamicScreenScale( dynamicScale );

        return renderTexture;
    }

    //----------------------------------------------------------------------
    CubemapPtr ResourceManager::createCubemap()
    {
        auto cubemap = Locator::getRenderer().createCubemap();

        m_textures.push_back( cubemap );

        return CubemapPtr( cubemap, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteTexture ) );
    }

    //----------------------------------------------------------------------
    AudioClipPtr ResourceManager::createAudioClip()
    {
        auto audioClip = new Audio::AudioClip();

        m_audioClips.push_back( audioClip );

        return AudioClipPtr( audioClip, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteAudioClip ) );
    }

    //----------------------------------------------------------------------
    RenderBufferPtr ResourceManager::createRenderBuffer()
    {
        auto texture = Locator::getRenderer().createRenderBuffer();

        m_textures.push_back( texture );

        return RenderBufferPtr( texture, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteTexture ) );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void ResourceManager::_DeleteMesh( Graphics::Mesh* mesh )
    {
#if PRINT_DELETES
        LOG( "DELETING MESH", Color::RED );
#endif
        m_meshes.erase( std::remove( m_meshes.begin(), m_meshes.end(), mesh ) );
        SAFE_DELETE( mesh );
    }

    //----------------------------------------------------------------------
    void ResourceManager::_DeleteMaterial( Graphics::Material* mat )
    {
#if PRINT_DELETES
        LOG( "DELETING MATERIAL", Color::RED );
#endif
        m_materials.erase( std::remove( m_materials.begin(), m_materials.end(), mat ) );
        SAFE_DELETE( mat );
    }
    
    //----------------------------------------------------------------------
    void ResourceManager::_DeleteTexture( Graphics::Texture* tex )
    {
#if PRINT_DELETES
        LOG( "DELETING TEXTURE", Color::RED );
#endif
        m_textures.erase( std::remove( m_textures.begin(), m_textures.end(), tex ) );
        SAFE_DELETE( tex );
    }

    //----------------------------------------------------------------------
    void ResourceManager::_DeleteRenderTexture( Graphics::RenderTexture* tex )
    {
#if PRINT_DELETES
        LOG( "DELETING TEXTURE", Color::RED );
#endif
        m_renderTextures.erase( std::remove( m_renderTextures.begin(), m_renderTextures.end(), tex ) );
        SAFE_DELETE( tex );
    }

    //----------------------------------------------------------------------
    void ResourceManager::_DeleteShader( Graphics::Shader* shader )
    {
#if PRINT_DELETES
        LOG( "DELETING SHADER " + shader->getName() );
#endif
        m_shaders.erase( std::remove( m_shaders.begin(), m_shaders.end(), shader ) );
        SAFE_DELETE( shader );
    }

    //----------------------------------------------------------------------
    void ResourceManager::_DeleteAudioClip( Audio::AudioClip* clip )
    {
        #if PRINT_DELETES
            LOG( "DELETING AudioClip", Color::RED );
        #endif
        m_audioClips.erase( std::remove( m_audioClips.begin(), m_audioClips.end(), clip ) );
        SAFE_DELETE( clip );
    }

    //----------------------------------------------------------------------
    void ResourceManager::_OnWindowSizeChanged()
    {
        auto& window = Locator::getWindow();
        if (window.getWidth() == 0 || window.getHeight() == 0)
            return;

        for (auto& texture : m_renderTextures)
        {
            auto renderTexture = dynamic_cast<Graphics::IRenderTexture*>( texture );
            if (renderTexture)
                if ( renderTexture->dynamicScales() )
                    renderTexture->recreate( window.getWidth(), window.getHeight() );
        }
    }

} } // end namespaces