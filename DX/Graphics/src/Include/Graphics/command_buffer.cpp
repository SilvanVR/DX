#include "command_buffer.h"
/**********************************************************************
    class: CommandBuffer (command_buffer.cpp)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

namespace Graphics {

    //----------------------------------------------------------------------
    CommandBuffer::CommandBuffer()
    {
        m_gpuCommands.reserve( COMMAND_BUFFER_INITIAL_CAPACITY );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::merge( const CommandBuffer& cmd )
    {
        auto& commands = cmd.getGPUCommands();
        m_gpuCommands.insert( m_gpuCommands.end(), commands.begin(), commands.end() );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::reset()
    {
        m_gpuCommands.clear();
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawMesh( MeshPtr mesh, MaterialPtr material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawMesh>( mesh, material, modelMatrix, subMeshIndex ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setCamera( Camera* camera )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetCamera>( camera ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::copyTexture( ITexture* srcTex, ITexture* dstTex )
    {
        copyTexture( srcTex, 0, 0, dstTex, 0, 0 );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::copyTexture( ITexture* srcTex, I32 srcElement, I32 srcMip, ITexture* dstTex, I32 dstElement, I32 dstMip )
    {
        ASSERT( srcTex->getWidth() == dstTex->getWidth() && srcTex->getHeight() == dstTex->getHeight() && "Textures must be of same size" );
        m_gpuCommands.push_back( std::make_unique<GPUC_CopyTexture>( srcTex, srcElement, srcMip, dstTex, dstElement, dstMip ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setGlobalFloat( StringID name, F32 value )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetGlobalFloat>( name, value ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setGlobalInt( StringID name, I32 value )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetGlobalInt>( name, value ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setGlobalVector4( StringID name, const Math::Vec4& vec4 )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetGlobalVector>( name, vec4 ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setGlobalColor( StringID name, Color color )
    {
        auto normalized = color.normalized();
        Math::Vec4 vec( normalized[0], normalized[1], normalized[2], normalized[3] );
        setGlobalVector4( name, vec );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setGlobalMatrix( StringID name, const DirectX::XMMATRIX& matrix )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetGlobalMatrix>( name, matrix ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawLight( const Light* light )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawLight>( light ) );
    }


} // End namespaces