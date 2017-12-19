#pragma once
/**********************************************************************
    class: GameState (game_state.h)

    author: S. Hau
    date: December 19, 2017

    - Consists of everything necessary to render one frame
    - The command buffers are send to the GPU for rendering
**********************************************************************/

#include "Core/Graphics/command_buffer.h"

class GameState
{
public:
    GameState() = default;

    void render();


private:
    

    //----------------------------------------------------------------------
    //GameState(const GameState& other)               = delete;
    //GameState& operator = (const GameState& other)  = delete;
    //GameState(GameState&& other)                    = delete;
    //GameState& operator = (GameState&& other)       = delete;
};