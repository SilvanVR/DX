#pragma once
/**********************************************************************
    class: None (event_names.h)

    author: S. Hau
    date: April 2, 2018
**********************************************************************/

#define EVENT_SCENE_CHANGED     SID( "OnSceneChanged" )
#define EVENT_WINDOW_RESIZE     SID( "OnWindowSizeChanged" )
#define EVENT_GAME_START        SID( "OnGameStart" )
#define EVENT_GAME_SHUTDOWN     SID( "OnGameShutdown" )
#define EVENT_FRAME_BEGIN       SID( "OnFrameBegin" )
#define EVENT_FRAME_END         SID( "OnFrameEnd" )
#define EVENT_HMD_FOCUS_GAINED  SID( "OnHMDFocusGained" )
#define EVENT_HMD_FOCUS_LOST    SID( "OnHMDFocusLost" )
#define EVENT_HMD_SHOULD_QUIT   SID( "OnHMDShouldQuit" )