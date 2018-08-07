#include "controller.h"
/**********************************************************************
    class: Controller

    author: S. Hau
    date: August 8, 2018
**********************************************************************/

#include "../listener/input_listener.h"
#include "Graphics/VR/OculusRift/oculus_rift.h"
#include "Core/locator.h"

namespace Core { namespace Input {

        //----------------------------------------------------------------------
        #define LISTENER_START_CAPACITY 4

        //----------------------------------------------------------------------
        Controller::Controller()
            : IChannelUser( EInputChannels::Default )
        {
            // Zero out arrays
            memset( m_keyPressedThisTick, 0, MAX_KEYS * sizeof( bool ) );
            memset( m_keyPressedLastTick, 0, MAX_KEYS * sizeof( bool ) );

            // Preallocate mem for stl stuff
            //m_keyListener.reserve( LISTENER_START_CAPACITY );
        }

        //----------------------------------------------------------------------
        void Controller::_UpdateInternalState()
        {
            _Update();
        }

        //**********************************************************************
        // PUBLIC
        //**********************************************************************

        //----------------------------------------------------------------------
        bool Controller::isTouchControllerConnected() const
        {
            return Graphics::VR::GetConnectedController() & ovrControllerType_Touch;
        }

        //----------------------------------------------------------------------
        bool Controller::isXBoxControllerConnected() const
        {
            return Graphics::VR::GetConnectedController() & ovrControllerType_XBox;
        }

        //----------------------------------------------------------------------
        bool Controller::isSupported() const
        {
            return Graphics::VR::OVRIsInitialized();
        }

        //----------------------------------------------------------------------
        bool Controller::isKeyDown( ControllerKey key ) const
        {
            return m_keyPressedThisTick[ (I32)key ] && _IsMasterChannelSet();
        }

        //----------------------------------------------------------------------
        bool Controller::wasKeyPressed( ControllerKey key ) const
        {
            I32 keyIndex = (I32)key;
            return ( m_keyPressedThisTick[ keyIndex ] && not m_keyPressedLastTick[ keyIndex ] ) && _IsMasterChannelSet();
        }

        //----------------------------------------------------------------------
        bool Controller::wasKeyReleased( ControllerKey key ) const
        {
            I32 keyIndex = (I32)key;
            return ( not m_keyPressedThisTick[ keyIndex ] && m_keyPressedLastTick[ keyIndex ] ) && _IsMasterChannelSet();
        }

        //**********************************************************************
        // PRIVATE
        //**********************************************************************

        //----------------------------------------------------------------------
        void Controller::_Update()
        {
            if ( not isSupported() )
                return;

            // Current used controller. A controller gets switched whenever a button on it is pressed.
            static ovrControllerType activeController = ovrControllerType_Touch;

            // Save last state and reset it
            memcpy( m_keyPressedLastTick, m_keyPressedThisTick, MAX_KEYS * sizeof( bool ) );
            memset( m_keyPressedThisTick, 0, MAX_KEYS * sizeof( bool ) );
            memset( m_touches, 0, MAX_TOUCHES * sizeof( bool ) );

            // Figure out which controller is used
            auto xboxControllerConnected = isXBoxControllerConnected();
            auto touchControllerConnected = isTouchControllerConnected();
            if (not xboxControllerConnected && not touchControllerConnected)
                return;

            if (xboxControllerConnected)
            {
                auto xboxState = Graphics::VR::GetOVRInputState( ovrControllerType_XBox );
                if (xboxState.Buttons) // Is any button pressed?
                    activeController = ovrControllerType_XBox;
            }

            if (touchControllerConnected)
            {
                auto touchState = Graphics::VR::GetOVRInputState( ovrControllerType_Touch );
                if (touchState.Buttons) // Is any button pressed?
                    activeController = ovrControllerType_Touch;
            }

            // Update buttons
            auto inputState = Graphics::VR::GetOVRInputState( activeController );
            if (inputState.Buttons & ovrButton_A)           m_keyPressedThisTick[(I32)ControllerKey::A] = true;
            if (inputState.Buttons & ovrButton_B)           m_keyPressedThisTick[(I32)ControllerKey::B] = true;
            if (inputState.Buttons & ovrButton_X)           m_keyPressedThisTick[(I32)ControllerKey::X] = true;
            if (inputState.Buttons & ovrButton_Y)           m_keyPressedThisTick[(I32)ControllerKey::Y] = true;
            if (inputState.Buttons & ovrButton_LThumb)      m_keyPressedThisTick[(I32)ControllerKey::LThumb] = true;
            if (inputState.Buttons & ovrButton_RThumb)      m_keyPressedThisTick[(I32)ControllerKey::RThumb] = true;
            if (inputState.Buttons & ovrButton_LShoulder)   m_keyPressedThisTick[(I32)ControllerKey::LShoulder] = true;
            if (inputState.Buttons & ovrButton_RShoulder)   m_keyPressedThisTick[(I32)ControllerKey::RShoulder] = true;
            if (inputState.Buttons & ovrButton_Home)        m_keyPressedThisTick[(I32)ControllerKey::Home] = true;
            if (inputState.Buttons & ovrButton_Enter)       m_keyPressedThisTick[(I32)ControllerKey::Enter] = true;
            if (inputState.Buttons & ovrButton_Left)        m_keyPressedThisTick[(I32)ControllerKey::Left] = true;
            if (inputState.Buttons & ovrButton_Right)       m_keyPressedThisTick[(I32)ControllerKey::Right] = true;
            if (inputState.Buttons & ovrButton_Down)        m_keyPressedThisTick[(I32)ControllerKey::Down] = true;
            if (inputState.Buttons & ovrButton_Up)          m_keyPressedThisTick[(I32)ControllerKey::Up] = true;

            // Update axes
            m_indexTrigger[0] = inputState.IndexTrigger[0];
            m_indexTrigger[1] = inputState.IndexTrigger[1];
            if (m_indexTrigger[0] > 0.5f) m_keyPressedThisTick[(I32)ControllerKey::LIndexTrigger] = true;
            if (m_indexTrigger[1] > 0.5f) m_keyPressedThisTick[(I32)ControllerKey::RIndexTrigger] = true;

            m_handTrigger[0] = inputState.HandTrigger[0];
            m_handTrigger[1] = inputState.HandTrigger[1];
            if (m_handTrigger[0] > 0.5f) m_keyPressedThisTick[(I32)ControllerKey::LHandTrigger] = true;
            if (m_handTrigger[1] > 0.5f) m_keyPressedThisTick[(I32)ControllerKey::RHandTrigger] = true;

            m_thumbsticks[0] = { inputState.Thumbstick[0].x, inputState.Thumbstick[0].y };
            m_thumbsticks[1] = { inputState.Thumbstick[1].x, inputState.Thumbstick[1].y };

            // Update touches
            if (inputState.Touches & ovrTouch_A)             m_touches[(I32)ControllerTouch::A] = true;
            if (inputState.Touches & ovrTouch_B)             m_touches[(I32)ControllerTouch::B] = true;
            if (inputState.Touches & ovrTouch_RThumb)        m_touches[(I32)ControllerTouch::RThumb] = true;
            if (inputState.Touches & ovrTouch_RThumbRest)    m_touches[(I32)ControllerTouch::RThumbRest] = true;
            if (inputState.Touches & ovrTouch_RIndexTrigger) m_touches[(I32)ControllerTouch::RIndexTrigger] = true;
        }

    }
} // end namespaces