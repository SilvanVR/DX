#include "input_manager.h"
/**********************************************************************
    class: InputManager (input_manager.cpp)

    author: S. Hau
    date: November 18, 2017
**********************************************************************/

#include "locator.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    #define AXIS_MIN        -1
    #define AXIS_MAX        1

    //----------------------------------------------------------------------
    void InputManager::init()
    {
        // Subscribe for OnTick() event
        Locator::getCoreEngine().subscribe( this );

        // Create Input Devices
        OS::Window& window = Locator::getWindow();
        m_mouse     = new Mouse( &window );
        m_keyboard  = new Keyboard( &window );

        // Preallocate mem
        m_axisInfos.reserve( 4 );

        // Register some default axes
        registerAxis( "Forward", Key::W, Key::S, 5.0f );
        registerAxis( "Left", Key::A, Key::D, 5.0f );
    }

    //----------------------------------------------------------------------
    void InputManager::OnTick( Time::Seconds delta )
    {
        m_keyboard->_UpdateInternalState();
        m_mouse->_UpdateInternalState();
        m_actionMapper._UpdateInternalState( *m_keyboard, *m_mouse );
        _UpdateAxes( delta.value );
        _UpdateMouseWheelAxis( delta.value );
    }

    //----------------------------------------------------------------------
    void InputManager::shutdown()
    {
        delete m_mouse;
        delete m_keyboard;
    }

    //----------------------------------------------------------------------
    F64 InputManager::getAxis(const char* name) const
    {
        StringID axis = SID( name );
        if ( m_axisMap.count( axis ) == 0 )
        {
            WARN( "InputManager::getAxis(): Axis name '" + axis.toString() + "' does not exist." );
            return 0.0f;
        }
        return m_axisMap.at( axis );
    }

    //----------------------------------------------------------------------
    void InputManager::registerAxis(const char* name, Key key0, Key key1, F64 acc)
    {
        StringID axis = SID( name );
        if ( m_axisMap.count( axis ) != 0 )
        {
            WARN( "InputManager::registerAxis(): Axis '" + axis.toString() + "' already exists. Consider a different name." );
            return;
        }

        m_axisInfos.push_back( { axis, key0, key1, acc } );
        m_axisMap[ axis ] = 0.0;
    }

    //----------------------------------------------------------------------
    void InputManager::unregisterAxis( const char* name )
    {
        StringID axis = SID( name );
        for ( auto it = m_axisInfos.begin(); it != m_axisInfos.end(); it++ )
        {
            if ( axis == it->name )
            {
                m_axisInfos.erase( it );
                break;
            }
        }
        m_axisMap.erase( axis );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void InputManager::_UpdateAxes( F64 delta )
    {
        for (auto& axis : m_axisInfos)
        {
            bool key0Down   = m_keyboard->isKeyDown( axis.key0 );
            bool key1Down   = m_keyboard->isKeyDown( axis.key1 );
            F64 step        = (axis.acc * delta);

            F64& val = m_axisMap[ axis.name ];
            if (key0Down)
            {
                val += step;
                if (val > AXIS_MAX) val = AXIS_MAX;
            }

            if (key1Down)
            {
                val -= step;
                if (val < AXIS_MIN) val = AXIS_MIN;
            }

            if ( not key0Down && not key1Down )
            {
                if (val > step)
                    val -= step;
                else if (val < -step)
                    val += step;
                else
                    val = 0;
            }
        }
    }

    //----------------------------------------------------------------------
    void InputManager::_UpdateMouseWheelAxis( F64 delta )
    {
        static F32 acceleration = 50.0f;
        static F32 deceleration = 10.0f;

        I16 wheelDelta = m_mouse->getWheelDelta();
        if (wheelDelta != 0)
        {
            m_wheelAxis += acceleration * wheelDelta * delta;
        }
        else
        {
            F64 step = deceleration * delta;
            if (m_wheelAxis > step)
                m_wheelAxis -= step;
            else if (m_wheelAxis < -step)
                m_wheelAxis += step;
            else
                m_wheelAxis = 0;
        }
    }


} }
