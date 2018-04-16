#include "color.h"

/**********************************************************************
    class: Color (color.cpp)

    author: S. Hau
    date: October 10, 2017

**********************************************************************/

//---------------------------------------------------------------------------
//  Defines
//---------------------------------------------------------------------------

#ifdef USE_RGBA_FORMAT

    #define R_COMPONENT_INDEX 0
    #define G_COMPONENT_INDEX 1
    #define B_COMPONENT_INDEX 2
    #define A_COMPONENT_INDEX 3

#else

    #define B_COMPONENT_INDEX 0
    #define G_COMPONENT_INDEX 1
    #define R_COMPONENT_INDEX 2
    #define A_COMPONENT_INDEX 3

#endif

#define RED_COMPONENT(x)      ( (x >> (R_COMPONENT_INDEX * 8)) & 0xFF )
#define GREEN_COMPONENT(x)    ( (x >> (G_COMPONENT_INDEX * 8)) & 0xFF )
#define BLUE_COMPONENT(x)     ( (x >> (B_COMPONENT_INDEX * 8)) & 0xFF )
#define ALPHA_COMPONENT(x)    ( (x >> (A_COMPONENT_INDEX * 8)) & 0xFF )

//---------------------------------------------------------------------------
//  Statics
//---------------------------------------------------------------------------

Color Color::WHITE(255, 255, 255);
Color Color::BLACK(0, 0, 0);
Color Color::RED(255, 0, 0);
Color Color::GREEN(0, 255, 0);
Color Color::BLUE(0, 0, 255);
Color Color::YELLOW(255, 255, 0);
Color Color::VIOLET(255, 0, 255);
Color Color::TURQUOISE(0, 255, 255);
Color Color::GREY(127, 127, 127);
Color Color::ORANGE(255, 127, 0);

//---------------------------------------------------------------------------
Color::Color() : m_componentsAsInt(~0)
{
}

//---------------------------------------------------------------------------
Color::Color( U32 color )
    : m_componentsAsInt(color)
{
    setAlpha( 255 );
}

//---------------------------------------------------------------------------
Color::Color( Byte r, Byte g, Byte b, Byte a )
{
    setRGBA( r, g, b, a );
}

//---------------------------------------------------------------------------
void Color::setRGBA(Byte r, Byte g, Byte b, Byte a)
{
    setRed( r );
    setGreen( g );
    setBlue( b );
    setAlpha( a );
}

//---------------------------------------------------------------------------
void Color::setRed( Byte red )
{
    m_components[R_COMPONENT_INDEX] = red;
}

//---------------------------------------------------------------------------
void Color::setGreen( Byte green )
{
    m_components[G_COMPONENT_INDEX] = green;
}

//---------------------------------------------------------------------------
void Color::setBlue( Byte blue )
{
    m_components[B_COMPONENT_INDEX] = blue;
}

//---------------------------------------------------------------------------
void Color::setAlpha( Byte alpha )
{
    m_components[A_COMPONENT_INDEX] = alpha;
}

//---------------------------------------------------------------------------
Byte Color::getRed() const
{
    return RED_COMPONENT( m_componentsAsInt );
}

//---------------------------------------------------------------------------
Byte Color::getGreen() const
{
    return GREEN_COMPONENT( m_componentsAsInt );
}

//---------------------------------------------------------------------------
Byte Color::getBlue() const
{
    return BLUE_COMPONENT( m_componentsAsInt );
}

//---------------------------------------------------------------------------
Byte Color::getAlpha() const
{
    return ALPHA_COMPONENT( m_componentsAsInt );
}

//---------------------------------------------------------------------------
Byte Color::getMax() const
{
    Byte max;

    Byte red    = getRed();
    Byte green  = getGreen();
    Byte blue   = getBlue();

    max = red > green ? red : green;
    max = max > blue ? max : blue;

    return max;
}

//---------------------------------------------------------------------------
F32 Color::getLuminance() const
{
    return (0.2126f * getRed() + 0.7152f * getGreen() + 0.0722f * getBlue() );
}

//---------------------------------------------------------------------------
bool Color::operator==( const Color& c ) const
{
    return (m_componentsAsInt == c.m_componentsAsInt);
}

//---------------------------------------------------------------------------
bool Color::operator!=( const Color& c ) const
{
    return not (*this == c);
}

//---------------------------------------------------------------------------
String Color::toString( bool includeAlpha ) const
{
    String result = "(" + TS( getRed() ) + ", " + TS( getGreen() ) + ", " + TS( getBlue() );

    if (includeAlpha)
        result += ", " + TS( getAlpha() );

    result += ")";

    return result;
}

//---------------------------------------------------------------------------
Color Color::Lerp( Color a, Color b, F32 lerp )
{
    return Color(Byte( a.getRed()     * (1.0f - lerp) + b.getRed()    * lerp + 0.5f ) ,
                 Byte( a.getGreen()   * (1.0f - lerp) + b.getGreen()  * lerp + 0.5f ) ,
                 Byte( a.getBlue()    * (1.0f - lerp) + b.getBlue()   * lerp + 0.5f ) ,
                 Byte( a.getAlpha()   * (1.0f - lerp) + b.getAlpha()  * lerp + 0.5f ) );
}