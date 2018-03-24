#include "resource_manager.h"

/**********************************************************************
    class: ResourceManager (resource_manager.cpp)

    author: S. Hau
    date: December 25, 2017

**********************************************************************/

#include "locator.h"
#include "default_shader.hpp"

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

        if ( not shader->compileFromFile( vertPath, fragPath, "main" ) )
            return m_defaultShader;

        m_shaders.push_back( shader );

        return shader;
    }

    //----------------------------------------------------------------------
    void ResourceManager::_CreateDefaultAssets()
    {
        // SHADERS
        {
            // Default shader
            m_defaultShader = Locator::getRenderer().createShader();
            m_defaultShader->setName( SHADER_DEFAULT_NAME );

            if ( not m_defaultShader->compileFromSource( DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SOURCE, "main" ) )
                ERROR( "Default shader source didn't compile. This is mandatory!" );

            m_shaders.push_back( m_defaultShader );

            // Default wireframe shader
            m_wireframeShader = Locator::getRenderer().createShader();
            m_wireframeShader->setName( SHADER_WIREFRAME_NAME );
            m_wireframeShader->setRasterizationState( { Graphics::FillMode::WIREFRAME } );
            m_wireframeShader->compileFromSource( DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SOURCE, "main" );

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