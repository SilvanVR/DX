#pragma once
/**********************************************************************
    class: WorldGeneration (world_generator.h)

    author: S. Hau
    date: April 16, 2018

    Script which generates chunks of block data using perlin noise and
    the polyvox library to generate polygon meshes for rendering.
**********************************************************************/
#include <DX.h>
#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/LargeVolume.h"
#include "noise_map.h"

namespace std {

    template <>
    struct hash<Math::Vec2>
    {
        std::size_t operator()(const Math::Vec2& v) const
        {
            using std::size_t;
            using std::hash;
            using std::string;

            return ((hash<float>()(v.x) ^ (hash<float>()(v.y) << 1)) >> 1);
        }
    };

}

enum class BlockType
{
    Air = 0,
    Sand,
    Gravel,
    Dirt,
    Stone,
    Snow
};

template <typename Type>
class TBlock
{
public:
    TBlock() : m_uMaterial(0) {}
    TBlock(Type uMaterial) : m_uMaterial(uMaterial) {}
    TBlock(BlockType blockType) : m_uMaterial(static_cast<Type>(blockType)) { }

    bool operator==(const TBlock& rhs) const
    {
        return (m_uMaterial == rhs.m_uMaterial);
    };

    bool operator!=(const TBlock& rhs) const
    {
        return !(*this == rhs);
    }

    Type getMaterial() const { return m_uMaterial; }
    void setMaterial(Type uMaterial) { m_uMaterial = uMaterial; }

private:
    Type m_uMaterial;
};

typedef TBlock<uint8_t> Block;

template<typename Type>
class PolyVox::DefaultIsQuadNeeded< TBlock<Type> >
{
public:
    bool operator()(TBlock<Type> back, TBlock<Type> front, uint32_t& materialToUse)
    {
        if ((back.getMaterial() > 0) && (front.getMaterial() == 0))
        {
            materialToUse = static_cast<uint32_t>(back.getMaterial());
            return true;
        }
        else
        {
            return false;
        }
    }
};


//void createSphereInVolume(PolyVox::LargeVolume<PolyVox::Material8>& volData, float fRadius)
//{
//    //This vector hold the position of the center of the volume
//    PolyVox::Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);
//    //Vector3DFloat v3dVolCenter(0,0,0);
//
//    //This three-level for loop iterates over every voxel in the volume
//    for (int z = 0; z < volData.getDepth(); z++)
//    {
//        for (int y = 0; y < volData.getHeight(); y++)
//        {
//            for (int x = 0; x < volData.getWidth(); x++)
//            {
//                //Store our current position as a vector...
//                PolyVox::Vector3DFloat v3dCurrentPos(x, y, z);
//                //And compute how far the current position is from the center of the volume
//                float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();
//
//                uint8_t uVoxelValue = 0;
//
//                //If the current voxel is less than 'radius' units from the center then we make it solid.
//                if (fDistToCenter <= fRadius)
//                {
//                    //Our new voxel value
//                    uVoxelValue = Math::Random::Int(1, NUM_MATERIALS);
//                }
//
//                //Wrte the voxel value into the volume	
//                volData.setVoxelAt(x, y, z, uVoxelValue);
//            }
//        }
//    }
//}

struct TerrainType
{
    String  name;
    F32     height;
    Color   color;
    Block   block;
};

//**********************************************************************
class WorldGeneration : public Components::IComponent
{
    static const I32 s_chunkSize    = 32;
    static const I32 s_chunkHeight  = 128;
    static const I32 s_maxViewDst   = 100;

    PolyVox::LargeVolume<Block> m_volData;
    ArrayList<TerrainType>      m_regions;

    MaterialPtr                 m_chunkMaterial;
    MaterialPtr                 m_noiseMapMaterial;

    bool    m_generating        = false;
    F32     m_speed             = 10.0f;

    F32     m_noiseScale        = 50.0f;
    F32     m_noiseLacunarity   = 2.0f;
    F32     m_noiseGain         = 0.3f;
    I32     m_noiseOctaves      = 4;
    F32     m_terrainHeight     = 30.0f;

    F32     m_chunksVisibleInViewDst = s_maxViewDst / s_chunkSize;

    enum class DrawMode
    {
        NoiseMap,
        ColorMap
    } m_drawMode = DrawMode::ColorMap;

public:
    WorldGeneration() 
        : m_volData(PolyVox::Region(PolyVox::Vector3DInt32(INT_MIN, -s_chunkHeight, INT_MIN),
                                    PolyVox::Vector3DInt32(INT_MAX, s_chunkHeight, INT_MAX))) {}

