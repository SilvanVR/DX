#pragma once
/**********************************************************************
    class: IScene + DefaultScene (i_scene.h)

    author: S. Hau
    date: December 17, 2017

    - Every scene consists of arbitrary gameobjects
    - Only one scene can be rendered at once
**********************************************************************/

#include "Core/Time/durations.h"

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
    virtual void shutdown() = 0;

    //----------------------------------------------------------------------
    const StringID                  getName()           const { return m_name; }
    U32                             numGameObjects()    const { return static_cast<U32>( m_gameObjects.size() ); }
    const ArrayList<GameObject*>&   getGameObjects()    const { return m_gameObjects; }

    //----------------------------------------------------------------------
    // Creates a new gameobject, which belongs to this scene.
    //----------------------------------------------------------------------
    GameObject* createGameObject(CString name = "NO NAME");

    //----------------------------------------------------------------------
    // Destroy the given gameobject
    //----------------------------------------------------------------------
    void        destroyGameObject(GameObject* go);

    //----------------------------------------------------------------------
    // Find a gameobject with the given name in this scene.
    //----------------------------------------------------------------------
    GameObject* findGameObject(CString name);

private:
    StringID                        m_name;
    ArrayList<GameObject*>          m_gameObjects;

    //----------------------------------------------------------------------
    friend class Core::SceneManager;
    void _PreTick(Core::Time::Seconds delta);
    void _Tick(Core::Time::Seconds delta);
    void _LateTick(Core::Time::Seconds delta);

    //----------------------------------------------------------------------
    IScene(const IScene& other)               = delete;
    IScene& operator = (const IScene& other)  = delete;
    IScene(IScene&& other)                    = delete;
    IScene& operator = (IScene&& other)       = delete;
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
    void tick(Core::Time::Seconds delta) {}
    void shutdown() {}
};