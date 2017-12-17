#include "i_scene.h"
/**********************************************************************
    class: IScene (i_scene.cpp)

    author: S. Hau
    date: December 17, 2017
**********************************************************************/

#include "gameobject.h"
#include "Components/ctransform.h"

//----------------------------------------------------------------------
IScene::IScene( CString name ) 
    : m_name( name ) 
{

}

//----------------------------------------------------------------------
IScene::~IScene()
{
    for (auto go : m_gameObjects.ActiveObjects())
        SAFE_DELETE( go );
    for (auto go : m_gameObjects.NonActiveObjects())
        SAFE_DELETE( go );
}

//**********************************************************************
// PUBLIC
//**********************************************************************

//----------------------------------------------------------------------
GameObject* IScene::createGameObject( CString name )
{
    //@TODO: Use custom allocator
    GameObject* go = new GameObject( this, name );
    go->addComponent<CTransform>();
    m_gameObjects.add( go );
    return go;
}

//----------------------------------------------------------------------
GameObject* IScene::findGameObject( CString name )
{
    StringID nameAsID = SID( name );
    for ( auto go : m_gameObjects.ActiveObjects() )
        if ( go->getName() == nameAsID )
            return go;
    return nullptr;
}

//**********************************************************************
// PRIVATE
//**********************************************************************

//----------------------------------------------------------------------
void IScene::_PreTick( Core::Time::Seconds delta )
{
    // Update components from all game-objects
    for ( auto go : m_gameObjects.ActiveObjects() )
    {
        if ( go->isActive() )
        {
            for ( auto comp : go->getComponents() )
            {
                if ( comp->isActive() )
                    comp->PreTick( delta );
            }
        }
    }
}

//----------------------------------------------------------------------
void IScene::_Tick( Core::Time::Seconds delta )
{
    // Update components from all game-objects
    for ( auto go : m_gameObjects.ActiveObjects() )
    {
        if ( go->isActive() )
        {
            for ( auto comp : go->getComponents() )
            {
                if ( comp->isActive() )
                    comp->Tick( delta );
            }
        }
    }
}

//----------------------------------------------------------------------
void IScene::_LateTick( Core::Time::Seconds delta )
{
    // Update components from all game-objects
    for ( auto go : m_gameObjects.ActiveObjects() )
    {
        if ( go->isActive() )
        {
            for ( auto comp : go->getComponents() )
            {
                if ( comp->isActive() )
                    comp->LateTick( delta );
            }
        }
    }
}


//----------------------------------------------------------------------
void IScene::_SetGameObjectActive( GameObject* go, bool active )
{
    m_gameObjects.disable(go);
}