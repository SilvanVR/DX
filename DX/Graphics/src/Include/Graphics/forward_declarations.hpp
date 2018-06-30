#pragma once
/**********************************************************************
    class: None

    author: S. Hau
    date: June 30, 2018

    Forward declarations for the most important classes in the graphics ns.
**********************************************************************/

namespace Graphics
{
    class Light;
    class Camera;
    class IMesh;
    class ITexture;
    class ITexture2D;
    class ITexture2DArray;
    class IRenderBuffer;
    class IRenderTexture;
    class ICubemap;
    class IMaterial;
    class IShader;
}

using MeshPtr = std::shared_ptr<Graphics::IMesh>;
using MaterialPtr = std::shared_ptr<Graphics::IMaterial>;
using CubemapPtr = std::shared_ptr<Graphics::ICubemap>;
using TexturePtr = std::shared_ptr<Graphics::ITexture>;
using RenderTexturePtr = std::shared_ptr<Graphics::IRenderTexture>;
using ShaderPtr = std::shared_ptr<Graphics::IShader>;
using Texture2DPtr = std::shared_ptr<Graphics::ITexture2D>;
using Texture2DArrayPtr = std::shared_ptr<Graphics::ITexture2DArray>;