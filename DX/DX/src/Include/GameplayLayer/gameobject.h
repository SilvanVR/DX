#pragma once
/**********************************************************************
    class: GameObject (gameobject.h)

    author: S. Hau
    date: December 17, 2017
**********************************************************************/

#include "Components/i_component.h"
#include "Components/transform.h"
#include "Logging/logging.h"
#include "GameplayLayer/layers.hpp"

//----------------------------------------------------------------------
template<typename T>
Hash TypeHash()
{
    static Hash hash = typeid(T).hash_code();
    return hash;
}

class IScene;

//**********************************************************************
class GameObject
{
public:
    ~GameObject();

    //----------------------------------------------------------------------
    const StringID       getName()       const              { return m_name; }
    bool                 isActive()      const              { return m_isActive; }
    const LayerMask      getLayerMask()  const              { return m_layerMask; }
    IScene*              getScene()                         { return m_attachedScene; }

    void                 setActive      (bool active)           { m_isActive = active; }
    void                 setLayerMask   (LayerMask layerMask)   { m_layerMask = layerMask; }
    void                 addLayer       (Layer layer)           { m_layerMask |= layer; }
    void                 removeLayer    (Layer layer)           { m_layerMask &= ~((LayerMask)layer); }

    // <---------------------- COMPONENT STUFF ---------------------------->
    template<typename T> T*   getComponent();
    template<typename T> bool removeComponent();
    template<typename T> bool removeComponent(T* comp);
    template<typename T, typename... Args> T* addComponent(Args&&... args);

    template<typename T> ArrayList<T*> getComponents();

    //----------------------------------------------------------------------
    // Retrieve the transform component directly. This is faster than using getComponent<T>()
    //----------------------------------------------------------------------
    inline Components::Transform* getTransform(){ return reinterpret_cast<Components::Transform*>(m_components[TypeHash<Components::Transform>()]); }

private:
    StringID            m_name;
    IScene*             m_attachedScene = nullptr;
    bool                m_isActive = true;
    LayerMask           m_layerMask = LAYER_DEFAULT;

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

    NULL_COPY_AND_ASSIGN(GameObject)
};

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
    if ( m_components.find( hash ) == m_components.end() )
        return nullptr;

    // Since we know the component exist a reinterpret cast suffice (instead of a more costly dynamic cast)
    return reinterpret_cast<T*>( m_components[hash] );
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
    return removeComponent<T>();
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
        LOG_WARN( "GameObject::addComponent(): Component already exists. Adding the same component to a single gameobject is not allowed." );
        return nullptr;
    }

    T* component = _CreateComponent<T>( args... );
    component->_SetGameObject( this );
    return component;
}

//----------------------------------------------------------------------
// @Return: All components with type T.
//----------------------------------------------------------------------
template<typename T> 
ArrayList<T*> GameObject::getComponents()
{
    ArrayList<T*> componentsOfTypeT;
    for (auto pair : m_components)
    {
        if (T* comp = dynamic_cast<T*>( pair.second ))
            componentsOfTypeT.push_back( comp );
    }
    return componentsOfTypeT;
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