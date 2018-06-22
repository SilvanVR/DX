#pragma once
/**********************************************************************
    class: IScene + DefaultScene (i_scene.h)

    author: S. Hau
    date: December 17, 2017

    - Every scene consists of arbitrary gameobjects
    - Only one scene can be rendered at once
**********************************************************************/

#include "Time/durations.h"
#include "Components/component_manager.h"

namespace Core { class SceneManager; }
class GameObject;

//**********************************************************************
// Interface for a new scene.
//**********************************************************************
class IScene
{
public:
    IScene(CString name);
    virtual ~IScene();

    //----------------------------------------------------------------------
    virtual void init() = 0;
    virtual void tick(Time::Seconds delta) {}
    virtual void shutdown() = 0;

    //----------------------------------------------------------------------
    const StringID                      getName()               const { return m_name; }
    U32                                 numGameObjects()        const { return static_cast<U32>( m_gameObjects.size() ); }
    const ArrayList<GameObject*>&       getGameObjects()        const { return m_gameObjects; }
    const Components::ComponentManager& getComponentManager()   const { return m_componentManager; }
    Components::ComponentManager&       getComponentManager()         { return m_componentManager; }

    //----------------------------------------------------------------------
    GameObject*     createGameObject(CString name = "NO NAME");
    void            destroyGameObject(GameObject* go);
    GameObject*     findGameObject(CString name);

    //----------------------------------------------------------------------
    // Returns the first camera component which renders to the screen.
    //----------------------------------------------------------------------
    Components::Camera* getMainCamera();

private:
    StringID                        m_name;
    ArrayList<GameObject*>          m_gameObjects;
    Components::ComponentManager    m_componentManager;

    // Separate list of gameobjects to add to the gameobject list. Necessary, so components can create new gameobjects in tick()
    ArrayList<GameObject*>          m_gameObjectsToAdd;

    //----------------------------------------------------------------------
    friend class Core::SceneManager;
    void _PreTick(Time::Seconds delta);
    void _Tick(Time::Seconds delta);
    void _LateTick(Time::Seconds delta);

    NULL_COPY_AND_ASSIGN(IScene)
};


//**********************************************************************
// Default-Scene, which always exists and is loaded at startup.
//**********************************************************************
class DefaultScene : public IScene
{
public:
    DefaultScene() : IScene("Default Scene") {}

    //----------------------------------------------------------------------
    void init() {}
    void tick(Time::Seconds delta) {}
    void shutdown() {}
};