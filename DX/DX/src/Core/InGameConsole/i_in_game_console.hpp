#pragma once
/**********************************************************************
    class: IInGameConsole (i_in_game_console.hpp)

    author: S. Hau
    date: November 19, 2017

    Interface for an In-Game Console.
    Register a function within an object with std::bind():
     IGC_REGISTER_COMMAND( std::bind( &Class::Function, pObject ) );
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "Core/Input/listener/input_listener.h"
#include "Core/Misc/variant_type.h"

//----------------------------------------------------------------------
#define IGC_REGISTER_COMMAND(F)                 Locator::getInGameConsole().registerCommand( #F, F )
#define IGC_REGISTER_COMMAND_WITH_NAME(NAME, F) Locator::getInGameConsole().registerCommand( NAME, F )

#define IGC_SET_VAR(NAME, V)                    Locator::getInGameConsole().setVariable( NAME, V )
#define IGC_GET_VAR(NAME)                       Locator::getInGameConsole().getVariable( NAME )

namespace Core {

    //----------------------------------------------------------------------
    #define CONSOLE_OPEN_KEY Key::OEM5 // '^'

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
        // Register a new command for the in game console.
        // @Params:
        //  "name": The name of the command
        //  "function": The function to invoke
        //----------------------------------------------------------------------
        virtual void registerCommand(const char* name, const std::function<void()>& function) = 0;

        //----------------------------------------------------------------------
        // Register a new variable in the in game console.
        // @Params:
        //  "name": The name of the variable
        //  "variable": The variable itself
        //----------------------------------------------------------------------
        virtual void setVariable(const char* name, const VariantType& var) = 0;

        //----------------------------------------------------------------------
        // Return the value of an console variable.
        // @Params:
        //  "name": The name of the variable
        // @Return:
        //  The variable as the type T.
        //----------------------------------------------------------------------
        virtual VariantType getVariable(const char* name) = 0;

        //----------------------------------------------------------------------
        // Print all commands
        //----------------------------------------------------------------------
        virtual void printAllCommands() {}

    protected:
        bool m_enabled = false;
        HashMap<StringID, std::function<void()>>    m_commands;
        HashMap<StringID, VariantType>              m_vars;

    private:
        //----------------------------------------------------------------------
        IInGameConsole(const IInGameConsole& other)               = delete;
        IInGameConsole& operator = (const IInGameConsole& other)  = delete;
        IInGameConsole(IInGameConsole&& other)                    = delete;
        IInGameConsole& operator = (IInGameConsole&& other)       = delete;
    };

} // End namespaces