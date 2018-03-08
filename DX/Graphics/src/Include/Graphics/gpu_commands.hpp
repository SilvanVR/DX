#pragma once
/**********************************************************************
    class: Several (gpu_commands.hpp)

    author: S. Hau
    date: December 19, 2017

    - Contains definitions for all supported gpu commands, which can
      be put into a command buffer.
**********************************************************************/

#include "structs.hpp"

namespace Graphics {

    class RenderTexture;

    //----------------------------------------------------------------------
    enum class GPUCommand
    {
        UNKNOWN,
        DRAW_MESH,
        SET_RENDER_TARGET,
        SET_CAMERA_PERSPECTIVE,
        SET_CAMERA_ORTHO,
        SET_VIEWPORT,
        CLEAR_RENDER_TARGET
    };

    //**********************************************************************
    struct GPUCommandBase
    {
    public:
        GPUCommandBase( GPUCommand type ) : m_type( type ) {}
        virtual ~GPUCommandBase() = default;

        //----------------------------------------------------------------------
        GPUCommand getType() const { return m_type; }

    private:
        GPUCommand m_type = GPUCommand::UNKNOWN;
    };

    //**********************************************************************
    struct GPUC_DrawMesh : public GPUCommandBase
    {
        GPUC_DrawMesh( const DirectX::XMMATRIX& modelMatrix )
            : GPUCommandBase( GPUCommand::DRAW_MESH ), modelMatrix( modelMatrix ) {}

        DirectX::XMMATRIX modelMatrix;
    };

    //**********************************************************************
    struct GPUC_SetRenderTarget : public GPUCommandBase
    {
        GPUC_SetRenderTarget( Graphics::RenderTexture* renderTarget )
            : GPUCommandBase( GPUCommand::SET_RENDER_TARGET ), 
            renderTarget( renderTarget ) {}

        Graphics::RenderTexture*    renderTarget;
    };

    //**********************************************************************
    struct GPUC_ClearRenderTarget : public GPUCommandBase
    {
        GPUC_ClearRenderTarget( const Color& clearColor )
            : GPUCommandBase( GPUCommand::CLEAR_RENDER_TARGET ), 
            clearColor( clearColor ) {}

        Color                       clearColor;
    };

    //**********************************************************************
    struct GPUC_SetCameraPerspective : public GPUCommandBase
    {
        GPUC_SetCameraPerspective( const DirectX::XMMATRIX& view, F32 fov, F32 zNear, F32 zFar ) 
            : GPUCommandBase( GPUCommand::SET_CAMERA_PERSPECTIVE ), 
            view( view ), fov( fov ), zNear( zNear ), zFar( zFar ) {}

        DirectX::XMMATRIX   view;
        F32                 fov;
        F32                 zNear;
        F32                 zFar;
    };

    //**********************************************************************
    struct GPUC_SetCameraOrtho : public GPUCommandBase
    {
        GPUC_SetCameraOrtho( const DirectX::XMMATRIX& view, F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar )
            : GPUCommandBase( GPUCommand::SET_CAMERA_ORTHO ), 
            view( view ), left( left ), right( right ), bottom( bottom ), top( top ), zNear( zNear ), zFar( zFar ) {}

        DirectX::XMMATRIX view;
        F32 left;
        F32 right;
        F32 bottom;
        F32 top;
        F32 zNear;
        F32 zFar;
    };

    //**********************************************************************
    struct GPUC_SetViewport : public GPUCommandBase
    {
        GPUC_SetViewport(const Graphics::ViewportRect& viewport)
            : GPUCommandBase(GPUCommand::SET_VIEWPORT),
              viewport( viewport ) {}

        Graphics::ViewportRect viewport;
    };

} // End namespaces