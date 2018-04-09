#pragma once
/**********************************************************************
    class: IGame (i_game.h)

    author: S. Hau
    date: October 27, 2017

    Inherit from this class and call start()
    to begin initializing the whole engine and run the core loop.

    Override init(), tick() and shutdown() for each game class.
**********************************************************************/

#include "Core/core_engine.h"
#include "GameplayLayer/i_scene.h"
#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/Components/transform.h"
#include "GameplayLayer/Components/Rendering/mesh_renderer.h"
#include "GameplayLayer/Components/Rendering/camera.h"
#include "GameplayLayer/Components/fps_camera.h"
#include "GameplayLayer/Components/skybox.h"
#include "Core/Assets/mesh_generator.h"

class IGame : public Core::CoreEngine
{
public:
    IGame() = default;
    virtual ~IGame() = default;

    //----------------------------------------------------------------------
    // Will be called after every subsystem has been initialized.
    //----------------------------------------------------------------------
    virtual void init() = 0;

    //----------------------------------------------------------------------
    // Tick will be called a fixed amount of times per second. (Default is 60x)
    //----------------------------------------------------------------------
    virtual void tick(Time::Seconds delta) = 0;

    //----------------------------------------------------------------------
    // Called after the main game loop has been terminated.
    //----------------------------------------------------------------------
    virtual void shutdown() = 0;

private:
    //----------------------------------------------------------------------
    IGame(const IGame& other)               = delete;
    IGame& operator = (const IGame& other)  = delete;
    IGame(IGame&& other)                    = delete;
    IGame& operator = (IGame&& other)       = delete;
};
