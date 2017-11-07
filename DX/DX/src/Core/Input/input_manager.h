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

    - When update? 
       each frame: Then it's not possible to detect if key was pressed "last frame"
       each tick:  This should work, but subsystems can't tick yet, only update...


    Responsibilites:
      - 
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "Core/OS/Window/keycodes.h"

namespace Core { namespace Input {


    //**********************************************************************
    class InputManager : public ISubSystem
    {
    public:
        InputManager() = default;
        ~InputManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void OnUpdate(Time::Seconds delta) override;
        void shutdown() override;


        //----------------------------------------------------------------------
        void _KeyCallback(Key key, KeyAction action, KeyMod mod);
        void _CharCallback(char c);
        void _MouseCallback(MouseKey key, KeyAction action, KeyMod mod);
        void _MouseWheelCallback(I16 param);
        void _CursorMovedCallback(I16 x, I16 y);

    private:

        //----------------------------------------------------------------------
        InputManager(const InputManager& other)                 = delete;
        InputManager& operator = (const InputManager& other)    = delete;
        InputManager(InputManager&& other)                      = delete;
        InputManager& operator = (InputManager&& other)         = delete;
    };


} }
