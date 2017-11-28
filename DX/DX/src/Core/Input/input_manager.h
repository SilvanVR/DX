#pragma once
/**********************************************************************
    class: InputManager (input_manager.h)

    author: S. Hau
    date: November 18, 2017

    This subsystem serves as the main Hub for all Input stuff.

    Features:
      - Mouse Input
      - Keyboard Input
      - Axis calculations (Smooth values based on several keys)
      - Action Mappings (Names to input events)
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "listener/input_listener.h"
#include "devices/keyboard.h"
#include "devices/mouse.h"
#include "misc/action_mapper.h"
#include "misc/axis_mapper.h"

//----------------------------------------------------------------------
#define MOUSE           Locator::getInputManager().getMouse()
#define KEYBOARD        Locator::getInputManager().getKeyboard()
#define ACTION_MAPPER   Locator::getInputManager().getActionMapper()
#define AXIS_MAPPER     Locator::getInputManager().getAxisMapper()

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
        void init(CoreEngine* coreEngine) override;
        void OnTick(Time::Seconds delta) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        Mouse&          getMouse()          { return *m_mouse; }
        Keyboard&       getKeyboard()       { return *m_keyboard; }
        ActionMapper&   getActionMapper()   { return *m_actionMapper; }
        AxisMapper&     getAxisMapper()     { return *m_axisMapper; }

    private:
        // <---------- DEVICES ----------->
        Keyboard*       m_keyboard  = nullptr;
        Mouse*          m_mouse     = nullptr;

        // <---------- MISC ----------->
        ActionMapper*   m_actionMapper  = nullptr;
        AxisMapper*     m_axisMapper    = nullptr;

    private:
        //----------------------------------------------------------------------
        InputManager(const InputManager& other)                 = delete;
        InputManager& operator = (const InputManager& other)    = delete;
        InputManager(InputManager&& other)                      = delete;
        InputManager& operator = (InputManager&& other)         = delete;
    };


} }
