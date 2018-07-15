#pragma once
/**********************************************************************
    class: Random (random.h)

    author: S. Hau
    date: March 11, 2018

    Using the C++ 11 <random> header file.
    Using the standard engine. (For more information see https://isocpp.org/files/papers/n3551.pdf) //possible better: std::mt19937
**********************************************************************/

#include <random>
#include "Common/color.h"
#include "Math/dxmath_wrapper.h"

namespace Math
{
    //---------------------------------------------------------------------------
    class Random
    {
    private:
        static std::default_random_engine engine; 

    public:
        // Returns an random Integer between [min,max].
        static  I32         Int(I32 min, I32 max);

        // Returns an random Integer between [0,max].
        static  I32         Int(I32 max);

        // Returns an random Float between [min,max].
        static  F32         Float(F32 min, F32 max);

        // Returns an random Float between [0,max].
        static  F32         Float(F32 max);

        // Returns an random Double between [min,max].
        static  F64         Double(F64 min, F64 max);

        // Returns an random Double between [0,max].
        static  F64         Double(F64 max);

        // Returns an random vector with all components between [min,max].
        static  Math::Vec3  Vec3(F32 min, F32 max);

        // Returns an random (normalized) quaternion.
        static  Math::Quat  Quat();

        // Returns a random color.
        static  Color       Color(bool randomAlpha = false);

        // Returns a value between [0,1].
        template <typename T> static T  value();

        // Returns a value between [min,max].
        template <typename T> static T  value(T min, T max);
    };

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //---------------------------------------------------------------------------
    inline I32 Random::Int( I32 min, I32 max )
    {
        return std::uniform_int_distribution<I32>{ min, max }( engine );
    }

    //---------------------------------------------------------------------------
    inline I32 Random::Int( I32 max = 1 )
    {
        return Int( 0, max );
    }

    //---------------------------------------------------------------------------
    inline F32 Random::Float( F32 min, F32 max )
    {
        return std::uniform_real_distribution<F32>{ min, max }( engine );
    }

    //---------------------------------------------------------------------------
    inline F32 Random::Float( F32 max = 1 )
    {
        return Float( 0, max );
    }

    //---------------------------------------------------------------------------
    inline F64 Random::Double( F64 min, F64 max )
    {
        return std::uniform_real_distribution<F64>{ min, max }( engine );
    }

    //---------------------------------------------------------------------------
    inline F64 Random::Double(F64 max = 1)
    {
        return Double( 0, max );
    }

    //---------------------------------------------------------------------------
    inline Math::Vec3 Random::Vec3( F32 min, F32 max )
    {
        auto distribution = std::uniform_real_distribution<F32>{ min, max };
        return Math::Vec3( distribution( engine ), distribution( engine ), distribution( engine ) );
    }

    //---------------------------------------------------------------------------
    inline Math::Quat Random::Quat()
    {
        auto distribution = std::uniform_real_distribution<F32>{ 0, 1 };
        return Math::Quat( distribution( engine ), distribution( engine ), distribution( engine ), distribution( engine ) ).normalized();
    }

    //---------------------------------------------------------------------------
    template <typename T> inline
    T Random::value()
    {
        return std::uniform_real_distribution<T>{ 0, 1 }(engine);
    }

    //---------------------------------------------------------------------------
    template <> inline
    I32 Random::value()
    {
        return std::uniform_int_distribution<I32>{ 0, 1 }(engine);
    }

    //---------------------------------------------------------------------------
    template <typename T> inline
    T Random::value( T min, T max )
    {
        return std::uniform_real_distribution<T>{ min, max }(engine);
    }

    //---------------------------------------------------------------------------
    template<> inline
    ::Color Random::value( ::Color min, ::Color max )
    {
        Byte minR = std::min( min.getRed(), max.getRed() ); Byte maxR = std::max( min.getRed(), max.getRed() );
        Byte r = Int( minR, maxR );

        Byte minG = std::min( min.getGreen(), max.getGreen() ); Byte maxG = std::max( min.getGreen(), max.getGreen() );
        Byte g = Int( minG, maxG );

        Byte minB = std::min( min.getBlue(), max.getBlue() ); Byte maxB = std::max( min.getBlue(), max.getBlue() );
        Byte b = Int( minB, maxB );

        Byte minA = std::min( min.getAlpha(), max.getAlpha() ); Byte maxA = std::max( min.getAlpha(), max.getAlpha() );
        Byte a = Int( minA, maxA );

        return ::Color( r, g, b, a );
    }

    //---------------------------------------------------------------------------
    inline Color Random::Color( bool randomAlpha )
    {
        Byte r = Int( 255 );
        Byte g = Int( 255 );
        Byte b = Int( 255 );

        return ::Color( r, g, b, randomAlpha ? Int( 255 ) : 255 );
    }

}