#include "axis_mapper.h"
/**********************************************************************
    class: AxisMapper (axis_mapper.cpp)

    author: S. Hau
    date: November 18, 2017
**********************************************************************/

#include "Logging/logging.h"
#include "Core/Input/devices/keyboard.h"
#include "Core/Input/devices/mouse.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    #define AXIS_MIN    -1.0
    #define AXIS_MAX    1.0

    //----------------------------------------------------------------------
    F64 AxisMapper::getAxisValue( const char* name ) const
    {
        StringID axis = SID( name );
        if ( m_axisMap.count( axis ) == 0 )
        {
            LOG_WARN( "InputManager::getAxis(): Axis name '" + axis.toString() + "' does not exist." );
            return 0.0f;
        }
        return m_axisMap.at( axis ).value;
    }

    //----------------------------------------------------------------------
    void AxisMapper::registerAxis( const char* name, Key key0, Key key1, F64 speed )
    {
        StringID axisName = SID( name );
        if ( m_axisMap.count( axisName ) == 0 )
        {
            // Register new axis
            Axis axis;
            axis.speed = speed;
            axis.events.push_back( { EInputDevice::Keyboard, key0, key1 } );

            m_axisMap[ axisName ] = axis;
        }
        else
        {
            // Axis already exists, just add new event
            m_axisMap[ axisName ].events.push_back( { EInputDevice::Keyboard, key0, key1 } );
        }
    }

    //----------------------------------------------------------------------
    void AxisMapper::registerAxis( const char* name, MouseKey key0, MouseKey key1, F64 speed )
    {
        StringID axisName = SID( name );
        if ( m_axisMap.count( axisName ) == 0 )
        {
            // Register new axis
            Axis axis;
            axis.speed = speed;
            axis.events.push_back( { EInputDevice::Mouse, key0, key1 } );

            m_axisMap[ axisName ] = axis;
        }
        else
        {
            // Axis already exists, just add new event
            m_axisMap[ axisName ].events.push_back( { EInputDevice::Mouse, key0, key1 } );
        }
    }

    //----------------------------------------------------------------------
    void AxisMapper::unregisterAxis( const char* name )
    {
        StringID axisName = SID( name );
        m_axisMap.erase( m_axisMap.find( axisName ) );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void AxisMapper::_UpdateInternalState( F64 delta )
    {
        _UpdateAxes( delta );
        _UpdateMouseWheelAxis( delta );
    }

    //----------------------------------------------------------------------
    void AxisMapper::_UpdateAxes( F64 delta )
    {
        for (auto& pair : m_axisMap)
        {
            Axis&   axis        = pair.second;
            F64     step        = (axis.speed * delta);
            bool    triggered   = false;

            for (auto& evt : axis.events)
            {
                bool key0Down = false;
                bool key1Down = false;
                switch (evt.device)
                {
                case EInputDevice::Keyboard:
                    key0Down = m_keyboard->isKeyDown( evt.keyboardKey0 );
                    key1Down = m_keyboard->isKeyDown( evt.keyboardKey1 );
                    break;
                case EInputDevice::Mouse:
                    key0Down = m_mouse->isKeyDown( evt.mouseKey0 );
                    key1Down = m_mouse->isKeyDown( evt.mouseKey1 );
                    break;
                default:
                    LOG_WARN( "AxisMapper::_UpdateInternalState: Unknown input device." );
                }

                if (key0Down)
                {
                    axis.value = approach( axis.value, AXIS_MAX, step );
                    triggered = true;
                }

                if (key1Down)
                {
                    axis.value = approach( axis.value, AXIS_MIN, step );
                    triggered = true;
                }
            }

            // Approach 0 as long no event was triggered this tick
            if ( not triggered )
                axis.value = approach( axis.value, 0.0, step );
        }
    }

    //----------------------------------------------------------------------
    void AxisMapper::_UpdateMouseWheelAxis( F64 delta )
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


} } // End namespaces