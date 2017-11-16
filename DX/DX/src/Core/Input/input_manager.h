#pragma once
/**********************************************************************
    class: InputManager (input_manager.h)

    author: S. Hau
    date: November 4, 2017

    - Virtual Mappings
      - just another level of indirection. 
        Store a map of [Key <-> Key] and check which key corresponds to the "virtual key"

    - Fixed mouse pos
    - Mouse delta

    - Ask if key is pressed CONTINOUSLY
    - Ask if key is pressed THIS FRAME

    {
        - Subscribe to events? 
           - e.g. Component A is interested in OnKeyDown(...);
             > Locator::getInputManager().subscribeMouseMoved( this );
                -> OnMouseMove(x, y) is called on this object then.
        - Attack callbacks directly to the input system?
    }

    - ActionNames e.g. "MoveForward" -> 
       should trigger when e.g. "W" is pressed OR controller "Forward"
       -> Map actions to names, so the action can be triggered regardless of input device


    Responsibilites:
      - 
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "Core/OS/Window/keycodes.h"

namespace Core { namespace Input {


    //**********************************************************************
    class InputManager : public ISubSystem
    {
        static const U32 MAX_KEYS = 255;

    public:
        InputManager() = default;
        ~InputManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void OnTick(Time::Seconds delta) override;
        void shutdown() override;


        //----------------------------------------------------------------------
        void _KeyCallback(Key key, KeyAction action, KeyMod mod);
        void _CharCallback(char c);
        void _MouseCallback(MouseKey key, KeyAction action, KeyMod mod);
        void _MouseWheelCallback(I16 param);
        void _CursorMovedCallback(I16 x, I16 y);

    private:
        bool m_keyPressed[MAX_KEYS];
        bool m_keyPressedLastTick[MAX_KEYS];

        HashMap<KeyAction, KeyAction> m_virtualKeys;

        //----------------------------------------------------------------------
        InputManager(const InputManager& other)                 = delete;
        InputManager& operator = (const InputManager& other)    = delete;
        InputManager(InputManager&& other)                      = delete;
        InputManager& operator = (InputManager&& other)         = delete;
    };


} }
