#pragma once
/**********************************************************************
    class: CommandBuffer (command_buffer.h)

    author: S. Hau
    date: December 19, 2017

    - Consists of arbitrary GPU commands
    - Can be passed to the renderer, who transform these calls to api
      dependant calls (and possibly do optimizations e.g. batch stuff)
**********************************************************************/

namespace Core { namespace Graphics {

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


    private:

        //----------------------------------------------------------------------
        friend class D3D11Renderer;


        //----------------------------------------------------------------------
        CommandBuffer(const CommandBuffer& other)               = delete;
        CommandBuffer& operator = (const CommandBuffer& other)  = delete;
        CommandBuffer(CommandBuffer&& other)                    = delete;
        CommandBuffer& operator = (CommandBuffer&& other)       = delete;
    };

} } // End namespaces