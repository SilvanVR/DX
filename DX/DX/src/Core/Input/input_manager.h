#pragma once
/**********************************************************************
    class: InputManager (input_manager.h)

    author: S. Hau
    date: November 4, 2017

    @TODO: Replace Point2D by vector

    Responsibilites:
      - Process the input window callbacks and saves the current state
        of input devices to be queried by anyone.

    Features:
      - Ask the InputManager about the current state e.g. mouse-pos etc.
      - Subscribe indirectly via inheritation from a listener class:
         IKeyListener: Override methods to get notified by key events.
         IMouseListener: Override methods to get notified by mouse events.
        Listener callbacks will be called whenever they occur in the
        update loop (possibly faster than tick rate)
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "listener/input_listener.h"
#include "mouse.h"

//----------------------------------------------------------------------
#define MOUSE Locator::getInputManager().getMouse()

namespace Core { namespace Input {

    class InputActionMapper
    {
    public:
        // Triggers action events
        // Set action events
    private:
        // Save state of action events
    };

    //----------------------------------------------------------------------
    #define AXIS_MIN        -1
    #define AXIS_MAX        1

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
        // @Return: True when the given key is down.
        //----------------------------------------------------------------------
        bool isKeyDown(Key key) const;

        //----------------------------------------------------------------------
        // @Return: True, the frame the key was pressed.
        //----------------------------------------------------------------------
        bool wasKeyPressed(Key key) const;

        //----------------------------------------------------------------------
        // @Return: True, the frame the key was released.
        //----------------------------------------------------------------------
        bool wasKeyReleased(Key key) const;

        //----------------------------------------------------------------------
        // Enable/Disable the first person mode.
        // True: Mouse is hidden and cursor is always centered.
        //----------------------------------------------------------------------
        void setFirstPersonMode(bool enabled) { m_mouse->setFirstPersonMode(enabled); }

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

        Mouse& getMouse() { return *m_mouse; }

    private:
        // <---------- KEYBOARD ----------->
        // Those will be updated per update
        bool    m_keyPressed[MAX_KEYS];
        bool    m_keyReleased[MAX_KEYS];

        // Those will be updated per tick
        bool    m_keyPressedThisTick[MAX_KEYS];
        bool    m_keyPressedLastTick[MAX_KEYS];

        // <---------- MOUSE ----------->
        Mouse*  m_mouse;

        // <---------- MISC ----------->
        HashMap<Key, Key>               m_virtualKeys;

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

        // <---------- LISTENER ----------->
        ArrayList<IKeyListener*>        m_keyListener;

        //----------------------------------------------------------------------
        friend class IKeyListener;
        void _Subscribe(IKeyListener* listener) { m_keyListener.push_back( listener ); }
        void _Unsubscribe(IKeyListener* listener) { m_keyListener.erase( std::remove( m_keyListener.begin(), m_keyListener.end(), listener ) ); }

        //----------------------------------------------------------------------
        void _UpdateKeyStates();
        void _NotifyKeyPressed(Key key, KeyMod mod) const;
        void _NotifyKeyReleased(Key key, KeyMod mod) const;
        void _NotifyOnChar(char c) const;

        void _UpdateAxes( F64 delta );
        void _UpdateMouseWheelAxis( F64 delta );

     public:
        //----------------------------------------------------------------------
        void _KeyCallback(Key key, KeyAction action, KeyMod mod);
        void _CharCallback(char c);

    private:
        //----------------------------------------------------------------------
        InputManager(const InputManager& other)                 = delete;
        InputManager& operator = (const InputManager& other)    = delete;
        InputManager(InputManager&& other)                      = delete;
        InputManager& operator = (InputManager&& other)         = delete;
    };


} }
