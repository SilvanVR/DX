#include "mesh_generator.h"
/**********************************************************************
    class: MeshGenerator (resource_manager.cpp)

    author: S. Hau
    date: December 25, 2017
**********************************************************************/

#include "Core/locator.h"

#define _USE_MATH_DEFINES
#include <math.h> // M_PI
#include "Math/math_utils.h"

namespace Assets { 

    //----------------------------------------------------------------------
    MeshPtr MeshGenerator::CreateCube( F32 size )
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
            0, 1, 3, 3, 1, 2,
            4, 6, 5, 4, 7, 6,
            4, 5, 1, 4, 1, 0,
            3, 2, 6, 3, 6, 7,
            1, 5, 6, 1, 6, 2,
            4, 0, 3, 4, 3, 7
        };

        return RESOURCES.createMesh( vertices, indices );
    }

    //----------------------------------------------------------------------
    MeshPtr MeshGenerator::CreateCube( F32 size, Color color )
    {
        auto mesh = CreateCube( size );

        ArrayList<Color> colors( mesh->getVertexCount(), color );
        mesh->setColors( colors );

        return mesh;
    }

    //----------------------------------------------------------------------
    MeshPtr MeshGenerator::CreateCube( const Math::Vec3& min, const Math::Vec3& max )
    {
        ArrayList<Math::Vec3> vertices = {
            min,
            Math::Vec3( min.x, max.y, min.z ),
            Math::Vec3( max.x, max.y, min.z ),
            Math::Vec3( max.x, min.y, min.z ),
            Math::Vec3( min.x, min.y, max.z ),
            Math::Vec3( min.x, max.y, max.z ),
            max,
            Math::Vec3( max.x, min.y, max.z )
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
    MeshPtr MeshGenerator::CreateCubeUV( F32 size )
    {
        ArrayList<Math::Vec3> vertices =
        {
            // Front Face
            Math::Vec3( -size, -size, -size ),
            Math::Vec3( -size,  size, -size ),
            Math::Vec3(  size,  size, -size ),
            Math::Vec3(  size, -size, -size ),

            // Back Face
            Math::Vec3( -size, -size, size ),
            Math::Vec3( -size,  size, size ),
            Math::Vec3(  size,  size, size ),
            Math::Vec3(  size, -size, size ),

            // Right Face
            Math::Vec3( size, -size, -size ),
            Math::Vec3( size,  size, -size ),
            Math::Vec3( size,  size, size ),
            Math::Vec3( size, -size, size ),

            // Left Face
            Math::Vec3( -size, -size, -size ),
            Math::Vec3( -size,  size, -size ),
            Math::Vec3( -size,  size,  size ),
            Math::Vec3( -size, -size,  size ),

            // Top Face
            Math::Vec3( -size,  size, -size ),
            Math::Vec3( -size,  size,  size ),
            Math::Vec3(  size,  size,  size ),
            Math::Vec3(  size,  size, -size ),

            // Bottom Face
            Math::Vec3( -size, -size, -size ),
            Math::Vec3( -size, -size,  size ),
            Math::Vec3(  size, -size,  size ),
            Math::Vec3(  size, -size, -size ),
        };
        ArrayList<U32> indices = {
            0, 1, 2, 0, 2, 3,
            4, 6, 5, 4, 7, 6,
            8, 9, 10, 8, 10, 11,
            12, 14, 13, 12, 15, 14,
            16, 17, 18, 16, 18, 19,
            20, 22, 21, 20, 23, 22
        };
        ArrayList<Math::Vec2> uvs =
        {
            Math::Vec2( 0.0f, 1.0f ),
            Math::Vec2( 0.0f, 0.0f ),
            Math::Vec2( 1.0f, 0.0f ),
            Math::Vec2( 1.0f, 1.0f ),

            Math::Vec2( 1.0f, 1.0f ),
            Math::Vec2( 1.0f, 0.0f ),
            Math::Vec2( 0.0f, 0.0f ),
            Math::Vec2( 0.0f, 1.0f ),

            Math::Vec2( 0.0f, 1.0f ),
            Math::Vec2( 0.0f, 0.0f ),
            Math::Vec2( 1.0f, 0.0f ),
            Math::Vec2( 1.0f, 1.0f ),

            Math::Vec2( 1.0f, 1.0f ),
            Math::Vec2( 1.0f, 0.0f ),
            Math::Vec2( 0.0f, 0.0f ),
            Math::Vec2( 0.0f, 1.0f ),

            Math::Vec2( 0.0f, 1.0f ),
            Math::Vec2( 0.0f, 0.0f ),
            Math::Vec2( 1.0f, 0.0f ),
            Math::Vec2( 1.0f, 1.0f ),

            Math::Vec2( 1.0f, 1.0f ),
            Math::Vec2( 1.0f, 0.0f ),
            Math::Vec2( 0.0f, 0.0f ),
            Math::Vec2( 0.0f, 1.0f )
        };

        auto mesh = RESOURCES.createMesh( vertices, indices, uvs );
        mesh->recalculateNormals();
        return mesh;
    }

    MeshPtr MeshGenerator::CreatePlane( F32 xSize, F32 ySize )
    {
        ArrayList<Math::Vec3> vertices = {
            Math::Vec3( -xSize, -ySize, 0.0f ),
            Math::Vec3( -xSize,  ySize, 0.0f ),
            Math::Vec3(  xSize,  ySize, 0.0f ),
            Math::Vec3(  xSize, -ySize, 0.0f )
        };
        ArrayList<U32> indices = {
            0, 1, 2, 0, 2, 3
        };
        ArrayList<Math::Vec2> uvs = {
            Math::Vec2( 0.0f, 1.0f ),
            Math::Vec2( 0.0f, 0.0f ),
            Math::Vec2( 1.0f, 0.0f ),
            Math::Vec2( 1.0f, 1.0f )
        };
        ArrayList<Math::Vec3> normals = { 
            Math::Vec3( 0.0f, 0.0f, -1.0f ),
            Math::Vec3( 0.0f, 0.0f, -1.0f ),
            Math::Vec3( 0.0f, 0.0f, -1.0f ),
            Math::Vec3( 0.0f, 0.0f, -1.0f )
        };

        auto mesh = RESOURCES.createMesh( vertices, indices, uvs );
        mesh->setNormals( normals );

        return mesh;
    }

    //----------------------------------------------------------------------
    MeshPtr MeshGenerator::CreatePlane( F32 size, Color color )
    {
        auto mesh = CreatePlane( size );

        ArrayList<Color> colors( mesh->getVertexCount(), color );
        mesh->setColors( colors );

        return mesh;
    }

    //----------------------------------------------------------------------
    MeshPtr MeshGenerator::CreatePlane( U32 width, U32 height )
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
    MeshPtr MeshGenerator::CreatePlane( U32 width, U32 height, Color color )
    {
        auto mesh = CreatePlane( width, height );

        ArrayList<Color> colors( mesh->getVertexCount(), color );
        mesh->setColors( colors );

        return mesh;
    }

    //----------------------------------------------------------------------
    MeshPtr MeshGenerator::CreateUVSphere( const Math::Vec3& center, F32 size, U32 meridians, U32 parallels )
    {
        ArrayList<Math::Vec3>   vertices;
        ArrayList<U32>          indices;

        // Vertices
        vertices.emplace_back( center.x, center.y + size, center.z );
        for (U32 j = 0; j < parallels - 1; ++j)
        {
            const F64 polar = M_PI * F64(j + 1) / F64( parallels );
            const F64 sp = std::sin( polar );
            const F64 cp = std::cos( polar );
            for (U32 i = 0; i < meridians; ++i)
            {
                const F64 azimuth = 2.0 * M_PI * F64( i ) / F64( meridians );
                const F64 sa = std::sin( azimuth );
                const F64 ca = std::cos( azimuth );
                const F64 x = sp * ca;
                const F64 y = cp;
                const F64 z = sp * sa;
                Math::Vec3 vertex( F32(x * size), F32(y * size), F32(z * size) );
                vertices.push_back( center + vertex );
            }
        }
        vertices.emplace_back( center.x, center.y + -size, center.z );

        // Indices
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
            indices.emplace_back( a );
            indices.emplace_back( b );
        }

        auto mesh = RESOURCES.createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices );

        return mesh;
    }

    //----------------------------------------------------------------------
    // Credits @https://github.com/caosdoar/spheres
    MeshPtr MeshGenerator::CreateUVSphere( U32 meridians, U32 parallels )
    {
        return CreateUVSphere( Math::Vec3(0), 1.0f, meridians, parallels );
    }

    //----------------------------------------------------------------------
    MeshPtr MeshGenerator::CreateUVSphere( U32 meridians, U32 parallels, Color color )
    {
        auto mesh = CreateUVSphere( meridians, parallels );

        ArrayList<Color> colors( mesh->getVertexCount(), color );
        mesh->setColors( colors );

        return mesh;
    }

    //----------------------------------------------------------------------
    MeshPtr MeshGenerator::CreateGrid( U32 size )
    {
        ArrayList<Math::Vec3>   vertices;
        ArrayList<U32>          indices;
        ArrayList<Color>        colors;

        // Generate grid on XZ plane
        F32 half = size / 2.0f;
        for (U32 i = 0; i <= size; i++)
        {
            // Horizontal Line
            vertices.emplace_back( i - half, 0.0f,  half );
            vertices.emplace_back( i - half, 0.0f, -half );

            indices.emplace_back( i * 4 );
            indices.emplace_back( i * 4 + 1 );

            // Vertical Line
            vertices.emplace_back( half,  0.0f, i - half );
            vertices.emplace_back(-half,  0.0f, i - half );

            indices.emplace_back( i * 4 + 2 );
            indices.emplace_back( i * 4 + 3 );
  
            if ( (i - half) == 0 ) // Center lines
            {
                colors.insert( colors.end(), 2, Color::BLUE );
                colors.insert( colors.end(), 2, Color::RED );
            }
            else
            {
                colors.insert( colors.end(), 4, Color::GREY );
            }
        }

        // Generate Y-Line
        vertices.emplace_back( 0.0f, -half, 0.0f );
        vertices.emplace_back( 0.0f,  half, 0.0f );
        indices.emplace_back( static_cast<U32>( vertices.size() ) - 2);
        indices.emplace_back( static_cast<U32>( vertices.size() ) - 1);
        colors.push_back( Color::GREEN );
        colors.push_back( Color::GREEN );

        auto mesh = RESOURCES.createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices, 0, Graphics::MeshTopology::Lines );
        mesh->setColors( colors );

        return mesh;
    }

    MeshPtr MeshGenerator::CreateFrustum( const Math::Vec3& pos, const Math::Vec3& up, const Math::Vec3& right, const Math::Vec3& forward,
                                          F32 fovAngleYDeg, F32 zNear, F32 zFar, F32 aspectRatio, Color color )
    {
        auto frustumCorners = Math::CalculateFrustumCorners( pos, up, right, forward, fovAngleYDeg, zNear, zFar, aspectRatio );
        ArrayList<Math::Vec3> vertices( frustumCorners.begin(), frustumCorners.end() );

        enum corner {
            NEAR_TOP_LEFT = 0, NEAR_TOP_RIGHT = 1, NEAR_BOTTOM_LEFT = 2, NEAR_BOTTOM_RIGHT = 3,
            FAR_TOP_LEFT = 4, FAR_TOP_RIGHT = 5, FAR_BOTTOM_LEFT = 6, FAR_BOTTOM_RIGHT = 7
        };

        ArrayList<U32> indices = {
            NEAR_TOP_LEFT, NEAR_TOP_RIGHT,
            NEAR_BOTTOM_LEFT, NEAR_BOTTOM_RIGHT,
            NEAR_TOP_LEFT, NEAR_BOTTOM_LEFT,
            NEAR_TOP_RIGHT, NEAR_BOTTOM_RIGHT,
            FAR_TOP_LEFT, FAR_TOP_RIGHT,
            FAR_BOTTOM_LEFT, FAR_BOTTOM_RIGHT,
            FAR_TOP_LEFT, FAR_BOTTOM_LEFT,
            FAR_TOP_RIGHT, FAR_BOTTOM_RIGHT,
            NEAR_TOP_LEFT, FAR_TOP_LEFT,
            NEAR_TOP_RIGHT, FAR_TOP_RIGHT,
            NEAR_BOTTOM_LEFT, FAR_BOTTOM_LEFT,
            NEAR_BOTTOM_RIGHT, FAR_BOTTOM_RIGHT
        };

        auto mesh = RESOURCES.createMesh();
        mesh->setVertices( vertices );
        mesh->setIndices( indices, 0, Graphics::MeshTopology::Lines );

        ArrayList<Color> colors( indices.size(), color );
        mesh->setColors( colors );

        return mesh;
    }

} // End namespaces