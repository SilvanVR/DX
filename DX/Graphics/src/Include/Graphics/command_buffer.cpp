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
        //// Insert drawing command sorted by the material shaders renderqueue
        //for (auto it = m_gpuCommands.begin(); it != m_gpuCommands.end(); it++)
        //{
        //    if ((*it)->getType() == Graphics::GPUCommand::DRAW_MESH)
        //    {
        //        auto c = reinterpret_cast<GPUC_DrawMesh*>( (*it).get() );

        //        if ( material->getShader()->getRenderQueue() <= c->material->getShader()->getRenderQueue() )
        //        {
        //            m_gpuCommands.insert( it, std::make_unique<GPUC_DrawMesh>( mesh, material, modelMatrix, subMeshIndex) );
        //            return;
        //        }
        //    }
        //}

        m_gpuCommands.push_back( std::make_unique<GPUC_DrawMesh>( mesh, material, modelMatrix, subMeshIndex ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setRenderTarget( RenderTexturePtr renderTarget )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetRenderTarget>( renderTarget ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::clearRenderTarget( const Color& clearColor )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_ClearRenderTarget>( clearColor ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setCameraPerspective( const DirectX::XMMATRIX& view, F32 fov, F32 zNear, F32 zFar )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetCameraPerspective>( view, fov, zNear, zFar ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setCameraOrtho( const DirectX::XMMATRIX& view, F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetCameraOrtho>( view, left, right, bottom, top, zNear, zFar ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setViewport( const Graphics::ViewportRect& viewport )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetViewport>( viewport ) );
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

} // End namespaces