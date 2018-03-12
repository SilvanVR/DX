#pragma once
/**********************************************************************
    class: GraphicsCommandRecorder (graphics_recorder.h)

    author: S. Hau
    date: December 24, 2017

    - Responsible for recording all rendering commands from a scene
      and sending them to the renderer once per frame
**********************************************************************/

//----------------------------------------------------------------------
class IScene;
namespace Graphics { class CommandBuffer; }

namespace Core {

    //**********************************************************************
    class GraphicsCommandRecorder
    {
    public:
        GraphicsCommandRecorder() = default;
        ~GraphicsCommandRecorder() = default;

        //----------------------------------------------------------------------
        void init();
        void shutdown();
        void dispatch( IScene& scene, F32 lerp );

    private:

        //----------------------------------------------------------------------
        GraphicsCommandRecorder(const GraphicsCommandRecorder& other)               = delete;
        GraphicsCommandRecorder& operator = (const GraphicsCommandRecorder& other)  = delete;
        GraphicsCommandRecorder(GraphicsCommandRecorder&& other)                    = delete;
        GraphicsCommandRecorder& operator = (GraphicsCommandRecorder&& other)       = delete;
    };

}
