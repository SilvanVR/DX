#pragma once
/**********************************************************************
    class: InGameConsole (in_game_console.h)

    author: S. Hau
    date: November 19, 2017

    In-Game Console for executing commands at runtime.

    @Considerations:
      - Register/Call arbitrary functions
      - Use 2D Renderer to display console on screen
**********************************************************************/

#include "i_in_game_console.hpp"
#include "Core/DataStructures/byte_array.hpp"

namespace Core {

    //----------------------------------------------------------------------
    #define IGC_BUFFER_SIZE 1024

    //*********************************************************************
    class InGameConsole 
        : public IInGameConsole
    {
    public:
        InGameConsole() = default;
        ~InGameConsole() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // IKeyListener Interface
        //----------------------------------------------------------------------
        void OnKeyPressed(Key key, KeyMod mod) override;
        void OnChar(char c) override;

        //----------------------------------------------------------------------
        void registerCommand(const char* name, const std::function<void()>& function) override;

    private:
        CharArray<IGC_BUFFER_SIZE> m_buffer;

        //----------------------------------------------------------------------
        void _ExecuteCommand(const char* command);

        //----------------------------------------------------------------------
        InGameConsole(const InGameConsole& other)               = delete;
        InGameConsole& operator = (const InGameConsole& other)  = delete;
        InGameConsole(InGameConsole&& other)                    = delete;
        InGameConsole& operator = (InGameConsole&& other)       = delete;
    };


} // End namespaces