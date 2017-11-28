#pragma once
/**********************************************************************
    class: ActionMapper (action_mapper.h)

    author: S. Hau
    date: November 18, 2017

    Allows to fire events based on names and regardless of input device.
    Code example:
      actionMapper.attachKeyboardEvent( "Fire", Key::E );
      actionMapper.attackMouseEvent( "Fire", MouseKey::LBUTTON );
      ...
      if ( actionMapper.isKeyPressed("Fire") )
        [fires either when "E" or "LBUTTON" is held down]
**********************************************************************/

#include "Core/Input/e_input_devices.hpp"
#include "Core/OS/Window/keycodes.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    class Keyboard;
    class Mouse;

    //**********************************************************************
    // Maps names to several actions regardless of InputDevice
    //**********************************************************************
    class ActionMapper 
    {
        //----------------------------------------------------------------------
        struct InputDeviceEvent
        {
            EInputDevice device;
            union
            {
                Key      keyboardKey;
                MouseKey mouseKey;
            };
        };

        //----------------------------------------------------------------------
        struct Action
        {
            bool activeThisTick = false;
            bool activeLastTick = false;
            ArrayList<InputDeviceEvent> events;
        };

    public:
        ActionMapper(const Keyboard* keyboard, const Mouse* mouse) : m_keyboard( keyboard ), m_mouse( mouse ) {}
        ~ActionMapper() = default;

        //----------------------------------------------------------------------
        // Attach a new input event to an existing or new action.
        // @Params:
        //  "name": The name of the action.
        //  "Key/MouseKey/...": Which button should fire this action.
        //----------------------------------------------------------------------
        void attachKeyboardEvent(const char* name, Key key);
        void attachMouseEvent(const char* name, MouseKey key);

        //----------------------------------------------------------------------
        // Same functions as with mouse / keyboard.
        //----------------------------------------------------------------------
        bool isKeyDown(const char* name);
        bool wasKeyPressed(const char* name);
        bool wasKeyReleased(const char* name);

        // Should be called once per tick. Checks if actions should be fired.
        void _UpdateInternalState();

    private:
        HashMap<StringID, Action> m_actionEvents;
        const Keyboard* m_keyboard;
        const Mouse*    m_mouse;

        //----------------------------------------------------------------------
        ActionMapper(const ActionMapper& other)              = delete;
        ActionMapper& operator = (const ActionMapper& other) = delete;
        ActionMapper(ActionMapper&& other)                   = delete;
        ActionMapper& operator = (ActionMapper&& other)      = delete;
    };

} } // end namespaces
