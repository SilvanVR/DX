#pragma once
/**********************************************************************
    class: CommandBuffer (command_buffer.h)

    author: S. Hau
    date: December 19, 2017

    - Consists of arbitrary GPU commands
    - Can be passed to the renderer, who transform these calls to api
      dependant calls (and possibly do optimizations e.g. batch stuff)
**********************************************************************/

namespace Graphics { class RenderTexture; }

namespace Graphics {

    //**********************************************************************
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        ~CommandBuffer() = default;

        //----------------------------------------------------------------------
        void reset();

        //----------------------------------------------------------------------
        void drawMesh();
        void setRenderTarget(Graphics::RenderTexture* renderTarget, const Color& clearColor);
        void setCameraPerspective(const DirectX::XMMATRIX& view, F32 fov, F32 zNear, F32 zFar);
        void setCameraOrtho(const DirectX::XMMATRIX& view, F32 left, F32 right, F32 bottom, F32 top, F32 zNear, F32 zFar);

    private:

        //----------------------------------------------------------------------
        friend class D3D11Renderer;


        //----------------------------------------------------------------------
        CommandBuffer(const CommandBuffer& other)               = delete;
        CommandBuffer& operator = (const CommandBuffer& other)  = delete;
        CommandBuffer(CommandBuffer&& other)                    = delete;
        CommandBuffer& operator = (CommandBuffer&& other)       = delete;
    };

} // End namespaces