#include "particle_system.h"
/**********************************************************************
    class: ParticleSystem

    author: S. Hau
    date: July 10, 2018
**********************************************************************/

#include "GameplayLayer/gameobject.h"
#include "GameplayLayer/Components/transform.h"
#include "Graphics/command_buffer.h"
#include "GameplayLayer/i_scene.h"
#include "Core/mesh_generator.h"
#include "OS/FileSystem/file.h"
#include "Core/locator.h"
#include "Math/random.h"
#include "camera.h"

#include "Ext/JSON/json.hpp"

namespace Components {

    static const StringID SHADER_NAME_MODEL_MATRIX = SID( "MODEL" );

    //----------------------------------------------------------------------
    ParticleSystem::ParticleSystem( const OS::Path& path )
    {
        m_particleMesh = Core::MeshGenerator::CreatePlane();
        _LoadFromFile( path );
        play();
    }

    //----------------------------------------------------------------------
    ParticleSystem::ParticleSystem( const MaterialPtr& material, bool playOnStart )
        : m_material{ material }
    {
        m_paused = not playOnStart;
        m_particleMesh = Core::MeshGenerator::CreatePlane();
        setParticleAlignment( ParticleAlignment::View );
        setSortMode( SortMode::ByDistance );
        play();
    }

    //----------------------------------------------------------------------
    void ParticleSystem::tick( Time::Seconds delta )
    {
        if (m_paused)
            return;

        bool isRunning = m_clock.tick( delta );
        if (isRunning)
        {
            auto clockDelta = m_clock.getDelta();
            _SpawnParticles( clockDelta );
            _UpdateParticles( clockDelta );
        }
        else
        {
            // Clock is has exceeded his duration, but remaining particles must still be updated
            _UpdateParticles( delta * m_clock.getTickModifier() );
        }

        _AlignParticles( m_particleAlignment );
        _SortParticles( m_sortMode );
        _UpdateMesh();
    }

    //----------------------------------------------------------------------
    void ParticleSystem::recordGraphicsCommands( Graphics::CommandBuffer& cmd, F32 lerp )
    {
        auto transform = getGameObject()->getTransform();
        ASSERT( transform != nullptr );

        // Draw instanced mesh with appropriate material
        auto modelMatrix = transform->getWorldMatrix( lerp );
        cmd.drawMeshInstanced( m_particleMesh, m_material, modelMatrix, m_currentParticleCount );
    }

