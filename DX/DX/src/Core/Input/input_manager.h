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

     @Considerations:
      - Move axes stuff in a separate class
       > Axes names should use action names
        (or integrate axis stuff into action mapper)
      - Action listener
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "listener/input_listener.h"
#include "devices/keyboard.h"
#include "devices/mouse.h"
#include "misc/action_mapper.h"

//----------------------------------------------------------------------
#define MOUSE           Locator::getInputManager().getMouse()
#define KEYBOARD        Locator::getInputManager().getKeyboard()
#define ACTION_MAPPER   Locator::getInputManager().getActionMapper()

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
        void OnTick(Time::Seconds delta) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        Mouse&          getMouse()          { return *m_mouse; }
        Keyboard&       getKeyboard()       { return *m_keyboard; }
        ActionMapper&   getActionMapper()   { return *m_actionMapper; }

        //----------------------------------------------------------------------
        // Return the axis value for the corresponding axis.
        // @Param:
        //  "name": The name of the registered axis.
        // @Return:
        //  The value of the registered axis.
        //----------------------------------------------------------------------
        F64 getAxis(const char* name) const;

        //----------------------------------------------------------------------
        // @Return:
        //  The value of the wheel axis.
        //----------------------------------------------------------------------
        F64 getMouseWheelAxis() const { return m_wheelAxis; }

        //----------------------------------------------------------------------
        // Register a new axis. The value of the axis interpolates
        // between -1 and 1 depending on which key is/was pressed.
        // @Params:
        //  "name": The name of the new axis.
        //  "key0": This key increases the axis value.
        //  "key1": This key decreases the axis value.
        //  "acc":  How fast the bounds are reached.
        //----------------------------------------------------------------------
        void registerAxis(const char* name, Key key0, Key key1, F64 acc = 1.0f);
        void unregisterAxis(const char* name);

    private:
        // <---------- DEVICES ----------->
        Keyboard*       m_keyboard  = nullptr;
        Mouse*          m_mouse     = nullptr;

        // <---------- MISC ----------->
        ActionMapper*   m_actionMapper = nullptr;

        // <---------- AXIS ----------->
        struct AxisInfo
        {
            StringID    name;
            Key         key0;
            Key         key1;
            F64         acc;
        };
        ArrayList<AxisInfo>     m_axisInfos;
        HashMap<StringID, F64>  m_axisMap;
        F64                     m_wheelAxis;

        void _UpdateAxes( F64 delta );
        void _UpdateMouseWheelAxis( F64 delta );

    private:
        //----------------------------------------------------------------------
        InputManager(const InputManager& other)                 = delete;
        InputManager& operator = (const InputManager& other)    = delete;
        InputManager(InputManager&& other)                      = delete;
        InputManager& operator = (InputManager&& other)         = delete;
    };


} }