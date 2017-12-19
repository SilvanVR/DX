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
    for (auto component : m_components)
        SAFE_DELETE( component );
}

//**********************************************************************
// PUBLIC
//**********************************************************************



//**********************************************************************
// PRIVATE
//**********************************************************************