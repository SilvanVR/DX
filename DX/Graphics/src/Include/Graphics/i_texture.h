#pragma once
/**********************************************************************
    class: Texture (texture.h)

    author: S. Hau
    date: March 24, 2018

**********************************************************************/

namespace Graphics
{

    //**********************************************************************
    class ITexture
    {
    public:
        ITexture() = default;
        virtual ~ITexture(){}

        //----------------------------------------------------------------------
        void generateMips() {}

        //----------------------------------------------------------------------
        F32 getAspectRatio()    const { return (F32)getWidth() / getHeight(); }
        U32 getWidth()          const { return m_width; }
        U32 getHeight()         const { return m_height; }


        //----------------------------------------------------------------------
        void setSize(U32 width, U32 height) { m_width = width; m_height = height; }

        virtual void init() = 0;
        virtual void apply() = 0;

    private:
        U32 m_width  = 0;
        U32 m_height = 0;

        //----------------------------------------------------------------------
        ITexture(const ITexture& other)               = delete;
        ITexture& operator = (const ITexture& other)  = delete;
        ITexture(ITexture&& other)                    = delete;
        ITexture& operator = (ITexture&& other)       = delete;
    };

    using Texture = ITexture;
}