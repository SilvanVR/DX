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
        COPY_TEXTURE,
        SET_GLOBAL_FLOAT, SET_GLOBAL_INT, SET_GLOBAL_VECTOR, SET_GLOBAL_MATRIX,
        DRAW_LIGHT
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
        GPUC_DrawMesh( MeshPtr mesh, MaterialPtr material, const DirectX::XMMATRIX& modelMatrix, I32 subMeshIndex )
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
    struct GPUC_CopyTexture : public GPUCommandBase
    {
        GPUC_CopyTexture(ITexture* srcTex, I32 srcElement, I32 srcMip, ITexture* dstTex, I32 dstElement, I32 dstMip)
            : GPUCommandBase( GPUCommand::COPY_TEXTURE ), 
            srcTex( srcTex ), srcElement(srcElement ), srcMip( srcMip ), dstTex( dstTex ), dstElement( dstElement ), dstMip( dstMip ){}

        ITexture*   srcTex;
        ITexture*   dstTex;
        I32         srcElement, dstElement, srcMip, dstMip;
    };


    //**********************************************************************
    struct GPUC_SetGlobalFloat : public GPUCommandBase
    {
        GPUC_SetGlobalFloat(StringID name, F32 value)
            : GPUCommandBase( GPUCommand::SET_GLOBAL_FLOAT ),
            name( name ), value( value ) {}

        StringID    name;
        F32         value;
    };

    //**********************************************************************
    struct GPUC_SetGlobalInt : public GPUCommandBase
    {
        GPUC_SetGlobalInt(StringID name, I32 value)
            : GPUCommandBase( GPUCommand::SET_GLOBAL_INT ),
            name( name ), value( value ) {}

        StringID    name;
        I32         value;
    };

    //**********************************************************************
    struct GPUC_SetGlobalVector : public GPUCommandBase
    {
        GPUC_SetGlobalVector(StringID name, const Math::Vec4& vec)
            : GPUCommandBase( GPUCommand::SET_GLOBAL_VECTOR ),
            name( name ), vec( vec ) {}

        StringID    name;
        Math::Vec4  vec;
    };

    //**********************************************************************
    struct GPUC_SetGlobalMatrix : public GPUCommandBase
    {
        GPUC_SetGlobalMatrix(StringID name, const DirectX::XMMATRIX& mat)
            : GPUCommandBase( GPUCommand::SET_GLOBAL_MATRIX ),
            name( name ), matrix( mat ) {}

        StringID name;
        DirectX::XMMATRIX matrix;
    };

    //**********************************************************************
    struct GPUC_DrawLight : public GPUCommandBase
    {
        GPUC_DrawLight(const Light* light)
            : GPUCommandBase( GPUCommand::DRAW_LIGHT ),
            light( light ) {}

        const Light* light;
    };

} // End namespaces