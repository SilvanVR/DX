#pragma once
/**********************************************************************
    class: GraphicsCommandRecorder (graphics_recorder.h)

    author: S. Hau
    date: December 24, 2017

    - Responsible for recording all rendering commands from a scene
      and sending them to the renderer once per frame
**********************************************************************/

//----------------------------------------------------------------------
#define COMMAND_BUFFER_COUNT 2

//----------------------------------------------------------------------
class IScene;

namespace Core {

    //----------------------------------------------------------------------
    namespace Graphics { class CommandBuffer; }

    //**********************************************************************
    class GraphicsCommandRecorder
    {
    public:
        GraphicsCommandRecorder() = default;
        ~GraphicsCommandRecorder() = default;

        //----------------------------------------------------------------------
        void init();
        void shutdown();
        void render( IScene& scene, F32 lerp );

    private:
        Graphics::CommandBuffer* m_CommandBuffers[COMMAND_BUFFER_COUNT];

        //----------------------------------------------------------------------
        GraphicsCommandRecorder(const GraphicsCommandRecorder& other)               = delete;
        GraphicsCommandRecorder& operator = (const GraphicsCommandRecorder& other)  = delete;
        GraphicsCommandRecorder(GraphicsCommandRecorder&& other)                    = delete;
        GraphicsCommandRecorder& operator = (GraphicsCommandRecorder&& other)       = delete;
    };

}
