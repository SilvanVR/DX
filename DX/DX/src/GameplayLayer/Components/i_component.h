#pragma once
/**********************************************************************
    class: IComponent (i_component.h)

    author: S. Hau
    date: December 17, 2017

**********************************************************************/

#include "Time/durations.h"

class GameObject;
namespace Core { namespace Graphics { class CommandBuffer; } }

namespace Components {

    //**********************************************************************
    class IComponent
    {
        friend class GameObject;

    public:
        IComponent() {}

        //----------------------------------------------------------------------
        GameObject*         getGameObject()         { return m_pGameObject; }
        const GameObject*   getGameObject() const   { return m_pGameObject; }

        bool                isActive()      const   { return m_isActive; }
        void                setActive(bool active)  { m_isActive = active; }

    protected:
        //----------------------------------------------------------------------
        // Will be called before this component ticks for the first time.
        //----------------------------------------------------------------------
        virtual void Init() {}

        virtual void PreTick(Time::Seconds delta) {}
        virtual void Tick(Time::Seconds delta) {}
        virtual void LateTick(Time::Seconds delta) {}

        virtual void Shutdown() {}

        virtual void recordGraphicsCommands(Core::Graphics::CommandBuffer& cmd, F32 lerp) {}

        //----------------------------------------------------------------------
        // Called immediately after the component was attached to a gameobject.
        //----------------------------------------------------------------------
        virtual void addedToGameObject(GameObject* go) {}

    private:
        GameObject* m_pGameObject   = nullptr;
        bool        m_bInitialized  = false;
        bool        m_isActive      = true;

        //----------------------------------------------------------------------
        void _SetGameObject(GameObject* go) { m_pGameObject = go; addedToGameObject( m_pGameObject ); }

        //----------------------------------------------------------------------
        IComponent(const IComponent& other)               = delete;
        IComponent& operator = (const IComponent& other)  = delete;
        IComponent(IComponent&& other)                    = delete;
        IComponent& operator = (IComponent&& other)       = delete;
    };
}