    //----------------------------------------------------------------------
    bool ParticleSystem::cull( const Graphics::Camera& camera )
    {
        if ( (m_maxParticleCount == 0) || (m_currentParticleCount == 0) )
            return false;

        if (not m_material)
        {
            LOG_WARN( "Particle system has no material! Please set a material." );
            return false;
        }

        if (not m_particleMesh)
        {
            LOG_WARN( "Particle system has no mesh! Please set a mesh." );
            return false;
        }

        return true;
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void ParticleSystem::play()
    {
        m_paused = false;
        m_currentParticleCount = 0;
        m_accumulatedSpawnTime = 0.0f;
        m_clock.setTickModifier( 1.0f );
        m_clock.setTime( 0_ms );
        m_particles.resize( m_maxParticleCount );

        // Make sure the mesh is a dynamic mesh
        if ( m_particleMesh->isImmutable() )
            m_particleMesh->setBufferUsage( Graphics::BufferUsage::Frequently );

        // Create vertex streams
        m_particleMesh->createVertexStream<DirectX::XMMATRIX>( SHADER_NAME_MODEL_MATRIX, m_maxParticleCount );
        m_particleMesh->createVertexStream<Math::Vec4>( Graphics::SID_VERTEX_COLOR, m_maxParticleCount );
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void ParticleSystem::_SpawnParticles( Time::Seconds delta )
    {
        // How much spawns we should do this tick
        F32 spawnsPerTick = m_emissionRate * (F32)delta;
        m_accumulatedSpawnTime += spawnsPerTick;

        while (m_accumulatedSpawnTime > 1.0f && m_currentParticleCount < m_maxParticleCount)
        {
            _SpawnParticle( m_currentParticleCount );
            m_currentParticleCount++;
            m_accumulatedSpawnTime -= 1.0f;
        }
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_SpawnParticle( U32 particleIndex )
    {
        m_particles[particleIndex].startLifetime = m_particles[particleIndex].remainingLifetime = m_spawnLifeTimeFnc();

        m_particles[particleIndex].spawnScale    = m_particles[particleIndex].scale = m_spawnScaleFnc();        
        m_particles[particleIndex].spawnRotation = m_particles[particleIndex].rotation = m_spawnRotationFnc();
        m_particles[particleIndex].position      = m_spawnPositionFnc();

        m_particles[particleIndex].spawnColor    = m_particles[particleIndex].color = m_spawnColorFnc();
        m_particles[particleIndex].velocity      = m_spawnVelocityFnc();
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_UpdateParticles( Time::Seconds delta )
    {
        for (U32 i = 0; i < m_currentParticleCount;)
        {
            m_particles[i].remainingLifetime -= delta;
            m_particles[i].velocity -= Math::Vec3{ 0, m_gravity * (F32)delta, 0 };
            m_particles[i].position += m_particles[i].velocity * (F32)delta;

            // From 0 - 1 across the whole lifetime of the particle. 0 means particle just spawned, 1 it's near death.
            F32 lifeTimeNormalized = 1 - ((F32)m_particles[i].remainingLifetime / (F32)m_particles[i].startLifetime);
            lifeTimeNormalized = std::min( lifeTimeNormalized, 1.0f );

            m_particles[i].rotation = m_particles[i].spawnRotation; // Set always rotation here, so particles can be aligned later on
            if (m_lifeTimeRotationFnc)
                m_particles[i].rotation *= m_lifeTimeRotationFnc( lifeTimeNormalized );

            if (m_lifeTimeScaleFnc)
                m_particles[i].scale = m_particles[i].spawnScale * m_lifeTimeScaleFnc( lifeTimeNormalized );

            if (m_lifeTimeColorFnc)
                m_particles[i].color = m_particles[i].spawnColor * m_lifeTimeColorFnc( lifeTimeNormalized );

            if (m_particles[i].remainingLifetime < 0_s)
            {
                // Move last living particle in this slot
                m_particles[i] = m_particles[m_currentParticleCount - 1];
                --m_currentParticleCount;
            }
            else
            {
                ++i;
            }
        }
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_AlignParticles( ParticleAlignment alignment )
    {
        switch (alignment)
        {
        case ParticleAlignment::View:
        {
            // 1. Since the view matrix rotates everything "eyeRot" backwards, to negate it we just add the eyeRot itself
            // 2. To negate the world rotation itself, we just need the conjugate
            auto worldRot = getGameObject()->getTransform()->getWorldRotation();
            auto& eyeRot = SCENE.getMainCamera()->getGameObject()->getTransform()->rotation;
            auto alignedRotation = eyeRot * worldRot.conjugate();
            for (U32 i = 0; i < m_currentParticleCount; ++i)
                m_particles[i].rotation *= alignedRotation;
            break;
        }
        case ParticleAlignment::None: break;
        }
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_SortParticles( SortMode sortMode )
    {
        switch (sortMode)
        {
        case SortMode::ByDistance:
        {
            auto worldMatrix = getGameObject()->getTransform()->getWorldMatrix();
            auto eyePos = DirectX::XMLoadFloat3( &SCENE.getMainCamera()->getGameObject()->getTransform()->position );

            // Sorting particles by distance to camera comes with one caveat:
            // 1.) Floating point precision can cause incorrect ordering when the camera moves around the particle
            //     Solution: Disable Z-Writes
            std::sort( m_particles.begin(), m_particles.begin() + m_currentParticleCount, [worldMatrix, eyePos](const Particle& p1, const Particle& p2) {
                auto vPos1 = DirectX::XMLoadFloat3( &p1.position );
                auto vPos2 = DirectX::XMLoadFloat3( &p2.position );
                auto pos1 = DirectX::XMVector3Transform( vPos1, worldMatrix );
                auto pos2 = DirectX::XMVector3Transform( vPos2, worldMatrix );

                auto distance1 = DirectX::XMVector3LengthSq( DirectX::XMVectorSubtract( eyePos, pos1 ) );
                auto distance2 = DirectX::XMVector3LengthSq( DirectX::XMVectorSubtract( eyePos, pos2 ) );

                return DirectX::XMVector3Greater( distance1, distance2 );
            } );
            break;
        }
        case SortMode::None: break;
        }
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_UpdateMesh()
    {
        auto& matrixStream = m_particleMesh->getVertexStream<DirectX::XMMATRIX>( SHADER_NAME_MODEL_MATRIX );
        auto& colorStream = m_particleMesh->getVertexStream<Math::Vec4>( Graphics::SID_VERTEX_COLOR );
        for (U32 i = 0; i < m_currentParticleCount; ++i)
        {
            DirectX::XMVECTOR s = DirectX::XMLoadFloat3( &m_particles[i].scale );
            DirectX::XMVECTOR r = DirectX::XMLoadFloat4( &m_particles[i].rotation );
            DirectX::XMVECTOR p = DirectX::XMLoadFloat3( &m_particles[i].position );
            matrixStream[i] = DirectX::XMMatrixAffineTransformation( s, DirectX::XMQuaternionIdentity(), r, p );

            colorStream[i] = m_particles[i].color.normalized();
        }
    }

    //----------------------------------------------------------------------
    static Math::Vec3 ParseVec3( const nlohmann::json& value )
    {
        Math::Vec3 result;
      
        if ( value.find("x") != value.end() )
            result.x = value["x"];
        else if ( value.find("r") != value.end() )
            result.x = value["r"];

        if ( value.find("y") != value.end() )
            result.y = value["y"];
        else if ( value.find("g") != value.end() )
            result.y = value["g"];

        if ( value.find("z") != value.end() )
            result.z = value["z"];
        else if ( value.find("b") != value.end() )
            result.z = value["b"];

        return result;
    }

    //----------------------------------------------------------------------
    static Color ParseColor( const nlohmann::json& value )
    {
        // Vec4 are valid in hex-format or as object
        if ( value.is_object() ) // i.e. {"x" : 1, "y" : 0, "z" : 0, "w" : 1} or {"r" : 1, "g" : 0, "b" : 0, "a" : 1}
        {
            Math::Vec4 color{ 255.0f };
            auto r = value.find("r");
            if (r != value.end()) color.x = *r;

            auto g = value.find("g");
            if (g != value.end()) color.y = *g;

            auto b = value.find("b");
            if (b != value.end()) color.z = *b;

            auto a = value.find("a");
            if (a != value.end()) color.w = *a;

            return Color( (Byte)color.x, (Byte)color.y, (Byte)color.z, (Byte)color.w );
        }
        else if ( value.is_string() ) // i.e. "#FF00FF" or "#FF00FFFF"
        {
            String hex = value;
            return Color( hex );
        }
        else
        {
            LOG_WARN( "Failed to parse color." );
            return Color::WHITE;
        }
    }

    //----------------------------------------------------------------------
    void ParticleSystem::_LoadFromFile( const OS::Path& path )
    {
        try
        {
           OS::BinaryFile psFile( path, OS::EFileMode::READ );

           nlohmann::json json;
           try {
               json = nlohmann::json::parse( psFile.readAll() );
           }
           catch (const nlohmann::detail::parse_error& e) {
               LOG_WARN( "Failed to parse '"+ path.toString() +"' as JSON. Reason: " + e.what() );
               m_material = ASSETS.getErrorMaterial();
               return;
           }

           try {
                auto material = json["material"];
                if (material != nullptr)
                {
                    String path = material;
                    m_material = ASSETS.getMaterial( path );
                }
                else
                {
                    LOG_WARN( "Particle System file has no [material] parameter. Please add one." );
                }

                auto duration = json["duration"];
                if (duration != nullptr)
                {
                    F32 val = duration;
                    m_clock.setDuration( Time::Seconds( val ) );
                }

                auto looping = json["looping"];
                if (looping != nullptr)
                    m_clock.setIsLooping( looping );

                auto maxParticles = json["maxParticles"];
                if (maxParticles != nullptr)
                    m_maxParticleCount = maxParticles;

                auto emissionRate = json["emissionRate"];
                if (emissionRate != nullptr)
                    m_emissionRate = emissionRate;

                auto gravity = json["gravity"];
                if (gravity != nullptr)
                    m_gravity = gravity;

                auto align = json["align"];
                if (align != nullptr)
                {
                    String alignment = align;
                    if (alignment == "view") m_particleAlignment = ParticleAlignment::View;
                    else LOG_WARN( "Could not read 'align' property." );
                }
                
                auto sort = json["sort"];
                if (sort != nullptr)
                {
                    String sortMode = sort;
                    if (sortMode == "byDistance") m_sortMode = SortMode::ByDistance;
                    else LOG_WARN( "Could not read 'sort' property." );
                }

                auto lifetime = json["lifetime"];
                if (lifetime != nullptr)
                {
                    if (lifetime.is_number())
                    {
                        F32 val = lifetime;
                        m_spawnLifeTimeFnc = Constant(val);
                    }
                    else
                    {
                        F32 min = lifetime["min"];
                        F32 max = lifetime["max"];
                        m_spawnLifeTimeFnc = RandomBetweenTwoConstants(min, max);
                    }
                }

                auto volume = json["volume"];
                if (volume != nullptr)
                {
                    auto box = volume["box"];
                    if (box != nullptr)
                    {
                        auto min = ParseVec3( box["min"] );
                        auto max = ParseVec3( box["max"] );
                        m_spawnPositionFnc = ShapeBox( min, max );
                    }

                    auto sphere = volume["sphere"];
                    if (sphere != nullptr)
                    {
                        auto center = ParseVec3( sphere["center"] );
                        F32 radius = sphere["radius"];
                        m_spawnPositionFnc = ShapeSphere( center, radius );
                    }
                }

                auto size = json["size"];
                if (size != nullptr)
                {
                    if (size.is_number())
                    {
                        F32 val = size;
                        m_spawnScaleFnc = Constant( val );
                    }
                    else
                    {
                        auto spawn = size["spawn"];
                        if (spawn != nullptr)
                        {
                            F32 min = spawn["min"];
                            F32 max = spawn["max"];
                            m_spawnScaleFnc = RandomBetweenTwoConstants( min, max );
                        }

                        auto lifetime = size["lifetime"];
                        if (lifetime != nullptr)
                        {
                            HashMap<F32, F32> m_dataMap;
                            for (auto it = lifetime.begin(); it != lifetime.end(); ++it)
                            {
                                String key = it.key();
                                F32 keyAsNum = (F32)std::atof( key.c_str() );
                                F32 val = it.value();
                                m_dataMap[keyAsNum] = val;
                            }
                            m_lifeTimeScaleFnc = LinearLerpBetweenValues( m_dataMap );
                        }
                    }
                }

                auto rotation = json["rotation"];
                if (rotation != nullptr)
                {  
                    auto spawn = rotation["spawn"];
                    if (spawn != nullptr)
                    {
                        auto min = ParseVec3( spawn["min"] );
                        auto max = ParseVec3( spawn["max"] );
                        m_spawnRotationFnc = [min, max]() { return Math::Quat::FromEulerAngles( Math::Random::Vec3( min, max ) ); };
                    }

                    auto lifetime = rotation["lifetime"];
                    if (lifetime != nullptr)
                    {
                        HashMap<F32, Math::Quat> m_dataMap;
                        for (auto it = lifetime.begin(); it != lifetime.end(); ++it)
                        {
                            String key = it.key();
                            F32 keyAsNum = (F32)std::atof( key.c_str() );
                            auto val = ParseVec3( it.value() );
                            m_dataMap[keyAsNum] = Math::Quat::FromEulerAngles( val );
                        }
                        m_lifeTimeRotationFnc = LinearLerpBetweenValues( m_dataMap );
                    }
                }

                auto color = json["color"];
                if (color != nullptr)
                {
                    auto spawn = color["spawn"];
                    if (spawn != nullptr)
                    {
                        auto min = ParseColor( spawn["min"] );
                        auto max = ParseColor( spawn["max"] );
                        m_spawnColorFnc = RandomBetweenTwoConstants( min, max );
                    }

                    auto lifetime = color["lifetime"];
                    if (lifetime != nullptr)
                    {
                        HashMap<F32, Color> m_dataMap;
                        for (auto it = lifetime.begin(); it != lifetime.end(); ++it)
                        {
                            String key = it.key();
                            F32 keyAsNum = (F32)std::atof( key.c_str() );
                            auto val = ParseColor( it.value() );
                            m_dataMap[keyAsNum] = val;
                        }
                        m_lifeTimeColorFnc = LinearLerpBetweenValues( m_dataMap );
                    }
                }

                auto velocity = json["velocity"];
                if (velocity != nullptr)
                {
                    auto spawn = velocity["spawn"];
                    if (spawn != nullptr)
                    {
                        auto min = ParseVec3( spawn["min"] );
                        auto max = ParseVec3( spawn["max"] );
                        m_spawnVelocityFnc = ShapeBox( min, max );
                    }

                    auto lifetime = velocity["lifetime"];
                    if (lifetime != nullptr)
                    {
                        //HashMap<F32, Color> m_dataMap;
                        //for (auto it = lifetime.begin(); it != lifetime.end(); ++it)
                        //{
                        //    String key = it.key();
                        //    F32 keyAsNum = (F32)std::atof(key.c_str());
                        //    auto val = ParseColor(it.value());
                        //    m_dataMap[keyAsNum] = val;
                        //}
                        //m_lifeTimeColorFnc = LinearLerpBetweenValues(m_dataMap);
                    }
                }
           }
           catch (const nlohmann::detail::type_error& e) {
               LOG_WARN( e.what() );
           }
        }
        catch (const std::runtime_error& e)
        {
            LOG_WARN( "ParticleSystem: Failed to open '" + path.toString() + "'. Reason: " + e.what() );
        }
    }

}