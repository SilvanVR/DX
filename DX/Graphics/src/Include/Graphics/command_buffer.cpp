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
    void CommandBuffer::drawMesh( const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawMesh>( mesh, material, modelMatrix, subMeshIndex ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setCamera( Camera* camera )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetCamera>( camera ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::endCamera( Camera* camera )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_EndCamera>( camera ) );
    }

    ////----------------------------------------------------------------------
    //void CommandBuffer::copyTexture( const TexturePtr& srcTex, const TexturePtr& dstTex )
    //{
    //    copyTexture( srcTex, 0, 0, dstTex, 0, 0 );
    //}

    ////----------------------------------------------------------------------
    //void CommandBuffer::copyTexture( const TexturePtr& srcTex, I32 srcElement, I32 srcMip, const TexturePtr& dstTex, I32 dstElement, I32 dstMip )
    //{
    //    ASSERT( srcTex->getWidth() == dstTex->getWidth() && srcTex->getHeight() == dstTex->getHeight() && "Textures must be of same size" );
    //    m_gpuCommands.push_back( std::make_unique<GPUC_CopyTexture>( srcTex, srcElement, srcMip, dstTex, dstElement, dstMip ) );
    //}

    //----------------------------------------------------------------------
    void CommandBuffer::drawLight( const Light* light )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawLight>( light ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setRenderTarget( const RenderTexturePtr& target )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetRenderTarget>( target ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawFullscreenQuad( const MaterialPtr& material )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawFullscreenQuad>( material ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::renderCubemap( const CubemapPtr& cubemap, const MaterialPtr& material, I32 dstMip )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_RenderCubemap>( cubemap, material, dstMip ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::blit( const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_Blit>( src, dst, material ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setScissor( const Math::Rect& rect )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetScissor>( rect ) );
    }


} // End namespaces