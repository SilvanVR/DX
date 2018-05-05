#include "input_listener.h"
/**********************************************************************
    class: Several Listener (input_listener.cpp)

    author: S. Hau
    date: November 16, 2017
**********************************************************************/

#include "Core/locator.h"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    IKeyListener::IKeyListener()
    {
        ASSERT( &Locator::getInputManager() != nullptr && "InputManager does not exist yet." );
        Locator::getInputManager().getKeyboard()._Subscribe( this );
    }

    //----------------------------------------------------------------------
    IKeyListener::~IKeyListener()
    {
        Locator::getInputManager().getKeyboard()._Unsubscribe( this );
    }

    //----------------------------------------------------------------------
    IMouseListener::IMouseListener()
    {
        ASSERT( &Locator::getInputManager() != nullptr && "InputManager does not exist yet." );
        Locator::getInputManager().getMouse()._Subscribe( this );
    }

    //----------------------------------------------------------------------
    IMouseListener::~IMouseListener()
    {
        Locator::getInputManager().getMouse()._Unsubscribe( this );
    }


} } // end namespaces