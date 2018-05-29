#include "D3D11ConstantBufferManager.h"
/**********************************************************************
    class: ConstantBufferManager (D3D11ConstantBufferManager.cpp)

    author: S. Hau
    date: May 12, 2018
**********************************************************************/

#include "Common/string_utils.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    MappedConstantBuffer* ConstantBufferManager::s_pBufferGlobal = nullptr;
    MappedConstantBuffer* ConstantBufferManager::s_pBufferObject = nullptr;
    MappedConstantBuffer* ConstantBufferManager::s_pBufferCamera = nullptr;
    MappedConstantBuffer* ConstantBufferManager::s_pBufferLights = nullptr;

    //----------------------------------------------------------------------
    void ConstantBufferManager::Destroy()
    {
        SAFE_DELETE( s_pBufferCamera );
        SAFE_DELETE( s_pBufferObject );
        SAFE_DELETE( s_pBufferGlobal );
        SAFE_DELETE( s_pBufferLights );
    }

    //----------------------------------------------------------------------
    void ConstantBufferManager::ReflectConstantBuffers( const ArrayList<ShaderUniformBufferDeclaration>& constantBuffers )
    {
        for ( const auto& cb : constantBuffers )
        {
            String lower = StringUtils::toLower( cb.getName().toString() );

            if (lower.find( GLOBAL_BUFFER_KEYWORD ) != String::npos)
            {
                if (s_pBufferGlobal == nullptr)
                {
                    s_pBufferGlobal = new MappedConstantBuffer( cb, BufferUsage::Frequently );
                    s_pBufferGlobal->bind( ShaderType::Vertex );
                    s_pBufferGlobal->bind( ShaderType::Fragment );
                }
                else
                {
                    // Check that constant buffer matches the other one
                    if ( s_pBufferGlobal->getBufferInfo() != cb )
                        LOG_WARN_RENDERING( "Global buffer mismatch in recent compiled shader. This might cause errors during shading. "
                                            "Please ensure that every global buffer has the exact same layout and binding slot." );
                }
            }
            else if (lower.find( OBJECT_BUFFER_KEYWORD ) != String::npos)
            {
                if (s_pBufferObject == nullptr)
                {
                    s_pBufferObject = new MappedConstantBuffer( cb, BufferUsage::Frequently );
                    s_pBufferObject->bind( ShaderType::Vertex );
                }
                else
                {
                    // Check that constant buffer matches the other one
                    if ( s_pBufferObject->getBufferInfo() != cb )
                        LOG_WARN_RENDERING( "Object buffer mismatch in recent compiled shader. This might cause errors during shading. "
                                            "Please ensure that every object buffer has the exact same layout and binding slot." );
                }
            }
            else if (lower.find( CAMERA_BUFFER_KEYWORD ) != String::npos)
            {
                if (s_pBufferCamera == nullptr)
                {
                    s_pBufferCamera = new MappedConstantBuffer( cb, BufferUsage::Frequently );
                    s_pBufferCamera->bind( ShaderType::Vertex );
                    s_pBufferCamera->bind( ShaderType::Fragment );
                }
                else
                {
                    // Check that constant buffer matches the other one
                    if ( s_pBufferCamera->getBufferInfo() != cb )
                        LOG_WARN_RENDERING( "Camera buffer mismatch in recent compiled shader. This might cause errors during shading. "
                                            "Please ensure that every camera buffer has the exact same layout and binding slot." );
                }
            }
            else if (lower.find( LIGHT_BUFFER_KEYWORD ) != String::npos)
            {
                if (s_pBufferLights == nullptr)
                {
                    s_pBufferLights = new MappedConstantBuffer( cb, BufferUsage::Frequently );
                    s_pBufferLights->bind( ShaderType::Fragment );
                }
                else
                {
                    // Check that constant buffer matches the other one
                    if ( s_pBufferLights->getBufferInfo() != cb )
                        LOG_WARN_RENDERING( "Light buffer mismatch in recent compiled shader. This might cause errors during shading. "
                                            "Please ensure that every light buffer has the exact same layout and binding slot." );
                }
            }
            else
            {
                // Constant buffer might be a material buffer or sth else
            }
        }

    }


} } // End namespaces