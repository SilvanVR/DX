#pragma once
/**********************************************************************
    class: GameObject (gameobject.h)

    author: S. Hau
    date: December 17, 2017

    @TODO:
     - allow copying of gameobjects
**********************************************************************/

#include "Components/ctransform.h"

//**********************************************************************
class GameObject
{
    friend class Scene;

public:
    GameObject(){}

private:
    Scene* m_scene;

    void setScene(Scene* scene){ m_scene = scene; }

    //----------------------------------------------------------------------
    GameObject(const GameObject& other)               = delete;
    GameObject& operator = (const GameObject& other)  = delete;
    GameObject(GameObject&& other)                    = delete;
    GameObject& operator = (GameObject&& other)       = delete;
};