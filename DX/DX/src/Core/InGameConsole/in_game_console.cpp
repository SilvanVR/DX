#include "in_game_console.h"
/**********************************************************************
    class: InGameConsole (in_game_console.cpp)

    author: S. Hau
    date: November 19, 2017
**********************************************************************/

#include "locator.h"
#include "Utils/string_utils.h"

namespace Core { 

    void Test()
    {
        LOG( "TEST. ES GEHT!!!!!!!!!!!" );
    }

    void Quit()
    {
        Locator::getCoreEngine().terminate();
    }

    //----------------------------------------------------------------------
    #define CONSOLE_INFO_COLOR  Color::GREEN
    #define CONSOLE_COLOR       Color::VIOLET


    //----------------------------------------------------------------------
    void InGameConsole::init()
    {
        IGC_REGISTER_COMMAND( Test );
        IGC_REGISTER_COMMAND( Quit );
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
            {
                LOG( " >>> Opening Console...", CONSOLE_INFO_COLOR );
            }
            else
            {
                LOG( " >>> Closing Console...", CONSOLE_INFO_COLOR );
            }
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
            _ExecuteCommand( m_buffer.data() );
            m_buffer.clear();
            break;
        default:
            if ( not m_buffer.isFull() )
                m_buffer.write( c );
        }

        if (m_buffer.size() > 0)
            LOG( m_buffer.data(), CONSOLE_COLOR );
    }

    //----------------------------------------------------------------------
    void InGameConsole::registerCommand( const char* name, const std::function<void()>& function )
    {
        String str( name );
        StringUtils::toLower( str );

        StringID id = SID( str.c_str() );
        if ( m_commands.count( id ) > 0 )
        {
            WARN( "InGameConsole::registerCommand(): Command '" + str + "' already exists." );
            return;
        }

        m_commands[id] = function;
    }


    //*********************************************************************
    // PRIVATE
    //*********************************************************************

    //----------------------------------------------------------------------
    void InGameConsole::_ExecuteCommand( const char* command )
    {
        String str = command;
        StringUtils::toLower( str );

        LOG( " >>> Execute command '" + str + "'...", CONSOLE_INFO_COLOR );

        // Execute command if known
        StringID id = SID_NO_ADD( str.c_str() );
        if ( m_commands.count( id ) != 0 )
        {
            m_commands[id]();
        }
        else
        {
            // Command is not known.
            LOG( " >>> Command '" + str + "' is unknown", CONSOLE_INFO_COLOR );
        }
    }

} 