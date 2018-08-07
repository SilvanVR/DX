#pragma once
/**********************************************************************
    class: Controller

    author: S. Hau
    date: August 8, 2018

    Encapsulation for any controller connected to the system e.g.
    updates its internal state based on the current used controller, e.g.
    the Oculus Touch or the XBox controller. Controller can be switched
    when any button on the corresponding one is pressed.
**********************************************************************/

#include "../input_enums.hpp"
#include "../channel_user.hpp"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    class IControllerListener;

    //**********************************************************************
    class Controller : public IChannelUser
    {
        static const U32 MAX_KEYS = (U32)ControllerKey::MAX_KEYS;
        static const U32 MAX_TOUCHES = (U32)ControllerTouch::MAX_KEYS;

    public:
        Controller();
        ~Controller() = default;

        //----------------------------------------------------------------------
        bool isAnyControllerConnected() const { return isTouchControllerConnected() || isXBoxControllerConnected(); }
        bool isTouchControllerConnected() const;
        bool isXBoxControllerConnected() const;

        //----------------------------------------------------------------------
        // @Return: True whether any controller is supported.
        //----------------------------------------------------------------------
        bool isSupported() const;

        //----------------------------------------------------------------------
        // @Return: True when the given key is down.
        //----------------------------------------------------------------------
        bool isKeyDown(ControllerKey key) const;

        //----------------------------------------------------------------------
        // @Return: True, the frame the key was pressed.
        //----------------------------------------------------------------------
        bool wasKeyPressed(ControllerKey key) const;

        //----------------------------------------------------------------------
        // @Return: True, the frame the key was released.
        //----------------------------------------------------------------------
        bool wasKeyReleased(ControllerKey key) const;

        //----------------------------------------------------------------------
        // @Return: Trigger value in the range 0.0f - 1.0f
        //----------------------------------------------------------------------
        F32 getIndexTrigger(ESide side) const { return m_indexTrigger[(I32)side]; }
        F32 getHandTrigger(ESide side) const { return m_handTrigger[(I32)side]; }

        //----------------------------------------------------------------------
        // @Return: Thumbstick axes (left-right / up-down) in the range 0.0f - 1.0f
        //----------------------------------------------------------------------
        Math::Vec2 getThumbstick(ESide side) const { return m_thumbsticks[(I32)side]; }

        //----------------------------------------------------------------------
        // @Return: Whether the user is touching the area on the oculus touch controller.
        //----------------------------------------------------------------------
        bool isTouching(ControllerTouch touch) { return m_touches[(I32)touch]; }

    private:
        bool m_keyPressedThisTick[MAX_KEYS];
        bool m_keyPressedLastTick[MAX_KEYS];
        bool m_touches[MAX_TOUCHES];

        F32         m_indexTrigger[2];
        F32         m_handTrigger[2];
        Math::Vec2  m_thumbsticks[2];

        //----------------------------------------------------------------------
        void _Update();

        //----------------------------------------------------------------------
        friend class InputManager;
        void _UpdateInternalState();

        NULL_COPY_AND_ASSIGN(Controller)
    };

} } // end namespaces