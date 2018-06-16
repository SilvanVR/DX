#pragma once
/**********************************************************************
    class: AxisMapper (axis_mapper.h)

    author: S. Hau
    date: November 18, 2017

    An axis represents a value in the range of [-1, 1].
    Each axis value can be modified by two buttons. 
    Note that more than two buttons can be attached to an axis, e.g. for
    modifying the axis value with different devices.

    Code example:
     axisMapper.registerAxis( "Forward", Key::W, Key::S, 5.0f );
     axisMapper.registerAxis( "Forward", MouseKey::LButton, MouseKey::RButton, 5.0f );

     F32 val = axisMapper.getAxis( "Forward" );
**********************************************************************/

#include "Core/Input/e_input_devices.hpp"
#include "OS/Window/keycodes.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    class Keyboard;
    class Mouse;

    //**********************************************************************
    class AxisMapper
    {
    public:
        AxisMapper(const Keyboard* keyboard, const Mouse* mouse) : m_keyboard( keyboard ), m_mouse( mouse ) {}
        ~AxisMapper() = default;

        //----------------------------------------------------------------------
        // Return the axis value for the corresponding axis. Note that the axes 
        // are already multiplied with delta, so dont do it again.
        // @Param:
        //  "name": The name of the registered axis.
        // @Return:
        //  The value of the registered axis (frame-rate independant).
        //----------------------------------------------------------------------
        F64 getAxisValue(const char* name) const;

        //----------------------------------------------------------------------
        // @Return:
        //  The value of the wheel axis (frame-rate independant).
        //----------------------------------------------------------------------
        F64 getMouseWheelAxisValue() const { return m_wheelAxis; }

        //----------------------------------------------------------------------
        // Register a new axis. The value of the axis interpolates
        // between -1 and 1 depending on which key is/was pressed.
        // This function can be called more than once for an axis, e.g. for
        // setting up an axis for the mouse AND the keyboard.
        // @Params:
        //  "name": The name of the new axis.
        //  "key0": This key increases the axis value.
        //  "key1": This key decreases the axis value.
        //  "acceleration": How fast the bounds are reached.
        //  "damping": How fast the axis value will jump back to 0.
        //----------------------------------------------------------------------
        void registerAxis(const char* name, Key key0, Key key1, F64 acceleration = 1.0f, F64 damping = 1.0f);
        void registerAxis(const char* name, MouseKey key0, MouseKey key1, F64 acceleration = 1.0f, F64 damping = 1.0f);
        void unregisterAxis(const char* name);

        //----------------------------------------------------------------------
        // Updates an existing axis. Issues a warning if axis does not exist.
        //----------------------------------------------------------------------
        void updateAxis(const char* name, F64 acceleration, F64 damping);

        // Should be called once per tick. Checks if actions should be fired.
        void _UpdateInternalState(F64 delta);

    private:
        const Keyboard* m_keyboard;
        const Mouse*    m_mouse;

        // <---------- AXIS ----------->
        struct AxisEvent
        {
            AxisEvent(EInputDevice _device, Key key0, Key key1) : device( _device ), keyboardKey0( key0 ), keyboardKey1( key1 ) {}
            AxisEvent(EInputDevice _device, MouseKey key0, MouseKey key1) : device( _device ), mouseKey0( key0 ), mouseKey1( key1 ) {}

            EInputDevice device;
            union {
                Key      keyboardKey0;
                MouseKey mouseKey0;
            };
            union {
                Key      keyboardKey1;
                MouseKey mouseKey1;
            };
        };

        struct Axis
        {
            F64 value = 0.0;
            F64 acceleration = 1.0;
            F64 damping = 1.0;

            ArrayList<AxisEvent> events;
        };

        HashMap<StringID, Axis> m_axisMap;
        F64                     m_wheelAxis;

        void _UpdateAxes(F64 delta);
        void _UpdateMouseWheelAxis(F64 delta);

        NULL_COPY_AND_ASSIGN(AxisMapper)
    };

} } // end namespaces
