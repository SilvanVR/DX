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
#include "PolyVoxCore/LargeVolume.h"
#include "PolyVoxCore/Raycast.h"
#include "noise_map.h"

#include "Math/aabb.h"
#include "Physics/ray.h"

inline Math::Vec3               ConvertVector(const PolyVox::Vector3DFloat& v) { return Math::Vec3( v.getX(), v.getY(), v.getZ() ); }
inline Math::Vec3               ConvertVector(const PolyVox::Vector3DInt32& v) { return Math::Vec3( v.getX(), v.getY(), v.getZ() ); }
inline PolyVox::Vector3DFloat   ConvertVector(const Math::Vec3& v) { return { v.x, v.y, v.z }; }

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

typedef TBlock<U8> Block;

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
    static const I32 s_chunkSize    = 16;
    static const I32 s_chunkHeight  = 128;

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

    F32     m_chunksVisibleInViewDst;

    enum class DrawMode
    {
        NoiseMap,
        ColorMap
    } m_drawMode = DrawMode::ColorMap;

public:
    WorldGeneration(I32 chunkViewDistance = 10) 
        : m_chunksVisibleInViewDst( chunkViewDistance ), 
          m_volData(PolyVox::Region(PolyVox::Vector3DInt32(INT_MIN, -s_chunkHeight, INT_MIN),
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
                NoiseMap noiseMap(s_chunkSize * 10.0f, s_chunkSize * 10.0f, m_noiseScale, m_noiseLacunarity, m_noiseGain, m_noiseOctaves );
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

        I32 currentChunkCoordX = static_cast<I32>( std::floorf( transform->position.x / s_chunkSize ) );
        I32 currentChunkCoordY = static_cast<I32>( std::floorf( transform->position.z / s_chunkSize ) );

        for (I32 ring = 0; ring < m_chunksVisibleInViewDst; ring++)
        {
            for (I32 x = -ring; x <= ring; x++)
            {
                for (I32 y = -ring; y <= ring; y++)
                {
                    if ( std::abs(x) == ring || std::abs(y) == ring)
                    {
                        Math::Vec2 viewedChunkCoord( currentChunkCoordX + x, currentChunkCoordY + y );

                        if (m_terrainChunkDictionary.find(viewedChunkCoord) != m_terrainChunkDictionary.end())
                        {
                            m_terrainChunkDictionary[viewedChunkCoord]->go->setActive(true);
                        }
                        else
                        {
                            auto newChunk = std::make_shared<TerrainChunk>(viewedChunkCoord, s_chunkSize);
                            m_terrainChunkDictionary[viewedChunkCoord] = newChunk;
                            m_chunkGenerationQueue.push(newChunk);
                        }
                    }
                }
            }
        }

        // Generate new chunk if requested and not currently generating one
        if ( not m_chunkGenerationQueue.empty() && not m_generating )
        {
            m_generating = true;
            auto nextChunk = m_chunkGenerationQueue.front();

            ASYNC_JOB([=] {
                // Noise map must be one pixel larger, so it fills the boundary chunks aswell, otherwise the mesh will contain holes
                NoiseMap noiseMap( s_chunkSize + 1, s_chunkSize + 1, m_noiseScale, m_noiseLacunarity, m_noiseGain, m_noiseOctaves, nextChunk->position);
                auto mesh = _GenerateMeshFromNoiseMap(nextChunk->bounds, noiseMap, m_terrainHeight);

                auto mr = nextChunk->go->getComponent<Components::MeshRenderer>();
                mr->setMesh(mesh);
                mr->setMaterial(m_chunkMaterial);
                m_generating = false;
            });

            m_chunkGenerationQueue.pop();
        }

        if (KEYBOARD.wasKeyPressed(Key::C))
        {
            F32 rayDistance = 100.0f;
            auto viewerDir = transform->rotation.getForward();

            Physics::Ray ray(transform->position, viewerDir * rayDistance);

            ChunkRayCastResult result;
            if ( ChunkRayCast(ray, &result) )
            {
                DEBUG.drawSphere(result.hitPoint, 0.1f, Color::RED, 10);
                DEBUG.drawRay(transform->position, ray.getDirection(), Color::BLUE, 10);

                Math::Vec3 blockSize(0.5f, 0.5f, 0.5f);
                Math::Vec3 min = result.blockCenter - blockSize;
                Math::Vec3 max = result.blockCenter + blockSize;
                DEBUG.drawCube(min, max, Color::GREEN, 10);
            }
        }
    }

    struct ChunkRayCastResult
    {
        Math::Vec3  hitPoint = Math::Vec3(0,0,0);
        Math::Vec3  blockCenter;
        Block       block = Block(BlockType::Air);
    };

    bool ChunkRayCast(const Physics::Ray& ray, ChunkRayCastResult* result)
    {
        auto cb = [&](const PolyVox::LargeVolume<Block>::Sampler& sampler) -> bool
        {
            auto voxel = sampler.getVoxel();
            if (voxel.getMaterial() == (U8)BlockType::Air)
                return true;

            result->block       = voxel;
            result->blockCenter = ConvertVector( sampler.getPosition() );

            // Since polyvox raycast does not deliever the exact hit-point but rather the block, it must be calculated separately
            Math::Vec3 blockSize(0.5f, 0.5f, 0.5f);
            Math::AABB blockBounds(result->blockCenter - blockSize, result->blockCenter + blockSize);

            Physics::RayCastResult rayResult;
            if ( ray.intersects( blockBounds, &rayResult ) )
                result->hitPoint = rayResult.hitPoint;

            return false;
        };

        PolyVox::Vector3DFloat start( ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z );
        PolyVox::Vector3DFloat dir( ray.getDirection().x, ray.getDirection().y, ray.getDirection().z );

        auto polyVoxResult = PolyVox::raycastWithDirection( &m_volData, start, dir, cb );

        return (polyVoxResult == PolyVox::RaycastResult::Interupted);
    }

    struct TerrainChunk
    {
        GameObject* go;
        Math::Vec2  position;
        Math::AABB  bounds;
        
        TerrainChunk(const Math::Vec2& pos, I32 size) 
            : position(pos * size)
        {
            F32 yPos = -s_chunkHeight - 15.0f;
            Math::Vec3 posV3(position.x, yPos, position.y);
            go = SCENE.createGameObject("CHUNK");
            auto transform = go->getTransform();
            transform->position = posV3;

            bounds.getMin() = Math::Vec3(position.x, yPos, position.y);
            bounds.getMax() = bounds.getMin() + Math::Vec3((F32)s_chunkSize, 2 * s_chunkHeight, (F32)s_chunkSize);

            go->addComponent<Components::MeshRenderer>();

            DEBUG.drawCube(bounds.getMin(), bounds.getMax(), Color::GREEN, 20000);
        }

        //void generateMesh(const NoiseMap& noiseMap)
        //{
        //    auto mesh = _GenerateMeshFromNoiseMap( bounds, noiseMap, m_terrainHeight );
        //    go->addComponent<Components::MeshRenderer>(mesh, m_chunkMaterial);
        //}
    };

    std::queue<std::shared_ptr<TerrainChunk>>                       m_chunkGenerationQueue;
    std::unordered_map<Math::Vec2, std::shared_ptr<TerrainChunk>>   m_terrainChunkDictionary;

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
        ArrayList<Math::Vec2>   materials;
        for ( auto& vertex : polyvoxMesh.getVertices() )
        {
            vertices.emplace_back( vertex.getPosition().getX(), vertex.getPosition().getY(), vertex.getPosition().getZ() );
            normals.emplace_back( vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ() );

            // Subtract minus 1, so the material range starts at 0 (cause 0 meant, there is no block)
            U8 material = static_cast<U8>( vertex.getMaterial() + 0.5 ) - 1;
            materials.emplace_back(Math::Vec2(material, material));
        }

        chunk->setVertices( vertices );
        chunk->setIndices( polyvoxMesh.getIndices() );
        chunk->setNormals( normals );
        chunk->setUVs( materials );

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
                F32 curHeight = noiseMap.get(x, y);
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
                F32 curHeight = noiseMap.get(x, y);
                tex2D->setPixel(x, y, Color::Lerp(Color::BLACK, Color::WHITE, curHeight));
            }
        }
        tex2D->setAnisoLevel(1);
        tex2D->setFilter(Graphics::TextureFilter::Point);
        tex2D->apply();

        return tex2D;
    }

    //----------------------------------------------------------------------
    MeshPtr _GenerateMeshFromNoiseMap(const Math::AABB& bounds, const NoiseMap& noiseMap, F32 maxHeight)
    {
        PolyVox::Region chunkDim(PolyVox::Vector3DInt32( bounds.getMin().x, bounds.getMin().y, bounds.getMin().z ),
                                 PolyVox::Vector3DInt32( bounds.getMax().x, bounds.getMax().y, bounds.getMax().z ));

        PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> mesh;
        PolyVox::CubicSurfaceExtractorWithNormals<PolyVox::LargeVolume<Block>> surfaceExtractor( &m_volData, chunkDim, &mesh );

        for (int x = 0; x < noiseMap.getWidth(); x++)
        {
            for (int z = 0; z < noiseMap.getHeight(); z++)
            {
                F32 noiseValue = noiseMap.get(x, z);
                F32 curHeight = noiseValue * maxHeight;

                for (int y = -s_chunkHeight; y < s_chunkHeight; y++)
                {
                    if ( y < curHeight )
                    {
                        Block block = _GetBlockFromHeight( noiseValue );
                        m_volData.setVoxelAt( bounds.getMin().x + x, y, bounds.getMin().z + z, block );
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
