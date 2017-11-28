#include "input_manager.h"
/**********************************************************************
    class: InputManager (input_manager.cpp)

    author: S. Hau
    date: November 18, 2017
**********************************************************************/

#include "locator.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    void InputManager::init( CoreEngine* coreEngine )
    {
        // Subscribe for OnTick() event
        coreEngine->subscribe( this );

        // Create Input Devices
        OS::Window& window = Locator::getWindow();
        m_mouse     = new Mouse( &window );
        m_keyboard  = new Keyboard( &window );

        m_actionMapper = new ActionMapper( m_keyboard, m_mouse );
        m_axisMapper   = new AxisMapper( m_keyboard, m_mouse );

        // Register some default axes
        m_axisMapper->registerAxis( "Forward", Key::W, Key::S, 5.0f );
        m_axisMapper->registerAxis( "Left", Key::A, Key::D, 5.0f );
    }

    //----------------------------------------------------------------------
    void InputManager::OnTick( Time::Seconds delta )
    {
        m_keyboard->_UpdateInternalState();
        m_mouse->_UpdateInternalState();
        m_actionMapper->_UpdateInternalState();
        m_axisMapper->_UpdateInternalState( delta.value );
    }

    //----------------------------------------------------------------------
    void InputManager::shutdown()
    {
        delete m_axisMapper;
        delete m_actionMapper;
        delete m_keyboard;
        delete m_mouse;
    }


} }