    //----------------------------------------------------------------------
    void init() override
    {
        //m_regions.push_back(TerrainType{ "Water", 0.3f, Color(0x4286f4), BlockType::Water });
        //m_regions.push_back(TerrainType{ "WaterShallow", 0.4f, Color(0x82cdff), BlockType::Water });
        m_regions.push_back(TerrainType{ "Sand", 0.2f, Color(0xf9f39a), BlockType::Sand });
        m_regions.push_back(TerrainType{ "Gravel", 0.3f, Color(0x99ff6b), BlockType::Gravel });
        m_regions.push_back(TerrainType{ "Dirt", 0.7f, Color(0x5eaa3b), BlockType::Dirt });
        m_regions.push_back(TerrainType{ "Stone", 0.9f, Color(0x4f4a32), BlockType::Stone });
        m_regions.push_back(TerrainType{ "Snow", 1.0f, Color(0xffffff), BlockType::Snow });

        _SetupShaderAndMaterial();

        // VISUALIZATION OF PERLIN NOISE ON A FLAT PLANE
        auto texShader = RESOURCES.createShader("TexShader", "/shaders/texVS.hlsl", "/shaders/texPS.hlsl");
        m_noiseMapMaterial = RESOURCES.createMaterial(texShader);

        SCENE.createGameObject()->addComponent<Components::MeshRenderer>(Core::Assets::MeshGenerator::CreatePlane(1, Color::GREEN), m_noiseMapMaterial);
    }

    F32 lastScale = 0.0f;
    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        //static F64 yaw = 0;
        //yaw += 45.0 * delta.value;
        //auto fw = Math::Quat::FromEulerAngles({ 0, (F32)yaw, 0}).getForward();
        //m_chunkMaterial->setVec4("dir", Math::Vec4(fw.x, -fw.y, fw.z, 0));

        if (KEYBOARD.isKeyDown(Key::Up))
            m_noiseScale += delta.value * m_speed * 10.0f;
        if (KEYBOARD.isKeyDown(Key::Down))
            m_noiseScale -= delta.value * m_speed * 10.0f;

        static F32 lastLacunarity = 0.0f;
        if (KEYBOARD.isKeyDown(Key::T))
            m_noiseLacunarity += delta.value * m_speed;
        if (KEYBOARD.isKeyDown(Key::G))
            m_noiseLacunarity -= delta.value * m_speed;

        static F32 lastGain = 0.0f;
        if (KEYBOARD.isKeyDown(Key::Z))
            m_noiseGain += delta.value * m_speed * 0.1f;
        if (KEYBOARD.isKeyDown(Key::H))
            m_noiseGain -= delta.value * m_speed * 0.1f;

        static I32 lastOctaves = 0;
        if (KEYBOARD.wasKeyPressed(Key::U))
            m_noiseOctaves++;
        if (KEYBOARD.wasKeyPressed(Key::J))
            m_noiseOctaves--;

        static DrawMode lastDrawMode;
        if (KEYBOARD.wasKeyPressed(Key::F5))
            m_drawMode = DrawMode::NoiseMap;
        if (KEYBOARD.wasKeyPressed(Key::F6))
            m_drawMode = DrawMode::ColorMap;

        static F32 lastTerrainHeight = 0.0f;
        if (KEYBOARD.isKeyDown(Key::Left))
            m_terrainHeight -= delta.value * m_speed;
        if (KEYBOARD.isKeyDown(Key::Right))
            m_terrainHeight += delta.value * m_speed;

        if ( (lastScale != m_noiseScale || lastLacunarity != m_noiseLacunarity || lastGain != m_noiseGain 
            || lastOctaves != m_noiseOctaves || lastDrawMode != m_drawMode || lastTerrainHeight != m_terrainHeight))
        {
            lastScale = m_noiseScale; lastLacunarity = m_noiseLacunarity; lastGain = m_noiseGain; 
            lastOctaves = m_noiseOctaves; lastDrawMode = m_drawMode; lastTerrainHeight = m_terrainHeight;

            ASYNC_JOB([&] {
                NoiseMap noiseMap(s_chunkSize, s_chunkSize, m_noiseScale, m_noiseLacunarity, m_noiseGain, m_noiseOctaves );
                switch (m_drawMode)
                {
                case DrawMode::NoiseMap: m_noiseMapMaterial->setTexture("tex", _GenerateNoiseTextureFromNoiseMap(noiseMap)); break;
                case DrawMode::ColorMap: m_noiseMapMaterial->setTexture("tex", _GenerateColorTextureFromNoiseMap(noiseMap)); break;
                }
            });

            LOG("Scale: " + TS(m_noiseScale));
            LOG("Lacunarity: " + TS(m_noiseLacunarity));
            LOG("Gain: " + TS(m_noiseGain));
            LOG("Octaves: " + TS(m_noiseOctaves));
            LOG("Terrain Height: " + TS(m_terrainHeight));
        }

        for (auto& pair : m_terrainChunkDictionary)
            pair.second->go->setActive( false );

