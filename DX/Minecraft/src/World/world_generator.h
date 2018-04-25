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
        I32 textureIndex = 0;
        for ( auto& pair : BlockDatabase::Get().getBlockInfos() )
        {
            auto& blockInfo = pair.second;

            blockInfo.texIndices = (F32)textureIndex++;
            m_blockTextures.push_back( ASSETS.getTexture2D( blockInfo.topBottom ) );

            // Add sideblock if the block looks different on the side
            if (blockInfo.topBottom != blockInfo.side)
            {
                blockInfo.texIndices.y = (F32)textureIndex++;
                m_blockTextures.push_back( ASSETS.getTexture2D( blockInfo.side ) );
            }
        }

        auto texArr = RESOURCES.createTexture2DArray( m_blockTextures[0]->getWidth(), m_blockTextures[0]->getHeight(),
                                                      (U32)m_blockTextures.size(), Graphics::TextureFormat::RGBA32, false );
        texArr->setAnisoLevel( 1 );
        texArr->setFilter( Graphics::TextureFilter::Point );

        for (U32 i = 0; i < m_blockTextures.size(); i++)
            texArr->setPixels( i, m_blockTextures[i] );
        texArr->apply();

        auto shader = RESOURCES.createShader( "ChunkShader", "/shaders/chunkVS.hlsl", "/shaders/chunkPS.hlsl" );
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