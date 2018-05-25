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
    void CommandBuffer::drawLight( const Light* light )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawLight>( light ) );
    }


} // End namespaces