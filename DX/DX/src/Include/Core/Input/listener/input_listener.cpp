#include "input_listener.h"
/**********************************************************************
    class: Several Listener (input_listener.cpp)

    author: S. Hau
    date: November 16, 2017
**********************************************************************/

#include "Core/locator.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    IKeyListener::IKeyListener( InputChannels channels )
        : IDeviceListener( channels )
    {
        ASSERT( &Locator::getInputManager() != nullptr && "InputManager does not exist yet." );
        Locator::getInputManager().getKeyboard()._Subscribe( this );
    }

    //----------------------------------------------------------------------
    IKeyListener::IKeyListener( EInputChannel channel )
        : IKeyListener( (InputChannels)channel ) {}

    //----------------------------------------------------------------------
    IKeyListener::~IKeyListener()
    {
        Locator::getInputManager().getKeyboard()._Unsubscribe( this );
    }

    //----------------------------------------------------------------------
    IMouseListener::IMouseListener( InputChannels channels )
        : IDeviceListener( channels )
    {
        ASSERT( &Locator::getInputManager() != nullptr && "InputManager does not exist yet." );
        Locator::getInputManager().getMouse()._Subscribe( this );
    }

    //----------------------------------------------------------------------
    IMouseListener::IMouseListener( EInputChannel channel )
        : IMouseListener( (InputChannels)channel ) {}

    //----------------------------------------------------------------------
    IMouseListener::~IMouseListener()
    {
        Locator::getInputManager().getMouse()._Unsubscribe( this );
    }


} } // end namespaces