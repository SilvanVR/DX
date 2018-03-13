#include "resource_manager.h"

/**********************************************************************
    class: ResourceManager (resource_manager.cpp)

    author: S. Hau
    date: December 25, 2017

**********************************************************************/

#include "locator.h"

namespace Core { namespace Resources {

    //----------------------------------------------------------------------
    void ResourceManager::init()
    {
        Locator::getCoreEngine().subscribe( this );

        m_defaultShader   = createShader( "/shaders/basicVS.hlsl", "/shaders/basicPS.hlsl" );
        m_defaultMaterial = createMaterial();

        Locator::getEngineClock().setInterval([this]{
            for (auto& shader : m_shaders)
            {
                if ( not shader->isUpToDate() )
                {
                    if ( shader->compile("main") )
                    {
                        LOG( "ResourceManager: Successfully recompiled shader:", Color::YELLOW );
                        for ( auto& shaderPath : shader->getShaderPaths() )
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
    Graphics::Material* ResourceManager::createMaterial()
    {
        auto mat = Locator::getRenderer().createMaterial();
        m_materials.push_back( mat );

        // Set default shader
        mat->setShader( m_defaultShader );

        return mat;
    }

    //----------------------------------------------------------------------
    Graphics::Shader* ResourceManager::createShader( CString vertPath, CString fragPath )
    {
        auto shader = Locator::getRenderer().createShader();
        m_shaders.push_back( shader );

        shader->setShaderPaths( vertPath, fragPath );

        if ( not shader->compile("main") )
        {
            return m_defaultShader;
        }

        return shader;
    }


} } // end namespaces