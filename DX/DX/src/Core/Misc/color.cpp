#include "color.h"

/**********************************************************************
    class: Color (color.cpp)

    author: S. Hau
    date: October 10, 2017

**********************************************************************/

//---------------------------------------------------------------------------
//  Defines
//---------------------------------------------------------------------------

#ifdef USE_BGRA_FORMAT

    #define B_COMPONENT_INDEX 0
    #define G_COMPONENT_INDEX 1
    #define R_COMPONENT_INDEX 2
    #define A_COMPONENT_INDEX 3

#else

    #define R_COMPONENT_INDEX 0
    #define G_COMPONENT_INDEX 1
    #define B_COMPONENT_INDEX 2
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
Color::Color() : m_components(~0)
{
}

//---------------------------------------------------------------------------
Color::Color( U32 color )
    : m_components(color)
{
}

//---------------------------------------------------------------------------
Color::Color( Byte r, Byte g, Byte b, Byte a )
{
    setRGBA( r, g, b, a );
}

//Color::Color(std::string hex)
//{
//    if (hex[0] != '#' || hex.length() != 7)
//    {
//        Logger::Log("Color::Color(): Given string '" + hex + "' is not a valid hexadecimal number."
//            " Using #FFFFFF (White) instead", LOGTYPE_WARNING);
//        hex = "#FFFFFF";
//    }
//    // Cut of "#" character
//    hex = hex.erase(0, 1);
//
//    r() = hexToRGB(hex.substr(0, 2)) * 1 / 255.0f;
//    g() = hexToRGB(hex.substr(2, 2)) * 1 / 255.0f;
//    b() = hexToRGB(hex.substr(4, 2)) * 1 / 255.0f;
//    a() = 1.0f;
//}

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
    Byte* address = reinterpret_cast<Byte*>( &m_components ) + R_COMPONENT_INDEX;
    memcpy( address, &red, sizeof(Byte) );
}

//---------------------------------------------------------------------------
void Color::setGreen( Byte green )
{
    Byte* address = reinterpret_cast<Byte*>( &m_components ) + G_COMPONENT_INDEX;
    memcpy( address, &green, sizeof(Byte) );
}

//---------------------------------------------------------------------------
void Color::setBlue( Byte blue )
{
    Byte* address = reinterpret_cast<Byte*>( &m_components ) + B_COMPONENT_INDEX;
    memcpy( address, &blue, sizeof(Byte) );
}

//---------------------------------------------------------------------------
void Color::setAlpha( Byte alpha )
{
    Byte* address = reinterpret_cast<Byte*>( &m_components ) + A_COMPONENT_INDEX;
    memcpy( address, &alpha, sizeof(Byte) );
}

//---------------------------------------------------------------------------
Byte Color::getRed() const
{
    return RED_COMPONENT( m_components );
}

//---------------------------------------------------------------------------
Byte Color::getGreen() const
{
    return GREEN_COMPONENT( m_components );
}

//---------------------------------------------------------------------------
Byte Color::getBlue() const
{
    return BLUE_COMPONENT( m_components );
}

//---------------------------------------------------------------------------
Byte Color::getAlpha() const
{
    return ALPHA_COMPONENT( m_components );
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

F32 Color::getLuminance() const
{
    return (0.2126f * getRed() + 0.7152f * getGreen() + 0.0722f * getBlue() );
}

//---------------------------------------------------------------------------
bool Color::operator==( const Color& c ) const
{
    return (m_components == c.m_components);
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
F32 Color::_HexToRGB(const char* hex)
{
   // int hexLength = static_cast<int>(hex.length());
    int hexLength = 0;
    double dec = 0;

    for (int i = 0; i < hexLength; ++i)
    {
        char b = hex[i];

        if (b >= 48 && b <= 57)
            b -= 48;
        else if (b >= 65 && b <= 70)
            b -= 55;

        dec += b * pow(16, ((hexLength - i) - 1));
    }
    return (F32)dec;
}

