#include "i_component.h"
/**********************************************************************
    class: IComponent (i_component.cpp)

    author: S. Hau
    date: December 17, 2017
**********************************************************************/

#include "../gameobject.h"

namespace Components
{

    //----------------------------------------------------------------------
    bool IComponent::isActive() const 
    { 
        return m_pGameObject->isActive() && m_isActive; 
    }

}