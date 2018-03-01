#pragma once
/**********************************************************************
    class: CTransform (ctransform.h)

    author: S. Hau
    date: December 17, 2017

**********************************************************************/

#include "i_component.h"

namespace Components {

    class CTransform : public IComponent
    {

    public:
        CTransform() {}

        void Tick(Time::Seconds delta) override;

    private:
        //XMFLOAT3 pos;

        //----------------------------------------------------------------------
        CTransform(const CTransform& other)               = delete;
        CTransform& operator = (const CTransform& other)  = delete;
        CTransform(CTransform&& other)                    = delete;
        CTransform& operator = (CTransform&& other)       = delete;
    };

}