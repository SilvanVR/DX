#pragma once
/**********************************************************************
    class: None (D3D11Defines.h)

    author: S. Hau
    date: March 25, 2018
**********************************************************************/

static constexpr StringID SID_VERTEX_POSITION   = StringID("POSITION");
static constexpr StringID SID_VERTEX_COLOR      = StringID("COLOR");
static constexpr StringID SID_VERTEX_UV         = StringID("TEXCOORD");
static constexpr StringID SID_VERTEX_NORMAL     = StringID("NORMAL");
static constexpr StringID SID_VERTEX_TANGENT    = StringID("TANGENT");

#define SEMANTIC_SYSTEM         "SV_"

// If a semantic ends with this (sem-index excluded) it will be treated as an instance attribute
#define SEMANTIC_INSTANCED      "_INSTANCED"

#define MATERIAL_NAME           "material"
#define SHADER_NAME             "shader"