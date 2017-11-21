#include "in_game_console.h"
/**********************************************************************
    class: InGameConsole (in_game_console.cpp)

    author: S. Hau
    date: November 19, 2017

    The SET-VARIABLE part is HIGHLY EXPERIMENTIALLY and will be
    replaced sometime in the future by a proper introspection system.
**********************************************************************/

#include "locator.h"
#include "Utils/string_utils.h"

namespace Core { 

    //----------------------------------------------------------------------
    #define CONSOLE_INFO_COLOR  Color::GREEN
    #define CONSOLE_COLOR       Color::VIOLET

    //----------------------------------------------------------------------
    void Quit()
    {
        Locator::getCoreEngine().terminate();
    }

    //----------------------------------------------------------------------
    void ProfileLog()
    {
        Locator::getProfiler().log();
    }

    //----------------------------------------------------------------------
    void MemoryStats()
    {
        Locator::getMemoryManager().log();
    }

    //----------------------------------------------------------------------
    void ToggleFullscreen()
    {
        static bool b = false;
        b = not b;
        Locator::getWindow().setBorderlessFullscreen( b );
    }

    //----------------------------------------------------------------------
    void Help()
    {
        LOG( "\nRegister commands with the macro 'IGC_REGISTER_COMMAND' or 'IGC_REGISTER_COMMAND_WITH_NAME'\n"
             "Then invoke commands just by the function name (or specified name).\n"
             "Register functions on objects with std::bind(). Example:\n"
             "IGC_REGISTER_COMMAND( std::bind( &Class::Function, pObject ) );", CONSOLE_INFO_COLOR );
    }

    //----------------------------------------------------------------------
    void InGameConsole::init()
    {
        IGC_REGISTER_COMMAND( Quit );
        IGC_REGISTER_COMMAND( ProfileLog );
        IGC_REGISTER_COMMAND( Help );
        IGC_REGISTER_COMMAND( MemoryStats );
        IGC_REGISTER_COMMAND_WITH_NAME( "fs", ToggleFullscreen );
    }

    //----------------------------------------------------------------------
    void InGameConsole::shutdown()
    {
    }

    //----------------------------------------------------------------------
    void InGameConsole::OnKeyPressed( Key key, KeyMod mod )
    {
        if ( key == CONSOLE_OPEN_KEY )
        {
            m_enabled = !m_enabled;
            if (m_enabled)
                LOG( " >>> Opening Console...", CONSOLE_INFO_COLOR );
            else
                LOG( " >>> Closing Console...", CONSOLE_INFO_COLOR );
        }
    }

    //----------------------------------------------------------------------
    void InGameConsole::OnChar( char c )
    {
        if ( not m_enabled || (c == '^') )
            return;

        switch (c)
        {
        case '\b':
            m_buffer.erase( 1 );
            break;
        case '\r':
            if ( !m_buffer.isEmpty() )
            {
                _ExecuteCommand( m_buffer.data() );
                m_buffer.clear();
            }
            break;
        default:
            if ( not m_buffer.isFull() )
                m_buffer.write( c );
        }

        // For now display on each new line
        if (m_buffer.size() > 0)
            LOG( m_buffer.data(), CONSOLE_COLOR );
    }

    //----------------------------------------------------------------------
    void InGameConsole::registerCommand( const char* name, const std::function<void()>& function )
    {
        String str = StringUtils::toLower( name );
        StringID id = SID( str.c_str() );
        if ( m_commands.count( id ) > 0 )
        {
            WARN( "InGameConsole::registerCommand(): Command '" + str + "' already exists." );
            return;
        }

        m_commands[id] = function;
    }

    //----------------------------------------------------------------------
    void InGameConsole::setVariable( const char* name, const VariantType& var )
    {
        String lower = StringUtils::toLower( name );
        StringID id = SID( lower.c_str() );
        m_vars[ id ] = var;
    }

    //----------------------------------------------------------------------
    VariantType InGameConsole::getVariable( const char* name )
    {
        String lower = StringUtils::toLower( name );
        StringID sid = SID_NO_ADD( lower.c_str() );
        if ( m_vars.count( sid ) != 0 )
        {
            return m_vars[sid];
        }
        else
        {
            WARN( "InGameConsole::getVariable(): Variable '" + lower + "' does not exist. Use SET_VAR(name,val) before." );
            return VariantType();
        }
    }

    //*********************************************************************
    // PRIVATE
    //*********************************************************************

    //----------------------------------------------------------------------
    void InGameConsole::_ExecuteCommand( const char* command )
    {
        String str = command;
        LOG( " >>> Execute command '" + str + "'...", CONSOLE_INFO_COLOR );

        StringUtils::IStringStream ss(str);
        String cmd;
        ss >> cmd;
        cmd = StringUtils::toLower( cmd );

        String rest;
        ss >> rest;
        if ( not rest.empty() )
        {
            auto splits = StringUtils::splitString( rest, '=' );
            if ( cmd == "print" )
            {
                String lower = StringUtils::toLower( splits[0] );
                StringID sid = SID_NO_ADD( lower.c_str() );
                if (m_vars.count(sid) > 0)
                {
                    m_vars[sid].log();
                }
                else
                {
                    WARN( "Variable '" + splits[0] + "' does not exist." );
                }
                return;
            }
            if ( splits.size() == 2 )
            {
                _ExecuteSetVarCommand( splits[0], splits[1] );
                return;
            }
        }

        // Execute command if known
        StringID sid = SID_NO_ADD( cmd.c_str() );
        if ( m_commands.count( sid ) != 0 )
        {
            m_commands[sid]();
        }
        else
        {
            // Command is not known.
            LOG( " >>> Command '" + str + "' is unknown.", CONSOLE_INFO_COLOR );
        }
    }

    //----------------------------------------------------------------------
    void InGameConsole::_ExecuteSetVarCommand( const String& name, const String& var )
    {
        String lower = StringUtils::toLower( name );
        StringID sid = SID_NO_ADD( lower.c_str() );
        if ( m_vars.count( sid ) != 0 )
        {
            LOG( "Setting Var...", Color::RED );
            if (var == "true")
            {
                m_vars[sid] = VariantType( true );
            }
            else if (var == "false")
            {
                m_vars[sid] = VariantType( false );
            }
            else
            {
                m_vars[ sid ] = var;
            }
        }
        else
        {
            LOG( " >>> Variable '" + lower + "' does not exist. Adding it.", CONSOLE_INFO_COLOR );
            setVariable( lower.c_str(), var );
        }
    }

} 