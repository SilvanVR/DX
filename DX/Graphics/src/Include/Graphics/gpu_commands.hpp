#pragma once
/**********************************************************************
    class: Several (gpu_commands.hpp)

    author: S. Hau
    date: December 19, 2017

    - Contains definitions for all supported gpu commands, which can
      be put into a command buffer.
**********************************************************************/

#include "structs.hpp"
#include "i_mesh.h"
#include "i_material.h"
#include "i_cubemap.hpp"
#include "i_render_texture.hpp"
#include "Lighting/light.h"
#include "camera.h"

namespace Graphics {

    //----------------------------------------------------------------------
    enum class GPUCommand
    {
        UNKNOWN,
        DRAW_MESH,
        SET_CAMERA,
        END_CAMERA,
        COPY_TEXTURE,
        DRAW_LIGHT,
        SET_RENDER_TARGET,
        DRAW_FULLSCREEN_QUAD,
        RENDER_CUBEMAP,
        BLIT
    };

    //**********************************************************************
    struct GPUCommandBase
    {
    public:
        GPUCommandBase( GPUCommand type ) : m_type( type ) {}
        virtual ~GPUCommandBase() = default;

        //----------------------------------------------------------------------
        GPUCommand getType() const { return m_type; }

    private:
        GPUCommand m_type = GPUCommand::UNKNOWN;
    };

    //**********************************************************************
    struct GPUC_DrawMesh : public GPUCommandBase
    {
        GPUC_DrawMesh( const MeshPtr& mesh, const MaterialPtr& material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
            : GPUCommandBase( GPUCommand::DRAW_MESH ),
            material( material ), mesh( mesh ), modelMatrix( modelMatrix ), subMeshIndex( subMeshIndex ) {}

        DirectX::XMMATRIX   modelMatrix;
        MeshPtr             mesh;
        MaterialPtr         material;
        I32                 subMeshIndex;
    };

    //**********************************************************************
    struct GPUC_SetCamera : public GPUCommandBase
    {
        GPUC_SetCamera( Camera* camera )
            : GPUCommandBase( GPUCommand::SET_CAMERA ), 
            camera( camera ) {}

        Camera* camera;
    };

    //**********************************************************************
    struct GPUC_EndCamera : public GPUCommandBase
    {
        GPUC_EndCamera( Camera* camera )
            : GPUCommandBase( GPUCommand::END_CAMERA ),
            camera( camera ) {}

        Camera* camera;
    };

    //**********************************************************************
    struct GPUC_CopyTexture : public GPUCommandBase
    {
        GPUC_CopyTexture(const TexturePtr& srcTex, I32 srcElement, I32 srcMip, const TexturePtr& dstTex, I32 dstElement, I32 dstMip)
            : GPUCommandBase( GPUCommand::COPY_TEXTURE ), 
            srcTex( srcTex ), srcElement(srcElement ), srcMip( srcMip ), dstTex( dstTex ), dstElement( dstElement ), dstMip( dstMip ){}

        TexturePtr  srcTex;
        TexturePtr  dstTex;
        I32         srcElement, dstElement, srcMip, dstMip;
    };

    //**********************************************************************
    struct GPUC_DrawLight : public GPUCommandBase
    {
        GPUC_DrawLight(const Light* light)
            : GPUCommandBase( GPUCommand::DRAW_LIGHT ),
            light( light ) {}

        const Light* light;
    };

    //**********************************************************************
    struct GPUC_SetRenderTarget : public GPUCommandBase
    {
        GPUC_SetRenderTarget(const RenderTexturePtr& target )
            : GPUCommandBase( GPUCommand::SET_RENDER_TARGET ),
            target( target ) {}

        const RenderTexturePtr  target;
    };

    //**********************************************************************
    struct GPUC_DrawFullscreenQuad : public GPUCommandBase
    {
        GPUC_DrawFullscreenQuad( const MaterialPtr& material )
            : GPUCommandBase( GPUCommand::DRAW_FULLSCREEN_QUAD ),
            material( material ) {}

        const MaterialPtr material;
    };

    //**********************************************************************
    struct GPUC_RenderCubemap : public GPUCommandBase
    {
        GPUC_RenderCubemap( const CubemapPtr& cubemap, const MaterialPtr& material, I32 dstMip )
            : GPUCommandBase( GPUCommand::RENDER_CUBEMAP ),
            cubemap( cubemap ), material( material ), dstMip( dstMip ) {}

        const CubemapPtr  cubemap;
        const MaterialPtr material;
        I32               dstMip;
    };

    //**********************************************************************
    struct GPUC_Blit : public GPUCommandBase
    {
        GPUC_Blit( const RenderTexturePtr& src, const RenderTexturePtr& dst, const MaterialPtr& material )
            : GPUCommandBase( GPUCommand::BLIT ),
            src( src ), dst( dst ), material( material ) {}

        const RenderTexturePtr  src;
        const RenderTexturePtr  dst;
        const MaterialPtr       material;
    };

} // End namespaces