#pragma once
/**********************************************************************
    class: Several (gpu_commands.hpp)

    author: S. Hau
    date: December 19, 2017

    - Contains definitions for all supported gpu commands, which can
      be put into a command buffer.
**********************************************************************/

namespace Graphics {

    //----------------------------------------------------------------------
    enum class GPUCommand
    {
        UNKNOWN,
        DRAW_MESH,
        SET_RENDER_TARGET,
        SET_CAMERA_PERSPECTIVE,
        SET_CAMERA_ORTHO
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
    struct GPUC_SetCameraPerspective : public GPUCommandBase
    {
    public:
        GPUC_SetCameraPerspective( const DirectX::XMMATRIX& _view, F32 _fov, F32 _zNear, F32 _zFar ) 
            : GPUCommandBase( GPUCommand::SET_CAMERA_PERSPECTIVE ), 
            view( _view ), fov( _fov ), zNear( _zNear ), zFar( _zFar ) {}

        DirectX::XMMATRIX   view;
        F32                 fov;
        F32                 zNear;
        F32                 zFar;
    };

    //**********************************************************************
    struct GPUC_SetCameraOrtho : public GPUCommandBase
    {
    public:
        GPUC_SetCameraOrtho( const DirectX::XMMATRIX& _view, F32 _left, F32 _right, F32 _bottom, F32 _top, F32 _zNear, F32 _zFar )
            : GPUCommandBase( GPUCommand::SET_CAMERA_ORTHO ), 
            view( _view ), left( _left ), right( _right ), bottom( _bottom ), top( _top ), zNear( _zNear ), zFar( _zFar ) {}

        DirectX::XMMATRIX view;
        F32 left;
        F32 right;
        F32 bottom;
        F32 top;
        F32 zNear;
        F32 zFar;
    };

} // End namespaces