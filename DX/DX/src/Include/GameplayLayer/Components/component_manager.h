#pragma once
/**********************************************************************
    class: ComponentManager (component_manager.h)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

#include "Rendering/camera.h"
#include "Rendering/i_render_component.hpp"
#include "Rendering/i_light_component.h"

namespace Components {

    //**********************************************************************
    class ComponentManager
    {
    public:
        ComponentManager()  = default;
        ~ComponentManager() = default;

        // <---------------------- COMPONENT STUFF ---------------------------->
        const ArrayList<Camera*>&           getCameras()    const { return m_pCameras; }
        const ArrayList<IRenderComponent*>& getRenderer()   const { return m_pRenderer; }
        const ArrayList<ILightComponent*>&  getLights()     const { return m_pLights; }

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
        ArrayList<Camera*>              m_pCameras;
        ArrayList<IRenderComponent*>    m_pRenderer;
        ArrayList<ILightComponent*>     m_pLights;

        //----------------------------------------------------------------------
        template <typename T, typename... Args> T*   _Create( Args&&... args );
        template <typename T>                   void _Destroy( T* component );

        NULL_COPY_AND_ASSIGN(ComponentManager)
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

        if constexpr( std::is_base_of<IRenderComponent, T>::value )
        {
            m_pRenderer.push_back( component );
        }

        if constexpr( std::is_base_of<ILightComponent, T>::value )
        {
            m_pLights.push_back( component );
        }

        return component;
    }

    //----------------------------------------------------------------------
    template <typename T>
    void ComponentManager::_Destroy( T* component )
    {
        if (auto c = dynamic_cast<Camera*>( component ))
            m_pCameras.erase( std::remove( m_pCameras.begin(), m_pCameras.end(), c ) );

        if (auto r = dynamic_cast<IRenderComponent*>( component ))
            m_pRenderer.erase( std::remove( m_pRenderer.begin(), m_pRenderer.end(), r) );

        if (auto l = dynamic_cast<ILightComponent*>( component ))
            m_pLights.erase( std::remove( m_pLights.begin(), m_pLights.end(), l ) );
    }

}