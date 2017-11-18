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

    @Considerations:
      - Integrate axis stuff into this class. (axes should use action names)
**********************************************************************/

#include "Core/OS/Window/keycodes.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    class Keyboard;
    class Mouse;

    //----------------------------------------------------------------------
    enum class EInputDevice
    {
        Keyboard,
        Mouse
    };

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
        ActionMapper() = default;
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
        void _UpdateInternalState(const Keyboard& keyboard, const Mouse& mouse);

    private:
        HashMap<StringID, Action> m_actionEvents;

        //----------------------------------------------------------------------
        ActionMapper(const ActionMapper& other)              = delete;
        ActionMapper& operator = (const ActionMapper& other) = delete;
        ActionMapper(ActionMapper&& other)                   = delete;
        ActionMapper& operator = (ActionMapper&& other)      = delete;
    };

} } // end namespaces
