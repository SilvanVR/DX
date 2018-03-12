#include "mesh_generator.h"
/**********************************************************************
    class: MeshGenerator (resource_manager.cpp)

    author: S. Hau
    date: December 25, 2017
**********************************************************************/

#include "locator.h"

#define _USE_MATH_DEFINES
#include <math.h> // M_PI

namespace Assets { 

    //----------------------------------------------------------------------
    Graphics::Mesh* MeshGenerator::CreateCube( F32 size )
    {
        ArrayList<Math::Vec3> vertices =
        {
            Math::Vec3(-size, -size, -size),
            Math::Vec3(-size,  size, -size),
            Math::Vec3(size,  size, -size),
            Math::Vec3(size, -size, -size),
            Math::Vec3(-size, -size,  size),
            Math::Vec3(-size,  size,  size),
            Math::Vec3(size,  size,  size),
            Math::Vec3(size, -size,  size)
        };
        ArrayList<U32> indices = {
            0, 1, 2, 0, 2, 3,
            4, 6, 5, 4, 7, 6,
            4, 5, 1, 4, 1, 0,
            3, 2, 6, 3, 6, 7,
            1, 5, 6, 1, 6, 2,
            4, 0, 3, 4, 3, 7
        };

        return RESOURCES.createMesh( vertices, indices );
    }

    //----------------------------------------------------------------------
    Graphics::Mesh* MeshGenerator::CreateCube( F32 size, Color color )
    {
        auto mesh = CreateCube( size );

        ArrayList<Color> colors( mesh->getVertexCount(), color );
        mesh->setColors( colors );

        return mesh;
    }

    //----------------------------------------------------------------------
    Graphics::Mesh* MeshGenerator::CreatePlane( F32 size )
    {
        ArrayList<Math::Vec3> vertices =
        {
            Math::Vec3(-size, -size, 0.0f),
            Math::Vec3(-size,  size, 0.0f),
            Math::Vec3( size,  size, 0.0f),
            Math::Vec3( size, -size, 0.0f)
        };
        ArrayList<U32> indices = {
            0, 1, 2, 0, 2, 3
        };

        return RESOURCES.createMesh( vertices, indices );
    }

    //----------------------------------------------------------------------
    Graphics::Mesh* MeshGenerator::CreatePlane( F32 size, Color color )
    {
        auto mesh = CreatePlane( size );

        ArrayList<Color> colors( mesh->getVertexCount(), color );
        mesh->setColors( colors );

        return mesh;
    }

    //----------------------------------------------------------------------
    Graphics::Mesh* MeshGenerator::CreatePlane( U32 width, U32 height )
    {
        ArrayList<Math::Vec3>   vertices;
        ArrayList<U32>          indices;

        vertices.reserve( width * height );
        indices.reserve( width * height * 6 - (width + height - 1) );

        for (U32 x = 0; x < width; x++)
        {
            for (U32 y = 0; y < height; y++)
            {
                vertices.emplace_back( (F32)x, (F32)y, 0.0f );

                if (x == 0 || y == 0)
                    continue;

                //Adds the index of the three vertices in order to make up each of the two tris
                indices.emplace_back( width * x + y );             //Top right
                indices.emplace_back( width * x + y - 1 );         //Bottom right
                indices.emplace_back( width * (x - 1) + y - 1 );   //Bottom left - First triangle
                indices.emplace_back( width * (x - 1) + y - 1 );   //Bottom left 
                indices.emplace_back( width * (x - 1) + y );       //Top left
                indices.emplace_back( width * x + y );             //Top right - Second triangle
            }
        }

        return RESOURCES.createMesh( vertices, indices );
    }

    //----------------------------------------------------------------------
    Graphics::Mesh* MeshGenerator::CreatePlane( U32 width, U32 height, Color color )
    {
        auto mesh = CreatePlane( width, height );

        ArrayList<Color> colors( mesh->getVertexCount(), color );
        mesh->setColors( colors );

        return mesh;
    }

    //----------------------------------------------------------------------
    // Credits @https://github.com/caosdoar/spheres
    Graphics::Mesh* MeshGenerator::CreateUVSphere( U32 meridians, U32 parallels )
    {
        ArrayList<Math::Vec3>   vertices;
        ArrayList<U32>          indices;

        vertices.emplace_back( 0.0f, 1.0f, 0.0f );
        for (U32 j = 0; j < parallels - 1; ++j)
        {
            F64 const polar = M_PI * F64(j + 1) / F64(parallels);
            F64 const sp = std::sin(polar);
            F64 const cp = std::cos(polar);
            for (U32 i = 0; i < meridians; ++i)
            {
                F64 const azimuth = 2.0 * M_PI * F64(i) / F64(meridians);
                F64 const sa = std::sin(azimuth);
                F64 const ca = std::cos(azimuth);
                F64 const x = sp * ca;
                F64 const y = cp;
                F64 const z = sp * sa;
                vertices.emplace_back( (F32)x, (F32)y, (F32)z);
            }
        }
        vertices.emplace_back( 0.0f, -1.0f, 0.0f );

        for (U32 i = 0; i < meridians; ++i)
        {
            U32 const a = i + 1;
            U32 const b = (i + 1) % meridians + 1;
            indices.emplace_back(0);
            indices.emplace_back(b);
            indices.emplace_back(a);
        }

        for (U32 j = 0; j < parallels - 2; ++j)
        {
            U32 aStart = j * meridians + 1;
            U32 bStart = (j + 1) * meridians + 1;
            for (U32 i = 0; i < meridians; ++i)
            {
                const U32 a = aStart + i;
                const U32 a1 = aStart + (i + 1) % meridians;
                const U32 b = bStart + i;
                const U32 b1 = bStart + (i + 1) % meridians;
                indices.emplace_back(a);
                indices.emplace_back(a1);
                indices.emplace_back(b1);
                indices.emplace_back(a);
                indices.emplace_back(b1);
                indices.emplace_back(b);
            }
        }

        for (U32 i = 0; i < meridians; ++i)
        {
            const U32 a = i + meridians * (parallels - 2) + 1;
            const U32 b = (i + 1) % meridians + meridians * (parallels - 2) + 1;
            indices.emplace_back( static_cast<U32>( vertices.size() ) - 1);
            indices.emplace_back(a);
            indices.emplace_back(b);
        }

        auto mesh = RESOURCES.createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices );

        return mesh;
    }

    //----------------------------------------------------------------------
    Graphics::Mesh* MeshGenerator::CreateUVSphere( U32 meridians, U32 parallels, Color color )
    {
        auto mesh = CreateUVSphere( meridians, parallels );

        ArrayList<Color> colors( mesh->getVertexCount(), color );
        mesh->setColors( colors );

        return mesh;
    }

}  // end namespaces