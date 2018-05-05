#pragma once
/**********************************************************************
    class: ComponentManager (component_manager.h)

    author: S. Hau
    date: December 19, 2017

    - Creates and Stores all components
    - Updates all components
**********************************************************************/

#include "i_component.h"

namespace Components {

    //**********************************************************************
    class ComponentManager
    {
        friend class GameObject; /* Can create components */

    public:
        ComponentManager()  = default;
        ~ComponentManager() = default;

        // <---------------------- COMPONENT STUFF ---------------------------->
        template<typename T> T*   getComponent();
        template<typename T> bool removeComponent();
        template<typename T> bool removeComponent(T* comp);
        template<typename T, typename... Args> bool addComponent(Args&&... args);

    private:
        

        //----------------------------------------------------------------------
        // Creates the component in memory.
        template<typename T, typename... Args> T* _CreateComponent( GameObject* go, Args&&... args );

        // Initiates the destruction of the component.
        template<typename T> void _DestroyComponent( GameObject* go, T* comp );

        //----------------------------------------------------------------------
        ComponentManager(const ComponentManager& other)               = delete;
        ComponentManager& operator = (const ComponentManager& other)  = delete;
        ComponentManager(ComponentManager&& other)                    = delete;
        ComponentManager& operator = (ComponentManager&& other)       = delete;
    };


    //**********************************************************************
    // TEMPLATE - PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    // Retrieve the component with type T. Nullptr if not found.
    //----------------------------------------------------------------------
    template<typename T>
    T* ComponentManager::getComponent()
    {
        for (IComponent* component : m_components)
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
    bool ComponentManager::removeComponent()
    {
        for (IComponent* component : m_components)
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
    bool ComponentManager::removeComponent( T* comp )
    {
        for (IComponent* component : m_components)
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
    bool ComponentManager::addComponent( Args&&... args )
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
    T* ComponentManager::_CreateComponent( GameObject* go, Args&&... args )
    {
        T* component = new T( std::forward<Args>( args )... );
        m_components.push_back( component );
        return component;
    }

    //----------------------------------------------------------------------
    template<typename T>
    void ComponentManager::_DestroyComponent( GameObject* go, T* comp )
    {
        m_components.erase( std::remove( m_components.begin(), m_components.end(), comp ), m_components.end() );
        SAFE_DELETE( comp );
    }

}