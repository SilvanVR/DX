#include "resource_manager.h"

/**********************************************************************
    class: ResourceManager (resource_manager.cpp)

    author: S. Hau
    date: December 25, 2017

    @Considerations:
      - Different deallocation scheme, e.g. delete after 5sec unused.
**********************************************************************/

#include "Core/locator.h"
#include "Graphics/default_shaders.hpp"
#include "Core/mesh_generator.h"

#define PRINT_DELETES 0

namespace Core { namespace Resources {

    #define HOT_RELOAD_INTERVAL_MILLIS  500

    //----------------------------------------------------------------------
    void ResourceManager::init()
    {
        _CreateDefaultAssets();
    }

    //----------------------------------------------------------------------
    void ResourceManager::shutdown()
    {
        // Must be deleted here, cause the shared_ptr can be deconstructed after their containing arraylists, which crashes the program
        m_defaultShader.reset();
        m_errorShader.reset();
        m_wireframeShader.reset();
        m_colorShader.reset();

        m_defaultMaterial.reset();
        m_wireframeMaterial.reset();
        m_colorMaterial.reset();

        m_black.reset();
        m_white.reset();

        m_defaultCubemap.reset();
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
        material->setShader( shader ? shader : m_defaultShader );

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

        m_textures.push_back( texture );

        return RenderTexturePtr( texture, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteTexture ) );
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
    void ResourceManager::_CreateDefaultAssets()
    {
        // SHADERS
        {
            // Error shader
            m_errorShader = ShaderPtr( Locator::getRenderer().createShader(), BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteShader ) );
            m_errorShader->setName( SHADER_ERROR_NAME );
            if ( not m_errorShader->compileFromSource( Graphics::ERROR_VERTEX_SHADER_SOURCE, Graphics::ERROR_FRAGMENT_SHADER_SOURCE, "main" ) )
                LOG_ERROR( "Error shader source didn't compile. This is mandatory!" );

            m_shaders.push_back( m_errorShader.get() );

            // Default shader
            m_defaultShader = ShaderPtr( Locator::getRenderer().createShader(), BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteShader ) );
            m_defaultShader->setName( SHADER_DEFAULT_NAME );

            if ( not m_defaultShader->compileFromSource( Graphics::DEFAULT_VERTEX_SHADER_SOURCE, Graphics::DEFAULT_FRAGMENT_SHADER_SOURCE, "main" ) )
                LOG_ERROR( "Default shader source didn't compile. This is mandatory!" );

            m_shaders.push_back( m_defaultShader.get() );

            // Default wireframe shader
            m_wireframeShader = ShaderPtr( Locator::getRenderer().createShader(), BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteShader ) );
            m_wireframeShader->setName( SHADER_WIREFRAME_NAME );
            m_wireframeShader->setRasterizationState( { Graphics::FillMode::Wireframe } );
            m_wireframeShader->compileFromSource( Graphics::DEFAULT_VERTEX_SHADER_SOURCE, Graphics::DEFAULT_FRAGMENT_SHADER_SOURCE, "main" );

            m_shaders.push_back( m_wireframeShader.get() );

            // Default shader
            m_colorShader = ShaderPtr( Locator::getRenderer().createShader(), BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteShader ) );
            m_colorShader->setName( SHADER_COLOR_NAME );

            if ( not m_colorShader->compileFromSource( Graphics::COLOR_VERTEX_SHADER_SOURCE, Graphics::COLOR_FRAGMENT_SHADER_SOURCE, "main") )
                LOG_ERROR( "Color shader source didn't compile. This is mandatory!" );

            m_shaders.push_back( m_colorShader.get() );
        }

        // MATERIALS
        {
            m_defaultMaterial   = createMaterial( m_defaultShader );
            m_defaultMaterial->setName( "Default Material" );
            m_wireframeMaterial = createMaterial( m_wireframeShader );
            m_wireframeMaterial->setName( "Wireframe Material" );
            m_colorMaterial = createMaterial( m_colorShader );
            m_colorMaterial->setName( "Color Material" );

            Locator::getRenderer().addGlobalMaterial( "Wireframe", m_wireframeMaterial.get() );
        }

        // TEXTURES
        {
            Color blacks[4] = { Color::BLACK, Color::BLACK, Color::BLACK, Color::BLACK };
            m_black = createTexture2D( 2, 2, Graphics::TextureFormat::BGRA32, blacks );

            Color whites[4] = { Color::WHITE, Color::WHITE, Color::WHITE, Color::WHITE };
            m_white = createTexture2D( 2, 2, Graphics::TextureFormat::BGRA32, whites );

            Color normalColor = Color( 128, 127, 255 );
            Color normals[4] = { normalColor, normalColor, normalColor, normalColor };
            m_normal = createTexture2D( 2, 2, Graphics::TextureFormat::BGRA32, normals );
        }

        // CUBEMAPS
        {
            const I32 size = 2;
            m_defaultCubemap = createCubemap();
            m_defaultCubemap->setFilter( Graphics::TextureFilter::Point );
            m_defaultCubemap->create( size, Graphics::TextureFormat::BGRA32 );

            Color colorsPerFace[6] = { Color::WHITE, Color::GREEN, Color::RED, Color::BLUE, Color::ORANGE, Color::VIOLET };
            for (int i = 0; i < 6; i++)
                m_defaultCubemap->setPixels( (Graphics::CubemapFace)i, ArrayList<Color>( size*size, colorsPerFace[i] ).data() );
            m_defaultCubemap->apply();
        }

        // Mesh
        {
            m_defaultMesh = MeshGenerator::CreatePlane( 1.0f, Color::RED );
        }
    }

} } // end namespaces