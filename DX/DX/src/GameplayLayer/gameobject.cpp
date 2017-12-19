#include "gameobject.h"
/**********************************************************************
    class: GameObject (gameobject.cpp)

    author: S. Hau
    date: December 17, 2017
**********************************************************************/

#include "i_scene.h"

//----------------------------------------------------------------------
GameObject::GameObject( IScene* scene, CString name )
    : m_attachedScene( scene ), m_name( name ) 
{
}

//----------------------------------------------------------------------
GameObject::~GameObject()
{
    for (auto pair : m_components)
        SAFE_DELETE( pair.second );
}

//**********************************************************************
// PUBLIC
//**********************************************************************

ArrayList<Components::IComponent*> GameObject::getComponents() const
{ 
    ArrayList<Components::IComponent*> components;
    for (auto pair : m_components)
        components.push_back( pair.second );
    return components;
}

//**********************************************************************
// PRIVATE
//**********************************************************************