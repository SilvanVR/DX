#pragma once
/**********************************************************************
    class: InGameConsole (in_game_console.h)

    author: S. Hau
    date: November 19, 2017

    In-Game Console for executing commands at runtime.
**********************************************************************/

#include "i_in_game_console.hpp"
#include "Core/DataStructures/byte_array.hpp"

namespace Core {

    //----------------------------------------------------------------------
    #define IGC_BUFFER_SIZE 64

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
        void init(CoreEngine* coreEngine) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // IKeyListener Interface
        //----------------------------------------------------------------------
        void OnKeyPressed(Key key, KeyMod mod) override;
        void OnChar(char c) override;

        //----------------------------------------------------------------------
        // IInGameConsole Interface
        //----------------------------------------------------------------------
        void registerCommand(const char* name, const std::function<void()>& function) override;
        void setVariable(const char* name, const VariantType& var) override;
        VariantType getVariable(const char* name) override;

    private:
        CharArray<IGC_BUFFER_SIZE> m_buffer;

        //----------------------------------------------------------------------
        void _ExecuteCommand(const char* command);
        void _ExecuteSetVarCommand(const String& name, const String& var);

        //----------------------------------------------------------------------
        InGameConsole(const InGameConsole& other)               = delete;
        InGameConsole& operator = (const InGameConsole& other)  = delete;
        InGameConsole(InGameConsole&& other)                    = delete;
        InGameConsole& operator = (InGameConsole&& other)       = delete;
    };


} // End namespaces