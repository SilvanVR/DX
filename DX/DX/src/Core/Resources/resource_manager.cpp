#include "resource_manager.h"

/**********************************************************************
    class: ResourceManager (resource_manager.cpp)

    author: S. Hau
    date: December 25, 2017

    @Considerations:
      - Different deallocation scheme, e.g. delete after 5sec unused.
**********************************************************************/

#include "locator.h"
#include "default_shaders.hpp"
#include "Assets/importer.h"

#define PRINT_DELETES 0

namespace Core { namespace Resources {

    //----------------------------------------------------------------------
    void ResourceManager::init()
    {
        Locator::getCoreEngine().subscribe( this );

        _CreateDefaultAssets();

        // HOT-RELOADING CALLBACK
        Locator::getEngineClock().setInterval([this]{

            // Shader reloading
            for (auto& shader : m_shaders)
            {
                if ( not shader->isUpToDate() )
                {
                    auto shaderPaths = shader->recompile();

                    if ( not shaderPaths.empty() )
                    {
                        LOG( "ResourceManager: Successfully recompiled shader:", Color::YELLOW );
                        for ( auto& shaderPath : shaderPaths )
                            LOG( shaderPath.toString(), Color::YELLOW );
                    }
                }
            }

            // Texture reloading
            for (auto& pair : m_textureFileInfo)
            {
                auto& texture   = pair.first;
                auto& fileInfo  = pair.second;

                auto currentFileTime = fileInfo.path.getLastWrittenFileTime();
                if ( fileInfo.timeAtLoad != currentFileTime )
                {
                    // Reload texture
                    LOG( "Reloading texture: " + fileInfo.path.toString() );
                    //texture->setPixels(nullptr);

                    fileInfo.timeAtLoad = currentFileTime;
                }
            }

        }, 500);
    }

    //----------------------------------------------------------------------
    void ResourceManager::shutdown()
    {
        // Must be deleted here, cause the shared_ptr can be deconstructer after their containing arraylists, which crashes the program
        m_defaultShader.reset();
        m_errorShader.reset();
        m_wireframeShader.reset();
        m_defaultMaterial.reset();
        m_wireframeMaterial.reset();
        m_black.reset();
        m_white.reset();
    }

    //----------------------------------------------------------------------
    void ResourceManager::OnTick( Time::Seconds delta )
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
    MaterialPtr ResourceManager::createMaterial( ShaderPtr shader )
    {
        auto material = Locator::getRenderer().createMaterial();
        material->setShader( shader ? shader : m_defaultShader );

        m_materials.push_back( material );

        return MaterialPtr( material, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteMaterial ) );
    }

    //----------------------------------------------------------------------
    ShaderPtr ResourceManager::createShader( CString name, const OS::Path& vertPath, const OS::Path& fragPath )
    {
        auto shader = Locator::getRenderer().createShader();
        shader->setName( name );

        m_shaders.push_back( shader );

        if ( not shader->compileFromFile( vertPath, fragPath, "main" ) )
            return m_errorShader;

        return  ShaderPtr( shader, BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteShader ) );
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
    Texture2DPtr ResourceManager::getTexture2D( const OS::Path& path, bool genMips )
    {
        StringID pathAsID = SID( path.c_str() );
        if ( m_textureCache.find( pathAsID ) != m_textureCache.end() )
        {
            auto weakPtr = m_textureCache[pathAsID];

            if ( not weakPtr.expired() )
            {
                LOG( "Texture '" + path.toString() + "' already loaded!" );
                return Texture2DPtr( weakPtr );
            }
        }

        auto tex = Assets::Importer::LoadTexture( path, genMips );

        //@TODO: handle load failure
        m_textureCache[pathAsID] = tex;

        FileInfo fileInfo;
        fileInfo.path       = path;
        fileInfo.timeAtLoad = path.getLastWrittenFileTime();
        m_textureFileInfo[tex.get()] = fileInfo;

        return tex;
    }

    //----------------------------------------------------------------------
    CubemapPtr ResourceManager::getCubemap( const OS::Path& posX, const OS::Path& negX,
                                            const OS::Path& posY, const OS::Path& negY,
                                            const OS::Path& posZ, const OS::Path& negZ, bool generateMips )
    {
        auto tex = Assets::Importer::LoadCubemap( posX, negX, posY, negY, posZ, negZ, generateMips );
        return tex;
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
        Graphics::Texture2D* tex2D = dynamic_cast<Graphics::Texture2D*>( tex );
        if (tex2D)
        {
            //@TODO: remove texture resource from arr
            if ( m_textureFileInfo.find(tex2D) != m_textureFileInfo.end() )
                LOG( "DELETING TEXTURE " + m_textureFileInfo[tex2D].path.toString(), Color::RED );
            else
                LOG("DELETING TEXTURE", Color::RED);
        }
        else
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
    void ResourceManager::_CreateDefaultAssets()
    {
        // SHADERS
        {
            // Error shader
            m_errorShader = ShaderPtr( Locator::getRenderer().createShader(), BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteShader ) );
            m_errorShader->setName( SHADER_ERROR_NAME );
            if ( not m_errorShader->compileFromSource( ERROR_VERTEX_SHADER_SOURCE, ERROR_FRAGMENT_SHADER_SOURCE, "main" ) )
                ERROR( "Error shader source didn't compile. This is mandatory!" );

            m_shaders.push_back( m_errorShader.get() );

            // Default shader
            m_defaultShader = ShaderPtr( Locator::getRenderer().createShader(), BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteShader ) );
            m_defaultShader->setName( SHADER_DEFAULT_NAME );

            if ( not m_defaultShader->compileFromSource( DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SHADER_SOURCE, "main" ) )
                ERROR( "Default shader source didn't compile. This is mandatory!" );

            m_shaders.push_back( m_defaultShader.get() );

            // Default wireframe shader
            m_wireframeShader = ShaderPtr( Locator::getRenderer().createShader(), BIND_THIS_FUNC_1_ARGS( &ResourceManager::_DeleteShader ) );
            m_wireframeShader->setName( SHADER_WIREFRAME_NAME );
            m_wireframeShader->setRasterizationState( { Graphics::FillMode::Wireframe } );
            m_wireframeShader->compileFromSource( DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SHADER_SOURCE, "main" );

            m_shaders.push_back( m_wireframeShader.get() );
        }

        // MATERIALS
        {
            m_defaultMaterial   = createMaterial( m_defaultShader );
            m_wireframeMaterial = createMaterial( m_wireframeShader );

            Locator::getRenderer().addGlobalMaterial( "Wireframe", m_wireframeMaterial );
        }

        // TEXTURES
        {
            Color blacks[4] = { Color::BLACK, Color::BLACK, Color::BLACK, Color::BLACK };
            m_black = createTexture2D( 2, 2, Graphics::TextureFormat::RGBA32, blacks );

            Color whites[4] = { Color::WHITE, Color::WHITE, Color::WHITE, Color::WHITE };
            m_white = createTexture2D(2, 2, Graphics::TextureFormat::RGBA32, whites);
        }
    }

} } // end namespaces