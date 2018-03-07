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

        //----------------------------------------------------------------------
        // Creates a new component of type T
        //----------------------------------------------------------------------
        template<typename T, typename... Args> static T* Create(Args&&... args)
        {
            T* component = new T( std::forward<Args>( args )... );
            return component;
        }

    private:

        //----------------------------------------------------------------------
        template <typename T, typename... Args, typename std::enable_if<std::is_base_of<CRenderer, T>::value>::type* = nullptr>
        static T* _Create(Args&&... args) 
        {
            LOG("CRENDERER", Color::RED);
            T* mr = new T(std::forward<Args>(args)...);
            return mr;
        }

        template <typename T, typename... Args, typename std::enable_if<std::is_same<Camera, T>::value>::type* = nullptr>
        static T* _Create(Args&&... args)
        {
            LOG("CAMERA", Color::RED);
            T* mr = new T(std::forward<Args>(args)...);
            return mr;
        }


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
    //template<typename T, typename... Args>
    //T* ComponentManager::Create( Args&&... args )
    //{
    //    //@TODO: More sophisticated allocation scheme for components
    //    return _Create<T>( std::forward<Args>( args )... );
    //}

    //**********************************************************************
    // TEMPLATE - PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    //template<typename T, typename... Args>
    //T* ComponentManager::_Create( Args&&... args )
    //{
    //    LOG("CREATED COMPONENT", Color::BLUE);
    //    T* component = new T( std::forward<Args>( args )... );
    //    return component;
    //}

    ////----------------------------------------------------------------------
    //template<ModelRenderer, typename... Args>
    //ModelRenderer* ComponentManager::_Create( Args&&... args )
    //{
    //    LOG("MODEL RENDERER CREATE", Color::RED);
    //    ModelRenderer* mr = new ModelRenderer( std::forward<Args>( args )... );
    //    return mr;
    //}

}