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
    void CommandBuffer::reset()
    {
        m_gpuCommands.clear();
        m_gpuCommands.reserve( COMMAND_BUFFER_INITIAL_CAPACITY );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::drawMesh( const DirectX::XMMATRIX& model, Graphics::Mesh* mesh, I32 subMeshIndex )
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_DrawMesh>( model, mesh, subMeshIndex ) );
    }

    //----------------------------------------------------------------------
    void CommandBuffer::setRenderTarget( Graphics::RenderTexture* renderTarget )
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
    void CommandBuffer::setViewport(const Graphics::ViewportRect& viewport)
    {
        m_gpuCommands.push_back( std::make_unique<GPUC_SetViewport>( viewport ) );
    }

} // End namespaces