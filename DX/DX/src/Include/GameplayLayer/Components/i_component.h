#pragma once
/**********************************************************************
    class: IComponent (i_component.h)

    author: S. Hau
    date: December 17, 2017

**********************************************************************/

#include "Time/durations.h"

class GameObject;
namespace Graphics { class CommandBuffer; };

namespace Components {

    //**********************************************************************
    class IComponent
    {
        friend class GameObject;

    public:
        IComponent() {}
        virtual ~IComponent() {}

        //----------------------------------------------------------------------
        inline GameObject*         getGameObject()         { return m_pGameObject; }
        inline const GameObject*   getGameObject() const   { return m_pGameObject; }

        inline void                setActive(bool active) { m_isActive = active; if (m_isActive) { onActive(); } else { onInActive(); }; }

        //----------------------------------------------------------------------
        // @Return:
        //  Whether this component is active. (P.S. if the attached gameobject is not active this returns also false!)
        //----------------------------------------------------------------------
        bool                        isActive()      const;

        //----------------------------------------------------------------------
        // @Return:
        // Component from attached gameobject. Nullptr if not found.
        // PS. THIS CAUSES A CIRCULAR COMPILER DEPENDENCY (Solving this is kinda annoying and takes some time though)
        //----------------------------------------------------------------------
        //template<typename T> inline T* getComponent() { return m_pGameObject->getComponent<T>(); }

    protected:
        //----------------------------------------------------------------------
        // Will be called before this component ticks for the first time.
        //----------------------------------------------------------------------
        virtual void init() {}

        virtual void preTick(Time::Seconds delta) {}
        virtual void tick(Time::Seconds delta) {}
        virtual void lateTick(Time::Seconds delta) {}

        virtual void shutdown() {}

        //----------------------------------------------------------------------
        // Called immediately after the component was attached to a gameobject.
        //----------------------------------------------------------------------
        virtual void addedToGameObject(GameObject* go) {}

        // Called when the components becomes active in the scene
        virtual void onActive() {}
        // Called when the components becomes inactive in the scene
        virtual void onInActive() {}

    private:
        GameObject* m_pGameObject   = nullptr;
        bool        m_bInitialized  = false;
        bool        m_isActive      = true;

        //----------------------------------------------------------------------
        void _SetGameObject(GameObject* go) { m_pGameObject = go; addedToGameObject( m_pGameObject ); }

        NULL_COPY_AND_ASSIGN(IComponent)
    };
}
