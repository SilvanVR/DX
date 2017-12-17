#pragma once
/**********************************************************************
    class: IScene + DefaultScene (i_scene.h)

    author: S. Hau
    date: December 17, 2017

    - Every scene consists of arbitrary gameobjects
    - Only one scene can be rendered at once
**********************************************************************/

#include "Core/Time/durations.h"

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
    virtual void preTick(Core::Time::Seconds delta) {}
    virtual void tick(Core::Time::Seconds delta) = 0;
    virtual void lateTick(Core::Time::Seconds delta) {}
    virtual void shutdown() = 0;

    //----------------------------------------------------------------------
    const String& getName() const { return m_name; }

private:
    String m_name;

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