#pragma once
/**********************************************************************
    class: Several Listener (input_listener.h)

    author: S. Hau
    date: November 16, 2017
**********************************************************************/

#include "Core/OS/Window/keycodes.h"

namespace Core { namespace Input {

    //**********************************************************************
    class IKeyListener {
    public:
        IKeyListener();
        virtual ~IKeyListener();

    protected:
        friend class InputManager;
        virtual void OnKeyPressed(Key key) {}
        virtual void OnKeyReleased(Key key) {}
    };

    //**********************************************************************
    class IMouseListener {
    public:
        IMouseListener();
        virtual ~IMouseListener();

    protected:
        friend class InputManager;
        virtual void OnMouseMoved(I16 x, I16 y) {}
        virtual void OnMousePressed(MouseKey key) {}
        virtual void OnMouseReleased(MouseKey key) {}
        virtual void OnMouseWheel(I16 delta) {}
    };

} } // end namespaces