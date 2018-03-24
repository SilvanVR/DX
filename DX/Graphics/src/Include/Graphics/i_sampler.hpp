#pragma once
/**********************************************************************
    class: IISampler (i_sampler.hpp)

    author: S. Hau
    date: March 24, 2018
**********************************************************************/

namespace Graphics
{

    //----------------------------------------------------------------------
    enum class TextureAddressMode
    {
        WRAP        = 1,
        MIRROR      = 2,
        CLAMP       = 3,
        BORDER      = 4,
        MIRROR_ONCE = 5
    };

    enum class Filter
    {
        MIN_MAG_MIP_POINT = 0,
        MIN_MAG_POINT_MIP_LINEAR = 0x1,
        MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
        MIN_POINT_MAG_MIP_LINEAR = 0x5,
        MIN_LINEAR_MAG_MIP_POINT = 0x10,
        MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
        MIN_MAG_LINEAR_MIP_POINT = 0x14,
        MIN_MAG_MIP_LINEAR = 0x15,
        ANISOTROPIC = 0x55,
        COMPARISON_MIN_MAG_MIP_POINT = 0x80,
        COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
        COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
        COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
        COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
        COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
        COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
        COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
        COMPARISON_ANISOTROPIC = 0xd5,
        MINIMUM_MIN_MAG_MIP_POINT = 0x100,
        MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
        MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
        MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
        MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
        MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
        MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
        MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
        MINIMUM_ANISOTROPIC = 0x155,
        MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
        MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
        MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
        MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
        MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
        MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
        MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
        MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
        MAXIMUM_ANISOTROPIC = 0x1d5
    };

    //**********************************************************************
    class ISampler
    {
    public:
        ISampler() = default;
        ~ISampler(){}


    private:


        //----------------------------------------------------------------------
        ISampler(const ISampler& other)               = delete;
        ISampler& operator = (const ISampler& other)  = delete;
        ISampler(ISampler&& other)                    = delete;
        ISampler& operator = (ISampler&& other)       = delete;
    };

    using Sampler = ISampler;
}