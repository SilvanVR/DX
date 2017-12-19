#include "c_model_renderer.h"
/**********************************************************************
    class: CModelRenderer (CModelRenderer.cpp)

    author: S. Hau
    date: December 19, 2017
**********************************************************************/

#include "../../gameobject.h"
#include "../c_transform.h"

namespace Components {


    void CModelRenderer::render()
    {
        auto transform = getGameObject()->getComponent<CTransform>();

    }


}