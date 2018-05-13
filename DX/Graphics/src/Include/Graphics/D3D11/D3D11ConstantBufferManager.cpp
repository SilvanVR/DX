#include "D3D11ConstantBufferManager.h"
/**********************************************************************
    class: ConstantBufferManager (D3D11ConstantBufferManager.cpp)

    author: S. Hau
    date: May 12, 2018
**********************************************************************/

#include "Common/string_utils.h"

namespace Graphics { namespace D3D11 {

    //----------------------------------------------------------------------
    MappedConstantBuffer* ConstantBufferManager::s_pConstantBufferGlobal = nullptr;
    MappedConstantBuffer* ConstantBufferManager::s_pConstantBufferObject = nullptr;
    MappedConstantBuffer* ConstantBufferManager::s_pConstantBufferCamera = nullptr;

    //----------------------------------------------------------------------
    void ConstantBufferManager::Destroy()
    {
        SAFE_DELETE( s_pConstantBufferCamera );
        SAFE_DELETE( s_pConstantBufferObject );
        SAFE_DELETE( s_pConstantBufferGlobal );
    }

    //----------------------------------------------------------------------
    void ConstantBufferManager::ReflectConstantBuffers( const HashMap<StringID, ConstantBufferInfo>& constantBuffers )
    {
        for (const auto& [name, bufferInfo] : constantBuffers)
        {
            String lower = StringUtils::toLower( name.toString() );

            if (lower.find( GLOBAL_BUFFER_KEYWORD ) != String::npos)
            {
                if (s_pConstantBufferGlobal == nullptr)
                {
                    s_pConstantBufferGlobal = new MappedConstantBuffer( bufferInfo, BufferUsage::Frequently );
                    s_pConstantBufferGlobal->bind( ShaderType::Vertex );
                    s_pConstantBufferGlobal->bind( ShaderType::Fragment );
                }
                else
                {
                    // Check that constant buffer matches the other one
                    if ( s_pConstantBufferGlobal->getBufferInfo() != bufferInfo )
                        LOG_WARN_RENDERING( "Global buffer mismatch in recent compiled shader. This might cause errors during shading. "
                                            "Please ensure that every global buffer has the exact same layout and binding slot." );
                }
            }
            else if (lower.find( OBJECT_BUFFER_KEYWORD ) != String::npos)
            {
                if (s_pConstantBufferObject == nullptr)
                {
                    s_pConstantBufferObject = new MappedConstantBuffer( bufferInfo, BufferUsage::Frequently );
                    s_pConstantBufferObject->bind( ShaderType::Vertex );
                }
                else
                {
                    // Check that constant buffer matches the other one
                    if ( s_pConstantBufferObject->getBufferInfo() != bufferInfo )
                        LOG_WARN_RENDERING( "Object buffer mismatch in recent compiled shader. This might cause errors during shading. "
                                            "Please ensure that every object buffer has the exact same layout and binding slot." );
                }
            }
            else if (lower.find( CAMERA_BUFFER_KEYWORD ) != String::npos)
            {
                if (s_pConstantBufferCamera == nullptr)
                {
                    s_pConstantBufferCamera = new MappedConstantBuffer( bufferInfo, BufferUsage::Frequently );
                    s_pConstantBufferCamera->bind( ShaderType::Vertex );
                }
                else
                {
                    // Check that constant buffer matches the other one
                    if ( s_pConstantBufferCamera->getBufferInfo() != bufferInfo )
                        LOG_WARN_RENDERING( "Camera buffer mismatch in recent compiled shader. This might cause errors during shading. "
                                            "Please ensure that every camera buffer has the exact same layout and binding slot." );
                }
            }
            else
            {
                // Constant buffer might be a material buffer or sth else
            }
        }

    }


} } // End namespaces