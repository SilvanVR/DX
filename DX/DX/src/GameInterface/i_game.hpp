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
    virtual void tick(F32 delta) = 0;

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
