#pragma once

/**********************************************************************
    class: Color (color.h)

    author: S. Hau
    date: October 10, 2017

    One color component is represented by exactly one byte. This means 
    the range for each component is 0 - 255. Which format is used 
    depends upon appropriate defines during compilation:
      #define USE_RGBA_FORMAT: Color in RGBA format.
    The default format is BGRA.
**********************************************************************/

#include <array>

class Color
{
    union
    {
        Byte    m_components[4];
        U32     m_componentsAsInt;
    };

public:
    Color();
    explicit Color(Byte r, Byte g, Byte b, Byte a = 255);

    //----------------------------------------------------------------------
    // Construct a new color based on the internal representation.
    // Default is BGRA, that means 0xFF0000 = RED, 0x0000FF = BLUE etc.
    // Alpha will be always set to 255.
    //----------------------------------------------------------------------
    explicit Color(U32 color);

    //----------------------------------------------------------------------
    // Constructs a new color from a hexadecimal representation.
    // 0xFF0000 = RED, 0x000000FF = ALPHA set to 1
    // This constructor is independent of the underlying format (BGRA or RGBA).
    //----------------------------------------------------------------------
    explicit Color(const String& hex);

    //----------------------------------------------------------------------
    // Return the greatest of the three R-G-B components (Alpha excluded).
    // @Return:
    //  Value representing the largest component. Range: [0 - 255]
    //----------------------------------------------------------------------
    Byte getMax() const;

    //----------------------------------------------------------------------
    // Calculate the luminance values based on the RGB-Components.
    // @Return:
    //  Luminance value represented by the RGB-Components.
    //----------------------------------------------------------------------
    F32 getLuminance() const;

    //----------------------------------------------------------------------
    // Return the color as an array of 4 normalized floats. [0.0f - 1.0f]
    // @Return:
    //  4 Floats with all components mapped to the range 0.0f - 1.0f
    //----------------------------------------------------------------------
    std::array<F32, 4> normalized() const { return { (F32)getRed() / 255.0f, (F32)getGreen() / 255.0f, (F32)getBlue() / 255.0f, (F32)getAlpha() / 255.0f }; }
   
    // Set all components. Range: [0 - 255]
    void setRGBA(Byte r, Byte g, Byte b, Byte a = 255);

    // Set single components. Range: [0 - 255]
    void setRed(Byte red);
    void setGreen(Byte green);
    void setBlue(Byte blue);
    void setAlpha(Byte alpha);

    Byte getRed() const;
    Byte getGreen() const;
    Byte getBlue() const;
    Byte getAlpha() const;

    bool operator == (const Color& c) const;
    bool operator != (const Color& c) const;

    // Converts to "(r, g, b)" or "(r, g, b, a)"
    String toString(bool includeAlpha = false) const;

    //----------------------------------------------------------------------
    // Linearly interpolate between color a and b.
    //----------------------------------------------------------------------
    static Color Lerp(Color a, Color b, F32 lerp);

private:
    F32 _HexToRGB(const String& hex);

public:
    static Color WHITE;
    static Color BLACK;
    static Color RED;
    static Color GREEN;
    static Color BLUE;
    static Color YELLOW;
    static Color VIOLET;
    static Color TURQUOISE;
    static Color GREY;
    static Color ORANGE;
};
