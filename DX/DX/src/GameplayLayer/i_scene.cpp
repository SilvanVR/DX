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
    for (GameObject* go : m_enabledGameObjects)
        SAFE_DELETE( go );
    for (GameObject* go : m_disabledGameObjects)
        SAFE_DELETE( go );
}

//**********************************************************************
// PUBLIC
//**********************************************************************

//----------------------------------------------------------------------
GameObject* IScene::createGameObject( CString name )
{
    GameObject* go = new GameObject( this, name );
    go->addComponent<CTransform>();
    m_enabledGameObjects.push_back( go );
    return go;
}

//----------------------------------------------------------------------
GameObject* IScene::findGameObject( CString name )
{
    StringID nameAsID = SID( name );
    for (GameObject* go : m_enabledGameObjects)
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

}

//----------------------------------------------------------------------
void IScene::_Tick( Core::Time::Seconds delta )
{

}

//----------------------------------------------------------------------
void IScene::_LateTick( Core::Time::Seconds delta )
{

}