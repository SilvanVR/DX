#pragma once
/**********************************************************************
    class: ActionMapper (action_mapper.h)

    author: S. Hau
    date: November 18, 2017

    Allows to fire events based on names and regardless of input device.
    Code example:
      actionMapper.attachKeyboardEvent( "Fire", Key::E );
      actionMapper.attachMouseEvent( "Fire", MouseKey::LBUTTON );
      ...
      if ( actionMapper.isKeyPressed("Fire") )
        [fires either when "E" or "LBUTTON" is held down]
**********************************************************************/

#include "Core/Input/input_enums.hpp"
#include "OS/Window/keycodes.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    class Keyboard;
    class Mouse;
    class Controller;

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
                Key             keyboardKey;
                MouseKey        mouseKey;
                ControllerKey   controllerKey;
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
        ActionMapper(const Keyboard* keyboard, const Mouse* mouse, const Controller* controller) 
            : m_keyboard(keyboard), m_mouse(mouse), m_controller(controller) {}
        ~ActionMapper() = default;

        //----------------------------------------------------------------------
        // Attach a new input event to an existing or new action.
        // @Params:
        //  "name": The name of the action.
        //  "Key/MouseKey/...": Which button should fire this action.
        //----------------------------------------------------------------------
        void attachKeyboardEvent(const char* name, Key key);
        void attachMouseEvent(const char* name, MouseKey key);
        void attachControllerEvent(const char* name, ControllerKey key);

        //----------------------------------------------------------------------
        // Same functions as with mouse / keyboard.
        //----------------------------------------------------------------------
        bool isKeyDown(const char* name);
        bool wasKeyPressed(const char* name);
        bool wasKeyReleased(const char* name);

    private:
        const Keyboard*             m_keyboard;
        const Mouse*                m_mouse;
        const Controller*           m_controller;
        HashMap<StringID, Action>   m_actionEvents;

        // Should be called once per tick. Checks if actions should be fired.
        friend class InputManager;
        void _UpdateInternalState();

        NULL_COPY_AND_ASSIGN(ActionMapper)
    };

} } // end namespaces