        // Calculate which chunks are visible
        Components::Camera* viewer = SCENE.getMainCamera();
        auto transform = viewer->getComponent<Components::Transform>();

        I32 currentChunkCoordX = static_cast<I32>(transform->position.x / s_chunkSize);
        I32 currentChunkCoordY = static_cast<I32>(transform->position.z / s_chunkSize);

        for (I32 yOffset = -m_chunksVisibleInViewDst; yOffset <= m_chunksVisibleInViewDst; yOffset++)
        {
            for (I32 xOffset = -m_chunksVisibleInViewDst; xOffset <= m_chunksVisibleInViewDst; xOffset++)
            {
                Math::Vec2 viewedChunkCoord( currentChunkCoordX + xOffset, currentChunkCoordY + yOffset );

                if (m_terrainChunkDictionary.find(viewedChunkCoord) != m_terrainChunkDictionary.end())
                {
                    m_terrainChunkDictionary[viewedChunkCoord]->go->setActive(true);
                }
                else
                {
                    auto newChunk = std::make_shared<TerrainChunk>( viewedChunkCoord, s_chunkSize );
                    m_terrainChunkDictionary[viewedChunkCoord] = newChunk;
                    m_chunkGenerationQueue.push(newChunk);
                }
            }
        }

        // Generate new chunk if requested and not currently generating one
        if ( not m_chunkGenerationQueue.empty() && not m_generating )
        {
            m_generating = true;
            auto nextChunk = m_chunkGenerationQueue.front();

            ASYNC_JOB([=] {
                NoiseMap noiseMap( s_chunkSize, s_chunkSize, m_noiseScale, m_noiseLacunarity, m_noiseGain, m_noiseOctaves, nextChunk->position);
                auto mesh = _GenerateMeshFromNoiseMap(nextChunk->bounds, noiseMap, m_terrainHeight);

                nextChunk->go->addComponent<Components::MeshRenderer>(mesh, m_chunkMaterial);
                //nextChunk->go->getComponent<Components::Transform>()->position = { -s_chunkSize / 2.0f, 0.0f, -s_chunkSize / 2.0f };
                m_generating = false;
            });

            m_chunkGenerationQueue.pop();
        }
    }

    struct Bounds
    {
        Math::Vec3 min;
        Math::Vec3 max;
    };

    struct TerrainChunk
    {
        GameObject* go;
        Math::Vec2  position;
        Bounds      bounds;
        
        TerrainChunk(const Math::Vec2& pos, I32 size) 
            : position(pos * size)
        {
            LOG(position.toString());
            Math::Vec3 posV3(position.x, -s_chunkHeight - 15.0f, position.y);
            go = SCENE.createGameObject("CHUNK");
            auto transform = go->getTransform();
            transform->position = posV3;

            bounds.min = Math::Vec3(position.x, -s_chunkHeight, position.y);
            bounds.max = bounds.min + Math::Vec3((F32)s_chunkSize, 2 * s_chunkHeight, (F32)s_chunkSize);

            DEBUG.drawCube(bounds.min, bounds.max, Color::RED, 20000);
            //transform->scale = Math::Vec3(size);
            //transform->rotation = Math::Quat(Math::Vec3::RIGHT, 90.0f);

            //go->addComponent<Components::MeshRenderer>(Core::Assets::MeshGenerator::CreatePlane(0.5f, Math::Random::Color()));
        }
    };

    std::queue<std::shared_ptr<TerrainChunk>> m_chunkGenerationQueue;
    std::unordered_map<Math::Vec2, std::shared_ptr<TerrainChunk>> m_terrainChunkDictionary;

