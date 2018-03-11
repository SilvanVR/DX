#pragma once
/**********************************************************************
    class: MeshGenerator (mesh_generator.h)

    author: S. Hau
    date: March 11, 2018
**********************************************************************/

#include "Graphics/i_mesh.hpp"

namespace Assets { 

    //*********************************************************************
    class MeshGenerator
    {
    public:
        //----------------------------------------------------------------------
        // Generates a new cube mesh.
        // @Params:
        //       "size": The size/radi of the cube.
        // [opt] "color": The vertex-color for each vertex.
        //----------------------------------------------------------------------
        static Graphics::Mesh* CreateCube(F32 size = 1.0f);
        static Graphics::Mesh* CreateCube(F32 size, Color color);

        //----------------------------------------------------------------------
        // Generates a new plane mesh in the X/Y Plane.
        // @Params:
        //       "size": The size of the plane.
        // [opt] "color": The vertex-color for each vertex.
        //----------------------------------------------------------------------
        static Graphics::Mesh* CreatePlane(F32 size = 1.0f);
        static Graphics::Mesh* CreatePlane(F32 size, Color color);

        //----------------------------------------------------------------------
        // Generates a new sphere mesh.
        // @Params:
        //       "size": The size of the plane.
        // [opt] "color": The vertex-color for each vertex.
        //----------------------------------------------------------------------
        static Graphics::Mesh* CreateUVSphere(U32 meridians, U32 parallels);
        static Graphics::Mesh* CreateUVSphere(U32 meridians, U32 parallels, Color color);

    private:
        //----------------------------------------------------------------------
        MeshGenerator()                                         = delete;
        MeshGenerator(const MeshGenerator& other)               = delete;
        MeshGenerator& operator = (const MeshGenerator& other)  = delete;
        MeshGenerator(MeshGenerator&& other)                    = delete;
        MeshGenerator& operator = (MeshGenerator&& other)       = delete;
    };

}