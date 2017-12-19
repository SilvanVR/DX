#include "scene_manager.h"
/**********************************************************************
    class: SceneManager (scene_manager.cpp)

    author: S. Hau
    date: December 17, 2017

    - New scenes will be initialized before the unitialization of the
      old scene so resources used in both scenes won't be unloaded.
    - Scene switching has to happen in tick(), otherwise it won't work
      e.g. in lamda-functions for button and asynchronously
**********************************************************************/

#include "locator.h"
#include "GameplayLayer/i_scene.h"

namespace Core {

    //----------------------------------------------------------------------
    static IScene*  sceneToLoad = nullptr;  // If not null, we have to transition to this scene the next tick
    static bool     popScene = false;    // If true, pop current scene when transitioning to a new scene

    //----------------------------------------------------------------------
    void SceneManager::init()
    {
        Locator::getCoreEngine().subscribe( this );
        m_sceneStack.reserve( 3 );
        m_sceneStack.push_back( new DefaultScene );
    }

    //----------------------------------------------------------------------
    void SceneManager::shutdown()
    {
        // Delete all remaining scenes
        for (IScene* scene : m_sceneStack)
        {
            scene->shutdown();
            delete scene;
        }
    }

    //----------------------------------------------------------------------
    void SceneManager::OnTick( Time::Seconds delta )
    {
        // Transition to new scene if requested
        if (sceneToLoad != nullptr)
        {
            _SwitchToScene( sceneToLoad );
            sceneToLoad = nullptr;
        }

        // Update all scenes in the scene-stack
        for (IScene* scene : m_sceneStack)
        {
            scene->_PreTick( delta );
            scene->_Tick( delta );
            scene->_LateTick( delta );
        }
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void SceneManager::PushScene( IScene* scene, bool popCurScene )
    {
        ASSERT( sceneToLoad == nullptr && "Only 1 Scene can be loaded in exactly one tick!" );

        scene->init();
        popScene    = popCurScene;
        sceneToLoad = scene;
    }

    //----------------------------------------------------------------------
    void SceneManager::PushSceneAsync( IScene* scene, bool popCurScene )
    {
        ASSERT( sceneToLoad == nullptr && "Only 1 Scene can be loaded in exactly one tick!" );

        ASYNC_JOB([scene, popCurScene, this] {
            scene->init();
            popScene    = popCurScene;
            sceneToLoad = scene;
        });
    }

    //----------------------------------------------------------------------
    void SceneManager::PopScene()
    {
        ASSERT( numScenes() > 0 && "No scene to delete exists." );
        IScene* curScene = m_sceneStack.back();
        curScene->shutdown();
        delete curScene;
        m_sceneStack.pop_back();
        popScene = false;
    }

    //----------------------------------------------------------------------
    void SceneManager::LoadScene( OS::Path path )
    {
        //@TODO: IMPLEMENT
        ASSERT(false);
    }

    //----------------------------------------------------------------------
    void SceneManager::LoadSceneAsync( OS::Path path )
    {
        //@TODO: IMPLEMENT
        ASSERT(false);
    }

    //----------------------------------------------------------------------
    void SceneManager::SaveCurrentSceneToFile( OS::Path path )
    {
        //@TODO: IMPLEMENT
        ASSERT(false);
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    void SceneManager::_SwitchToScene( IScene* newScene )
    {
        if (popScene)
            PopScene();

        // Add new scene
        m_sceneStack.push_back( newScene );
    }


} // End namespaces