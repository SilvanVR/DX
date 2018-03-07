#pragma once
/**********************************************************************
    class: ComponentManager (component_manager.h)

    author: S. Hau
    date: December 19, 2017

    @Description:
    - Creates all components
      > For different component types the method can be overriden

    @Considerations:
    - Store all components here aswell
      > For better data locality
    - Fetch memory from a preallocated pool for components
**********************************************************************/

#include "i_component.h"
#include "Rendering/model_renderer.h"
#include "Rendering/camera.h"
#include "Logging/logging.h"

namespace Components {

    //**********************************************************************
    class ComponentManager
    {
    public:
        ComponentManager()  = default;
        ~ComponentManager() = default;

        // <---------------------- COMPONENT STUFF ---------------------------->
        const ArrayList<Camera*>&       getCameras()    const { return m_pCameras; }
        const ArrayList<CRenderer*>&    getRenderer()   const { return m_pRenderer; }

        //----------------------------------------------------------------------
        // Creates a new component of type T
        //----------------------------------------------------------------------
        template<typename T, typename... Args> T* Create(Args&&... args)
        {
            return _Create<T>( std::forward<Args>( args )... );
        }

        //----------------------------------------------------------------------
        // Creates a new component of type T
        //----------------------------------------------------------------------
        template<typename T> void Destroy(T* component)
        {
            _Destroy<T>( component );
        }

    private:
        ArrayList<Camera*>      m_pCameras;
        ArrayList<CRenderer*>   m_pRenderer;

        //----------------------------------------------------------------------
        template <typename T, typename... Args> T*   _Create( Args&&... args );
        template <typename T>                   void _Destroy( T* component );

        //----------------------------------------------------------------------
        ComponentManager(const ComponentManager& other)               = delete;
        ComponentManager& operator = (const ComponentManager& other)  = delete;
        ComponentManager(ComponentManager&& other)                    = delete;
        ComponentManager& operator = (ComponentManager&& other)       = delete;
    };

    //**********************************************************************
    // TEMPLATE - PUBLIC
    //**********************************************************************

    //**********************************************************************
    // TEMPLATE - PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    template <typename T, typename... Args>
    T* ComponentManager::_Create( Args&&... args )
    {
        T* component = new T( std::forward<Args>( args )... );

        if constexpr( std::is_same<Camera, T>::value )
        {
            m_pCameras.push_back( component );
        }

        if constexpr( std::is_base_of<CRenderer, T>::value )
        {
            m_pRenderer.push_back( component );
        }

        return component;
    }

    //----------------------------------------------------------------------
    template <typename T>
    void ComponentManager::_Destroy( T* component )
    {
        if constexpr( std::is_same<Camera, T>::value )
        {
            m_pCameras.erase( std::remove( m_pCameras.begin(), m_pCameras.end(), component ) );
        }

        if constexpr( std::is_base_of<CRenderer, T>::value )
        {
            m_pRenderer.erase( std::remove( m_pRenderer.begin(), m_pRenderer.end(), component ) );
        }
    }

}