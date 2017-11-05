#include "input_manager.h"
/**********************************************************************
    class: InputManager (input_manager.cpp)

    author: S. Hau
    date: November 4, 2017
**********************************************************************/

#include "locator.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    static InputManager* s_instance = nullptr;

    //----------------------------------------------------------------------
    void KeyCallback( Key key, KeyAction action, KeyMod mod )        { s_instance->_KeyCallback( key, action, mod ); }
    void CharCallback( char c )                                      { s_instance->_CharCallback( c ); }
    void MouseCallback( MouseKey key, KeyAction action, KeyMod mod ) { s_instance->_MouseCallback( key, action, mod ); }
    void MouseWheelCallback( I16 param )                             { s_instance->_MouseWheelCallback( param ); }
    void CursorMovedCallback( I16 x, I16 y )                         { s_instance->_CursorMovedCallback( x, y ); }

    //----------------------------------------------------------------------
    void InputManager::init()
    {
       ASSERT( s_instance == nullptr );
       s_instance = this;

       // Subscribe to all events
       OS::Window& window = Locator::getWindow();
       window.setCallbackKey( KeyCallback );
       window.setCallbackChar( CharCallback );
       window.setCallbackMouseButtons( MouseCallback );
       window.setCallbackMouseWheel( MouseWheelCallback );
       window.setCallbackCursorMove( CursorMovedCallback );
    }

    //----------------------------------------------------------------------
    void InputManager::update(Time::Seconds delta)
    {

    }

    //----------------------------------------------------------------------
    void InputManager::shutdown()
    {
        // Unsubscribe to all events
        OS::Window& window = Locator::getWindow();
        window.setCallbackKey( nullptr );
        window.setCallbackChar( nullptr );
        window.setCallbackMouseButtons( nullptr );
        window.setCallbackMouseWheel( nullptr );
        window.setCallbackCursorMove( nullptr );
    }

    //----------------------------------------------------------------------
    void InputManager::_MouseCallback( MouseKey key, KeyAction action, KeyMod mod )
    {
          switch (action)
            {
                case KeyAction::DOWN:
                    if(key == MouseKey::LButton)
                        LOG("Left Mouse Down", Color::RED);
                    else if(key == MouseKey::MButton)
                        LOG("Middle Mouse Down", Color::RED);
                    else if (key == MouseKey::RButton)
                        LOG("Right Mouse Down", Color::RED);
                break;
                case KeyAction::UP:
                    if (key == MouseKey::LButton)
                        LOG("Left Mouse Up", Color::RED);
                    else if (key == MouseKey::MButton)
                        LOG("Middle Mouse Up", Color::RED);
                    else if (key == MouseKey::RButton)
                        LOG("Right Mouse Up", Color::RED);
                break;
            }
            if (mod & KeyModBits::CONTROL)
                LOG("Control Pressed!", Color::BLUE);
            if (mod & KeyModBits::SHIFT)
                LOG("Shift Pressed!", Color::VIOLET);
            if (mod & KeyModBits::ALT)
                LOG("ALT was pressed!", Color::GREEN);
    }

    //----------------------------------------------------------------------
    void InputManager::_CursorMovedCallback( I16 x, I16 y )
    {
        //LOG( "(" + TS(x) + "," + TS(y) + ")", Color::GREEN );
    }

    //----------------------------------------------------------------------
    void InputManager::_MouseWheelCallback( I16 param )
    {
        LOG( "Mouse Wheel: " + TS(param), Color::RED );
    }

    //----------------------------------------------------------------------
    void InputManager::_KeyCallback( Key key, KeyAction action, KeyMod mod )
    {
        switch (action)
        {
        case KeyAction::DOWN:
            if (key == Key::Enter)
                LOG("Enter pressed", Color::YELLOW);
            if (key == Key::C)
                LOG("C pressed", Color::YELLOW);
            if (key == Key::NumPad0)
                LOG("NumPad0 pressed", Color::YELLOW);
        }
    }

    //----------------------------------------------------------------------
    void InputManager::_CharCallback( char c )
    {
        static String buffer;
        if (c == '\b')
            buffer = buffer.substr(0, buffer.size() - 1);
        else
            buffer += c;

        if (buffer.size() > 0)
            LOG(buffer);
    }

} }
