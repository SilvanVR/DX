#pragma once
/**********************************************************************
    class: IInGameConsole (i_in_game_console.hpp)

    author: S. Hau
    date: November 19, 2017

    Interface for an In-Game Console.
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "Core/Input/listener/input_listener.h"

//----------------------------------------------------------------------
#define IGC_REGISTER_COMMAND(F) Locator::getInGameConsole().registerCommand( #F, F );


namespace Core {

    //----------------------------------------------------------------------
    #define CONSOLE_OPEN_KEY Key::OEM5

    //*********************************************************************
    class IInGameConsole 
        : public ISubSystem, public Input::IKeyListener
    {
    public:
        IInGameConsole() = default;
        virtual ~IInGameConsole() = default;

        //----------------------------------------------------------------------
        // IKeyListener Interface
        //----------------------------------------------------------------------
        virtual void OnKeyPressed(Key key, KeyMod mod) = 0;
        virtual void OnChar(char c) = 0;

        //----------------------------------------------------------------------
        virtual void registerCommand(const char* name, const std::function<void()>& function) = 0;

    protected:
        bool m_enabled = false;
        HashMap<StringID, std::function<void()>> m_commands;

    private:
        //----------------------------------------------------------------------
        IInGameConsole(const IInGameConsole& other)               = delete;
        IInGameConsole& operator = (const IInGameConsole& other)  = delete;
        IInGameConsole(IInGameConsole&& other)                    = delete;
        IInGameConsole& operator = (IInGameConsole&& other)       = delete;
    };


} // End namespaces