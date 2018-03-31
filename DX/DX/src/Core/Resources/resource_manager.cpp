#include "resource_manager.h"

/**********************************************************************
    class: ResourceManager (resource_manager.cpp)

    author: S. Hau
    date: December 25, 2017

**********************************************************************/

#include "locator.h"
#include "default_shaders.hpp"

namespace Core { namespace Resources {

    //----------------------------------------------------------------------
    void ResourceManager::init()
    {
        Locator::getCoreEngine().subscribe( this );

        _CreateDefaultAssets();

        // HOT-RELOADING CALLBACK
        Locator::getEngineClock().setInterval([this]{
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
        }, 500);
    }

    //----------------------------------------------------------------------
    void ResourceManager::shutdown()
    {
        for (auto& mesh : m_meshes)
            SAFE_DELETE( mesh );
        for (auto& material : m_materials)
            SAFE_DELETE( material );
        for (auto& shader : m_shaders)
            SAFE_DELETE( shader );
        for (auto& texture : m_textures)
            SAFE_DELETE( texture );
    }

    //----------------------------------------------------------------------
    void ResourceManager::OnTick( Time::Seconds delta )
    {
        
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    Graphics::Mesh* ResourceManager::createMesh()
    {
        auto mesh = Locator::getRenderer().createMesh();

        m_meshes.push_back( mesh );

        return mesh;
    }

    //----------------------------------------------------------------------
    Graphics::Mesh* ResourceManager::createMesh( const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices )
    {
        auto mesh = createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices );
        return mesh;
    }

    //----------------------------------------------------------------------
    Graphics::Mesh* ResourceManager::createMesh( const ArrayList<Math::Vec3>& vertices, const ArrayList<U32>& indices, const ArrayList<Math::Vec2>& uvs )
    {
        auto mesh = createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices );
        mesh->setUVs( uvs );
        return mesh;
    }

    //----------------------------------------------------------------------
    Graphics::Material* ResourceManager::createMaterial( Graphics::Shader* shader )
    {
        auto mat = Locator::getRenderer().createMaterial();
        mat->setShader( shader ? shader : m_defaultShader );

        m_materials.push_back( mat );

        return mat;
    }

    //----------------------------------------------------------------------
    Graphics::Shader* ResourceManager::createShader( CString name, const OS::Path& vertPath, const OS::Path& fragPath )
    {
        auto shader = Locator::getRenderer().createShader();
        shader->setName( name );

        m_shaders.push_back( shader );

        if ( not shader->compileFromFile( vertPath, fragPath, "main" ) )
            return m_errorShader;

        return shader;
    }

    //----------------------------------------------------------------------
    Graphics::Texture2D* ResourceManager::createTexture2D( U32 width, U32 height, Graphics::TextureFormat format, bool generateMips )
    {
        auto texture = Locator::getRenderer().createTexture2D();
        texture->create( width, height, format, generateMips );

        m_textures.push_back( texture );

        return texture;
    }

    //----------------------------------------------------------------------
    Graphics::Texture2D* ResourceManager::createTexture2D(U32 width, U32 height, Graphics::TextureFormat format, const void* pData)
    {
        auto texture = Locator::getRenderer().createTexture2D();
        texture->create( width, height, format, pData );

        m_textures.push_back(texture);

        return texture;
    }

    //----------------------------------------------------------------------
    void ResourceManager::setGlobalAnisotropicFiltering( U32 level )
    {
        for ( auto& tex : m_textures )
            tex->setAnisoLevel( level );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void ResourceManager::_CreateDefaultAssets()
    {
        // SHADERS
        {
            // Error shader
            m_errorShader = Locator::getRenderer().createShader();
            m_errorShader->setName( SHADER_ERROR_NAME );
            if ( not m_errorShader->compileFromSource( ERROR_VERTEX_SHADER_SOURCE, ERROR_FRAGMENT_SHADER_SOURCE, "main" ) )
                ERROR( "Error shader source didn't compile. This is mandatory!" );

            m_shaders.push_back( m_errorShader );

            // Default shader
            m_defaultShader = Locator::getRenderer().createShader();
            m_defaultShader->setName( SHADER_DEFAULT_NAME );

            if ( not m_defaultShader->compileFromSource( DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SHADER_SOURCE, "main" ) )
                ERROR( "Default shader source didn't compile. This is mandatory!" );

            m_shaders.push_back( m_defaultShader );

            // Default wireframe shader
            m_wireframeShader = Locator::getRenderer().createShader();
            m_wireframeShader->setName( SHADER_WIREFRAME_NAME );
            m_wireframeShader->setRasterizationState( { Graphics::FillMode::Wireframe } );
            m_wireframeShader->compileFromSource( DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SHADER_SOURCE, "main" );

            m_shaders.push_back( m_wireframeShader );
        }

        // MATERIALS
        {
            m_defaultMaterial   = createMaterial( m_defaultShader );
            m_wireframeMaterial = createMaterial( m_wireframeShader );

            Locator::getRenderer().addGlobalMaterial( "Wireframe", m_wireframeMaterial );
        }
    }

} } // end namespaces