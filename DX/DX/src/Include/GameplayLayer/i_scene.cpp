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
#include "Components/Rendering/camera.h"

//----------------------------------------------------------------------
IScene::IScene( CString name ) 
    : m_name( name, true ) 
{
}

//----------------------------------------------------------------------
IScene::~IScene()
{
    while (not m_gameObjects.empty())
    {
        // Because gameobjects can delete other gameobjects, we must delete them one by one from the front
        SAFE_DELETE( m_gameObjects.front() );
        m_gameObjects.erase( m_gameObjects.begin(), m_gameObjects.begin() + 1 );
    }
}

//**********************************************************************
// PUBLIC
//**********************************************************************

//----------------------------------------------------------------------
GameObject* IScene::createGameObject( CString name )
{
    GameObject* go = new GameObject( this, name );

    go->addComponent<Components::Transform>();
    m_gameObjectsToAdd.push_back( go );
    return go;
}

//----------------------------------------------------------------------
void IScene::destroyGameObject( GameObject* go )
{
    for (auto child : go->getTransform()->getChildren())
        destroyGameObject( child->getGameObject() );

    m_gameObjects.erase( std::remove( m_gameObjects.begin(), m_gameObjects.end(), go ), m_gameObjects.end() );
    SAFE_DELETE( go );
}

//----------------------------------------------------------------------
GameObject* IScene::findGameObject( CString name )
{
    StringID nameAsID = SID( name );
    for ( auto& go : m_gameObjects )
        if ( go->getName() == nameAsID )
            return go;
    return nullptr;
}

//----------------------------------------------------------------------
Components::Camera* IScene::getMainCamera()
{
    for ( auto& cam : m_componentManager.getCameras() )
    {
        if( cam->isBlittingToScreen() || cam->isBlittingToHMD() )
            return cam;
    }

    return nullptr;
}

//**********************************************************************
// PRIVATE
//**********************************************************************

//----------------------------------------------------------------------
void IScene::_PreTick( Time::Seconds delta )
{
    if ( not m_gameObjectsToAdd.empty() )
    {
        m_gameObjects.insert( m_gameObjects.end(), m_gameObjectsToAdd.begin(), m_gameObjectsToAdd.end() );
        m_gameObjectsToAdd.clear();
    }

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
    tick( delta );

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

