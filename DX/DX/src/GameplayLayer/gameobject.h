#pragma once
/**********************************************************************
    class: GameObject (gameobject.h)

    author: S. Hau
    date: December 17, 2017

    @TODO:
     - allow copying of gameobjects
**********************************************************************/

#include "Components/i_component.h"
#include "locator.h"

//**********************************************************************
class GameObject
{
    friend class IScene;
    GameObject(IScene* scene, CString name);

public:
    ~GameObject();

    //----------------------------------------------------------------------
    const StringID                  getName()       const   { return m_name; }
    IScene*                         getScene()              { return m_attachedScene; }
    bool                            isActive()      const   { return m_isActive; }
    void                            setActive(bool active)  { m_isActive = active;}

    const ArrayList<Components::IComponent*>& getComponents() const { return m_components; }

    // <---------------------- COMPONENT STUFF ---------------------------->
    template<typename T> T*   getComponent();
    template<typename T> bool removeComponent();
    template<typename T> bool removeComponent(T* comp);
    template<typename T, typename... Args> bool addComponent(Args&&... args);


private:
    StringID                            m_name;
    IScene*                             m_attachedScene;
    ArrayList<Components::IComponent*>  m_components;
    bool                                m_isActive = true;

    //----------------------------------------------------------------------
    // Creates the component in memory.
    template<typename T, typename... Args> T* _CreateComponent( Args&&... args );

    // Initiates the destruction of the component.
    template<typename T> void _DestroyComponent( T* comp );

    //----------------------------------------------------------------------
    GameObject(const GameObject& other)               = delete;
    GameObject& operator = (const GameObject& other)  = delete;
    GameObject(GameObject&& other)                    = delete;
    GameObject& operator = (GameObject&& other)       = delete;
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
    for (auto component : m_components)
    {
        if ( T* c = dynamic_cast<T*>( component ) )
            return c;
    }
    return nullptr;
}

//----------------------------------------------------------------------
// Remove the component with type T. False if not found.
//----------------------------------------------------------------------
template <typename T>
bool GameObject::removeComponent()
{
    for (auto component : m_components)
    {
        if ( T* c = dynamic_cast<T*>( component ) )
        {
            _DestroyComponent( c );
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------
// Remove the given component. True if successful, otherwise false.
//----------------------------------------------------------------------
template <typename T>
bool GameObject::removeComponent( T* comp )
{
    for (auto component : m_components)
    {
        if ( comp == component )
        {
            _DestroyComponent( comp );
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------
// Add a new component with type T to this gameobject. False if not
// successful, e.g. the component-type was already present.
//----------------------------------------------------------------------
template<typename T, typename... Args>
bool GameObject::addComponent( Args&&... args )
{
    T* foundComp = getComponent<T>();
    if (foundComp != nullptr)
    {
        WARN( "GameObject::addComponent(): Duplicated component type." );
        return false;
    }

    T* component = _CreateComponent<T>( args... );
    component->_SetGameObject( this );
    return true;
}

//**********************************************************************
// TEMPLATE - PRIVATE
//**********************************************************************

//----------------------------------------------------------------------
template<typename T, typename... Args>
T* GameObject::_CreateComponent( Args&&... args )
{
    //@TODO: More sophisticated allocation scheme for components
    T* component = new T( std::forward<Args>( args )... );
    m_components.push_back( component );
    return component;
}

//----------------------------------------------------------------------
template<typename T>
void GameObject::_DestroyComponent( T* comp )
{
    m_components.erase( std::remove( m_components.begin(), m_components.end(), comp ), m_components.end() );
    SAFE_DELETE( comp );
}