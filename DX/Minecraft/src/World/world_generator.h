#pragma once
/**********************************************************************
    class: WorldGeneration (world_generator.h)

    author: S. Hau
    date: April 16, 2018

    Script which generates chunks of block data using perlin noise and
    the polyvox library to generate polygon meshes for rendering.
**********************************************************************/
#include "world.h"
#include "block_database.h"
#include "Terrain Generator/terrain_generator.h"

//**********************************************************************
class WorldGeneration : public Components::IComponent
{
    std::shared_ptr<TerrainGenerator>   m_terrainGenerator;
    ArrayList<Texture2DPtr>             m_blockTextures;

public:
    WorldGeneration(std::shared_ptr<TerrainGenerator> terrainGenerator, I32 chunkViewDistance = 10)
        : m_terrainGenerator( terrainGenerator ) 
    { 
        World::CHUNK_VIEW_DISTANCE = chunkViewDistance; 
    }
    ~WorldGeneration(){ World::Get().shutdown(); }

    //----------------------------------------------------------------------
    void init() override
    {
        _SetupShaderAndMaterial();
        World::Get().setChunkCallback( BIND_THIS_FUNC_1_ARGS( &WorldGeneration::ChunkUpdateCallback ) );
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        //static F64 yaw = 0;
        //yaw += 45.0 * delta.value;
        //auto fw = Math::Quat::FromEulerAngles({ 0, (F32)yaw, 0}).getForward();
        //m_chunkMaterial->setVec4("dir", Math::Vec4(fw.x, -fw.y, fw.z, 0));

        World::Get().update( (F32)delta.value );
    }

private:
    //----------------------------------------------------------------------
    void _SetupShaderAndMaterial()
    {
        ArrayList<BlockInfo> blockInfos;
        blockInfos.emplace_back( Block::Dirt, "/textures/blocks/dirt.png");
        blockInfos.emplace_back( Block::Sand, "/textures/blocks/sand.png");
        blockInfos.emplace_back( Block::Gravel, "/textures/blocks/gravel.png");
        blockInfos.emplace_back( Block::Stone, "/textures/blocks/stone.png");
        blockInfos.emplace_back( Block::Snow, "/textures/blocks/snow.png");
        blockInfos.emplace_back( Block::Grass, "/textures/blocks/grass_side.png", "/textures/blocks/grass_top.png" );
        blockInfos.emplace_back( Block::Oak, "/textures/blocks/log_oak.png", "/textures/blocks/log_oak_top.png");
        blockInfos.emplace_back( Block::OakLeaves, "/textures/blocks/leaves_oak.png");

        I32 textureIndex = 0;
        for (auto& blockInfo : blockInfos)
        {
            blockInfo.indices = (F32)textureIndex++;
            m_blockTextures.push_back( ASSETS.getTexture2D( blockInfo.topBottom ) );

            // Add sideblock if the block looks different on the side
            if (blockInfo.topBottom != blockInfo.side)
            {
                blockInfo.indices.y = (F32)textureIndex++;
                m_blockTextures.push_back( ASSETS.getTexture2D( blockInfo.side ) );
            }

            BlockDatabase::AddBlockInfo( blockInfo );
        }

        auto texArr = RESOURCES.createTexture2DArray( m_blockTextures[0]->getWidth(), m_blockTextures[0]->getHeight(),
                                                      (U32)m_blockTextures.size(), Graphics::TextureFormat::RGBA32, false );
        texArr->setAnisoLevel( 1 );
        texArr->setFilter( Graphics::TextureFilter::Point );

        for (U32 i = 0; i < m_blockTextures.size(); i++)
            texArr->setPixels( i, m_blockTextures[i] );
        texArr->apply();

        auto shader = RESOURCES.createShader( "ChunkShader", "/shaders/chunkVS.hlsl", "/shaders/chunkPS.hlsl" );
        shader->enableAlphaBlending();

        auto chunkMaterial = RESOURCES.createMaterial( shader );
        chunkMaterial->setColor( "color", Color::WHITE );
        chunkMaterial->setVec4( "dir", Math::Vec4( 0, -1, 1, 0 ) );
        chunkMaterial->setFloat( "intensity", 1.0f );
        chunkMaterial->setTexture( "texArray", texArr );

        World::CHUNK_MATERIAL = chunkMaterial;
    }

    //----------------------------------------------------------------------
    void ChunkUpdateCallback(Chunk& chunk)
    {
        m_terrainGenerator->generateTerrainFor(chunk);
    }

};
