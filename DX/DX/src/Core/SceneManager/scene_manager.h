#pragma once
/**********************************************************************
    class: SceneManager (scene_manager.h)

    author: S. Hau
    date: December 17, 2017

    This class manages all scenes, in particular:
     - Scene Initialization
     - Scene Deinitialization
     - Scene Transitions

    All loaded scenes will be updated, but only the last one will be rendered.
**********************************************************************/

#include "Core/i_subsystem.hpp"
#include "Core/OS/FileSystem/path.h"

class IScene;

namespace Core {

    //**********************************************************************
    class SceneManager : public ISubSystem
    {
    public:
        SceneManager() = default;
        ~SceneManager() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;
        void OnTick(Time::Seconds delta) override;

        //----------------------------------------------------------------------
        IScene&     getCurrentScene()           { return *m_sceneStack.back(); }
        U32         numScenes()         const   { return static_cast<U32>( m_sceneStack.size() ); }

        //----------------------------------------------------------------------
        // Push a new scene onto the scene-stack.
        // @Params:
        //  "scene": The new scene to load.
        //  "popCurScene": If true, the current scene will be unloaded.
        //----------------------------------------------------------------------
        void PushScene(IScene* scene, bool popCurScene = true);

        //----------------------------------------------------------------------
        // Push a new scene onto the scene-stack. The scene will be loaded
        // asynchronously on a separate thread.
        // @Params:
        //  "scene": The new scene to load.
        //  "popCurScene": If true, the current scene will be unloaded.
        //----------------------------------------------------------------------
        void PushSceneAsync(IScene* scene, bool popCurScene = true);

        //----------------------------------------------------------------------
        // Pop the current scene from the scene stack.
        //----------------------------------------------------------------------
        void PopScene();

        //----------------------------------------------------------------------
        // Loads a new scene and unloads the current one. Equivalent to "PushScene( scene, true );"
        // @Params:
        //  "scene": The new scene to load.
        //----------------------------------------------------------------------
        void LoadScene(IScene* scene) { PushScene( scene, true ); }
        void LoadSceneAsync(IScene* scene) { PushSceneAsync(scene, true); }

        //----------------------------------------------------------------------
        void LoadScene(OS::Path path);
        void LoadSceneAsync(OS::Path path);
        void SaveCurrentSceneToFile(OS::Path path);

    private:
        ArrayList<IScene*> m_sceneStack;

        //----------------------------------------------------------------------
        void _SwitchToScene(IScene* newScene);

        //----------------------------------------------------------------------
        SceneManager(const SceneManager& other)               = delete;
        SceneManager& operator = (const SceneManager& other)  = delete;
        SceneManager(SceneManager&& other)                    = delete;
        SceneManager& operator = (SceneManager&& other)       = delete;
    };


} // End namespaces