private:
    //----------------------------------------------------------------------
    void _SetupShaderAndMaterial()
    {
        auto shader = RESOURCES.createShader( "ChunkShader", "/shaders/chunkVS.hlsl", "/shaders/chunkPS.hlsl" );

        ArrayList<Texture2DPtr> blockTextures;
        for (auto region : m_regions)
        {
            String path = "/textures/blocks/" + region.name + ".png";
            blockTextures.push_back( ASSETS.getTexture2D( path.c_str() ) );
        }

        auto texArr = RESOURCES.createTexture2DArray( blockTextures[0]->getWidth(), blockTextures[0]->getHeight(), 
                                                      (U32)blockTextures.size(), Graphics::TextureFormat::RGBA32, false );
        texArr->setAnisoLevel( 1 );
        texArr->setFilter( Graphics::TextureFilter::Point );

        for (U32 i = 0; i < blockTextures.size(); i++)
            texArr->setPixels( i, blockTextures[i] );
        texArr->apply();

        m_chunkMaterial = RESOURCES.createMaterial( shader );
        m_chunkMaterial->setColor( "color", Color::WHITE );
        m_chunkMaterial->setVec4( "dir", Math::Vec4( 0, -1, 1, 0 ) );
        m_chunkMaterial->setFloat( "intensity", 1.0f );
        m_chunkMaterial->setTexture( "texArray", texArr );
    }

    //----------------------------------------------------------------------
    MeshPtr _BuildMeshForRendering(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& polyvoxMesh)
    {
        auto chunk = RESOURCES.createMesh();

        ArrayList<Math::Vec3>   vertices;
        ArrayList<Math::Vec3>   normals;
        ArrayList<Math::Vec2>   materialID;
        ArrayList<Color>        colors;
        for ( auto& vertex : polyvoxMesh.getVertices() )
        {
            vertices.emplace_back( vertex.getPosition().getX(), vertex.getPosition().getY(), vertex.getPosition().getZ() );
            normals.emplace_back( vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ() );

            // Subtract minus 1, so the material range starts at 0 (cause 0 meant, there is no block)
            U8 material = static_cast<U8>( vertex.getMaterial() + 0.5 ) - 1;
            materialID.emplace_back( material );

            colors.emplace_back( Math::Random::Color() );
        }

        chunk->setVertices( vertices );
        chunk->setIndices( polyvoxMesh.getIndices() );
        chunk->setColors( colors );
        chunk->setNormals( normals );
        chunk->setUVs( materialID );

        return chunk;
    }

    //----------------------------------------------------------------------
    Texture2DPtr _GenerateColorTextureFromNoiseMap(const NoiseMap& noiseMap)
    {
        auto tex2D = RESOURCES.createTexture2D(noiseMap.getWidth(), noiseMap.getHeight(), Graphics::TextureFormat::BGRA32);
        for (U32 x = 0; x < tex2D->getWidth(); x++)
        {
            for (U32 y = 0; y < tex2D->getHeight(); y++)
            {
                F32 curHeight = noiseMap.getValue(x, y);
                for (auto region : m_regions)
                {
                    if (curHeight <= region.height)
                    {
                        tex2D->setPixel(x, y, region.color);
                        break;
                    }
                }
            }
        }
        tex2D->setAnisoLevel(1);
        tex2D->setFilter(Graphics::TextureFilter::Point);
        tex2D->apply();
        return tex2D;
    }

    //----------------------------------------------------------------------
    Texture2DPtr _GenerateNoiseTextureFromNoiseMap(const NoiseMap& noiseMap)
    {
        auto tex2D = RESOURCES.createTexture2D(noiseMap.getWidth(), noiseMap.getHeight(), Graphics::TextureFormat::BGRA32);
        for (U32 x = 0; x < tex2D->getWidth(); x++)
        {
            for (U32 y = 0; y < tex2D->getHeight(); y++)
            {
                F32 curHeight = noiseMap.getValue(x, y);
                tex2D->setPixel(x, y, Color::Lerp(Color::BLACK, Color::WHITE, curHeight));
            }
        }
        tex2D->setAnisoLevel(1);
        tex2D->setFilter(Graphics::TextureFilter::Point);
        tex2D->apply();

        return tex2D;
    }

    //----------------------------------------------------------------------
    MeshPtr _GenerateMeshFromNoiseMap(const Bounds& bounds, const NoiseMap& noiseMap, F32 maxHeight)
    {
        PolyVox::Region chunkDim(PolyVox::Vector3DInt32( bounds.min.x, bounds.min.y, bounds.min.z ),
                                 PolyVox::Vector3DInt32( bounds.max.x, bounds.max.y, bounds.max.z ));
        auto lowCorn    = chunkDim.getLowerCorner();
        auto upperCorn  = chunkDim.getUpperCorner();

        PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> mesh;
        PolyVox::CubicSurfaceExtractorWithNormals<PolyVox::LargeVolume<Block>> surfaceExtractor( &m_volData, chunkDim, &mesh );

        for (int x = 0; x < noiseMap.getWidth(); x++)
        {
            for (int z = 0; z < noiseMap.getHeight(); z++)
            {
                F32 noiseValue = noiseMap.getValue(x, z);
                F32 curHeight = noiseValue * maxHeight;

                for (int y = -s_chunkHeight; y < s_chunkHeight; y++)
                {
                    if ( y < curHeight )
                    {
                        Block block = _GetBlockFromHeight( noiseValue );
                        m_volData.setVoxelAt( bounds.min.x + x, y, bounds.min.z + z, block );
                    }
                }
            }
        }

        surfaceExtractor.execute();

        return _BuildMeshForRendering( mesh );
    }

    //----------------------------------------------------------------------
    Block _GetBlockFromHeight(F32 y)
    {
        for (auto region : m_regions)
            if (y <= region.height)
                return region.block;

        ASSERT(false);
        return Block(BlockType::Air);
    }

};
