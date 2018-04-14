#pragma once
/**********************************************************************
    class: GameObject (gameobject.h)

    author: S. Hau
    date: December 17, 2017

    @TODO:
     - allow copying of gameobjects
**********************************************************************/

#include "Components/i_component.h"
#include "Logging/logging.h"

class IScene;

//**********************************************************************
class GameObject
{
public:
    ~GameObject();

    //----------------------------------------------------------------------
    inline const StringID      getName()       const   { return m_name; }
    inline IScene*             getScene()              { return m_attachedScene; }
    inline bool                isActive()      const   { return m_isActive; }
    inline void                setActive(bool active)  { m_isActive = active; }

    // <---------------------- COMPONENT STUFF ---------------------------->
    template<typename T> T*   getComponent();
    template<typename T> bool removeComponent();
    template<typename T> bool removeComponent(T* comp);
    template<typename T, typename... Args> T* addComponent(Args&&... args);

private:
    StringID m_name;
    IScene*  m_attachedScene = nullptr;
    bool     m_isActive = true;

    std::unordered_map<Hash, Components::IComponent*> m_components;

    //----------------------------------------------------------------------
    friend class IScene;
    GameObject(IScene* scene, CString name);
    void _PreTick(Time::Seconds delta);
    void _Tick(Time::Seconds delta);
    void _LateTick(Time::Seconds delta);

    //----------------------------------------------------------------------
    // Creates the component in memory.
    template<typename T, typename... Args> T* _CreateComponent( Args&&... args );

    // Initiates the destruction of the component.
    template<typename T> void _DestroyComponent( Size hash );

    //----------------------------------------------------------------------
    GameObject(const GameObject& other)               = delete;
    GameObject& operator = (const GameObject& other)  = delete;
    GameObject(GameObject&& other)                    = delete;
    GameObject& operator = (GameObject&& other)       = delete;
};

//----------------------------------------------------------------------
template<typename T>
Hash TypeHash()
{
    static Hash hash = typeid(T).hash_code();
    return hash;
}

//**********************************************************************
// TEMPLATE - PUBLIC
//**********************************************************************

//----------------------------------------------------------------------
// Retrieve the component with type T. Nullptr if not found.
//----------------------------------------------------------------------
template<typename T>
T* GameObject::getComponent()
{
    Size hash = TypeHash<T>();
    if (m_components.count( hash ) == 0)
        return nullptr;

    return dynamic_cast<T*>( m_components[hash] );
}

//----------------------------------------------------------------------
// Remove the component with type T. False if not found.
//----------------------------------------------------------------------
template <typename T>
bool GameObject::removeComponent()
{
    Size hash = TypeHash<T>();
    if (m_components.count( hash ) == 0)
        return false;

    _DestroyComponent<T>( hash );
    return true;
}

//----------------------------------------------------------------------
// Remove the given component. True if successful, otherwise false.
//----------------------------------------------------------------------
template <typename T>
bool GameObject::removeComponent( T* comp )
{
    removeComponent<T>();
}

//----------------------------------------------------------------------
// Add a new component with type T to this gameobject. Returns the newly added component.
// Issues a warning and returns nullptr if component type was already present.
//----------------------------------------------------------------------
template<typename T, typename... Args>
T* GameObject::addComponent( Args&&... args )
{
    Size hash = TypeHash<T>();
    if (m_components.count( hash ) != 0)
    {
        WARN( "GameObject::addComponent(): Component already exists. Adding the same component to a single gameobject is not allowed." );
        return nullptr;
    }

    T* component = _CreateComponent<T>( args... );
    component->_SetGameObject( this );
    return component;
}

//**********************************************************************
// TEMPLATE - PRIVATE
//**********************************************************************

//----------------------------------------------------------------------
template<typename T, typename... Args>
T* GameObject::_CreateComponent( Args&&... args )
{
    T* component = m_attachedScene->getComponentManager().Create<T>( std::forward<Args>( args )... );
    Size hash = TypeHash<T>();
    m_components[hash] = component;
    return component;
}

//----------------------------------------------------------------------
template<typename T>
void GameObject::_DestroyComponent( Size hash )
{
    T* comp = dynamic_cast<T*>( m_components[hash] );
    comp->shutdown();
    m_attachedScene->getComponentManager().Destroy<T>( comp );
    m_components.erase( hash );
    SAFE_DELETE( comp );
}