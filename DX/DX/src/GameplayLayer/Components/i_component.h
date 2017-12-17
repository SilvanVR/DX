#pragma once
/**********************************************************************
    class: IComponent (i_component.h)

    author: S. Hau
    date: December 17, 2017

**********************************************************************/

//**********************************************************************
class IComponent
{
    friend class GameObject;

public:
    IComponent() {}

    GameObject* getGameObject() { return m_gameObject; }
    const GameObject* getGameObject() const { return m_gameObject; }

private:
    GameObject* m_gameObject;

    void _SetGameObject(GameObject* go) { m_gameObject = go; }

    //----------------------------------------------------------------------
    IComponent(const IComponent& other)               = delete;
    IComponent& operator = (const IComponent& other)  = delete;
    IComponent(IComponent&& other)                    = delete;
    IComponent& operator = (IComponent&& other)       = delete;
};