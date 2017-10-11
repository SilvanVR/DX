#pragma once

/**********************************************************************
    class: Color (color.h)

    author: S. Hau
    date: October 10, 2017

    One color component is represented by exactly one byte. This means 
    the range for each component is 0 - 255. Which format is used 
    depends upon appropriate defines during compilation:
      #define USE_BGRA_FORMAT: Color in BGRA format.
    The default format is RGBA.
**********************************************************************/


class Color
{
    U32 m_components;

public:
    Color();
    //Color(const char* hex);
    explicit Color(U32 color);
    explicit Color(Byte r, Byte g, Byte b, Byte a = 255);

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

private:
    F32 _HexToRGB(const char* hex);

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
