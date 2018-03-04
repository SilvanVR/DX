#include "i_scene.h"
/**********************************************************************
    class: IScene (i_scene.cpp)

    author: S. Hau
    date: December 17, 2017

    The current entity-component system is highly unperformant due to
    a lot of virtual function calls and checking if an object is active.
    This will be redone sometime in the future.
**********************************************************************/

#include "gameobject.h"
#include "Components/transform.h"

//----------------------------------------------------------------------
IScene::IScene( CString name ) 
    : m_name( name ) 
{
}

//----------------------------------------------------------------------
IScene::~IScene()
{
    for (auto go : m_gameObjects)
        SAFE_DELETE( go );
}

//**********************************************************************
// PUBLIC
//**********************************************************************

//----------------------------------------------------------------------
GameObject* IScene::createGameObject( CString name )
{
    GameObject* go = new GameObject( this, name );

    go->addComponent<Components::Transform>();
    m_gameObjects.push_back( go );
    return go;
}

//----------------------------------------------------------------------
void IScene::destroyGameObject( GameObject* go )
{
    m_gameObjects.erase( std::remove( m_gameObjects.begin(), m_gameObjects.end(), go ), m_gameObjects.end() );
    SAFE_DELETE( go );
}

//----------------------------------------------------------------------
GameObject* IScene::findGameObject( CString name )
{
    StringID nameAsID = SID( name );
    for ( auto go : m_gameObjects )
        if ( go->getName() == nameAsID )
            return go;
    return nullptr;
}

//**********************************************************************
// PRIVATE
//**********************************************************************

//----------------------------------------------------------------------
void IScene::_PreTick( Time::Seconds delta )
{
    // Update components from all game-objects
    for ( auto go : m_gameObjects )
    {
        if ( go->isActive() )
            go->_PreTick( delta );
    }
}

//----------------------------------------------------------------------
void IScene::_Tick( Time::Seconds delta )
{
    // Update components from all game-objects
    for ( auto go : m_gameObjects )
    {
        if ( go->isActive() )
            go->_Tick( delta );
    }
}

//----------------------------------------------------------------------
void IScene::_LateTick( Time::Seconds delta )
{
    // Update components from all game-objects
    for ( auto go : m_gameObjects )
    {
        if ( go->isActive() )
            go->_LateTick( delta );
    }
}

