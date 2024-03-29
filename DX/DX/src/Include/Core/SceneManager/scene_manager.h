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

#include "Common/i_subsystem.hpp"
#include "OS/FileSystem/path.h"

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

        U32                         numScenes() const   { return static_cast<U32>( m_sceneStack.size() ); }
        const ArrayList<IScene*>&   getScenes() const   { return m_sceneStack; }

        //----------------------------------------------------------------------
        // @Return: Current scene on top of the scene stack
        //----------------------------------------------------------------------
        IScene& getCurrentScene() { return *m_sceneStack.back(); }

        //----------------------------------------------------------------------
        // @Return: Loaded scene if any otherwise current scene on top of the scene stack
        //----------------------------------------------------------------------
        IScene& getCurrentSceneLoad() { return sceneIsLoading ? *sceneIsLoading : getCurrentScene(); }

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
        void LoadScene(IScene* scene) { PushScene(scene, true); }
        void LoadSceneAsync(IScene* scene) { PushSceneAsync(scene, true); }

        //----------------------------------------------------------------------
        void LoadScene(OS::Path path);
        void LoadSceneAsync(OS::Path path);
        void SaveCurrentSceneToFile(OS::Path path);

    private:
        ArrayList<IScene*> m_sceneStack;
        IScene*  sceneIsLoading = nullptr;  // Scene which is currently in the process of loading
        IScene*  sceneToLoad    = nullptr;  // If not null, we have to transition to this scene the next tick
        bool     popScene       = false;    // If true, pop current scene when transitioning to a new scene

        //----------------------------------------------------------------------
        void _SwitchToScene(IScene* newScene);
        void _PopScene();

        NULL_COPY_AND_ASSIGN(SceneManager)
    };


} // End namespaces