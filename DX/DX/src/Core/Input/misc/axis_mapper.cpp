#include "axis_mapper.h"
/**********************************************************************
    class: AxisMapper (axis_mapper.cpp)

    author: S. Hau
    date: November 18, 2017
**********************************************************************/

#include "Logging/logging.h"
#include "Core/Input/devices/keyboard.h"
#include "Core/Input/devices/mouse.h"

#include "Math/math_utils.h"

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
    void AxisMapper::registerAxis( const char* name, Key key0, Key key1, F64 acceleration, F64 damping )
    {
        StringID axisName = SID( name );
        if ( m_axisMap.count( axisName ) == 0 )
        {
            // Register new axis
            Axis axis;
            axis.acceleration = acceleration;
            axis.damping = damping;
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
    void AxisMapper::registerAxis( const char* name, MouseKey key0, MouseKey key1, F64 acceleration, F64 damping )
    {
        StringID axisName = SID( name );
        if ( m_axisMap.count( axisName ) == 0 )
        {
            // Register new axis
            Axis axis;
            axis.acceleration = acceleration;
            axis.damping = damping;
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

    //----------------------------------------------------------------------
    void AxisMapper::updateAxis( const char* name, F64 acceleration, F64 damping )
    {
        StringID axisName = SID( name );
        if ( m_axisMap.find( axisName ) == m_axisMap.end() )
        {
            LOG_WARN( "AxisMapper::updateAxis(): Given axis name '" + String(name) + "' does not exist." );
            return;
        }

        m_axisMap[axisName].acceleration = acceleration;
        m_axisMap[axisName].damping = damping;
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
                    LOG_WARN( "AxisMapper::_UpdateInternalState(): Unknown input device." );
                }

                if (key0Down)
                {
                    axis.value = Math::Lerp( axis.value, AXIS_MAX, axis.acceleration * delta );
                    triggered = true;
                }

                if (key1Down)
                {
                    axis.value = Math::Lerp( axis.value, AXIS_MIN, axis.acceleration * delta );
                    triggered = true;
                }
            }

            // Approach 0 as long no event was triggered this tick
            if ( not triggered )
                axis.value = Math::Lerp( axis.value, 0.0, axis.damping * delta );
        }
    }

    //----------------------------------------------------------------------
    void AxisMapper::_UpdateMouseWheelAxis( F64 delta )
    {
        static F32 acceleration = 50.0f;
        static F32 damping = 0.15f;

        I16 wheelDelta = m_mouse->getWheelDelta();
        if (wheelDelta != 0)
        {
            m_wheelAxis += acceleration * wheelDelta * delta;
        }
        else
        {
            m_wheelAxis = Math::Lerp( m_wheelAxis, 0.0, damping );
        }
    }


} } // End namespaces