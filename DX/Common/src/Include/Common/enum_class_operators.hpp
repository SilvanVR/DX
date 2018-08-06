#pragma once
/**********************************************************************
    class: None

    author: S. Hau
    date: August 6, 2018

    Enables the usage of "enum classes" for type safe bitfields:
    enum class Flags
    {
    None = 0,       // Important to define a "zero" constant for "if's". See below.
    One = 1 << 0,
    Two = 1 << 1
    }
    ENABLE_BITMASK_OPERATORS(Flags)

    Flags flags = Flags::One | Flags::Two;
    if ( (flags & Flags::One) != Flags::Zero )
        // Do something

    Honor to http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
**********************************************************************/

#include <type_traits>

template<typename Enum>
struct EnableBitMaskOperators
{
    static const bool enable = false;
};

#define ENABLE_BITMASK_OPERATORS(x)  \
template<>                           \
struct EnableBitMaskOperators<x>     \
{                                    \
    static const bool enable = true; \
};

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
constexpr operator |(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
        static_cast<underlying>(lhs) |
        static_cast<underlying>(rhs)
        );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type
constexpr operator |=(Enum& lhs, Enum rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
constexpr operator &(Enum lhs, Enum rhs)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
        static_cast<underlying>(lhs) &
        static_cast<underlying>(rhs)
        );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type
constexpr operator &=(Enum& lhs, Enum rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
constexpr operator ~(Enum e)
{
    using underlying = typename std::underlying_type<Enum>::type;
    return static_cast<Enum> (
        ~static_cast<underlying>(e)
        );
}