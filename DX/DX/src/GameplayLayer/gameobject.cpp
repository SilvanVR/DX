#include "gameobject.h"
/**********************************************************************
    class: GameObject (gameobject.cpp)

    author: S. Hau
    date: December 17, 2017
**********************************************************************/

#include "i_scene.h"
#define INITIAL_COMPONENT_LIST_SIZE 4

//----------------------------------------------------------------------
GameObject::GameObject( IScene* scene, CString name )
    : m_attachedScene( scene ), m_name( name ) 
{
    m_components.reserve( INITIAL_COMPONENT_LIST_SIZE );
}

//----------------------------------------------------------------------
GameObject::~GameObject()
{
    for (IComponent* component : m_components)
        delete component;
}

//**********************************************************************
// PUBLIC
//**********************************************************************

void GameObject::setActive( bool active )
{
    m_isActive = active;
    m_attachedScene->_SetGameObjectActive( this, m_isActive );
}


//**********************************************************************
// PRIVATE
//**********************************************************************