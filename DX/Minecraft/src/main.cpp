#include <DX.h>
#define DISPLAY_CONSOLE 1

#include "world_generator.h"

#ifdef _DEBUG
    const char* gameName = "[DEBUG] Minecraft";
#else
    const char* gameName = "[RELEASE] Minecraft";
#endif

#define ACTION_NAME_DIG     "dig"
#define ACTION_NAME_PLACE   "place"
#define ACTION_NAME_JUMP    "jump"

static const F32 GRAVITY = -0.7f;

//**********************************************************************
class DrawFrustum : public Components::IComponent
{
public:
    void addedToGameObject(GameObject* go) override
    {
        auto cam = go->getComponent<Components::Camera>();
        auto mesh = Core::Assets::MeshGenerator::CreateFrustum(Math::Vec3(0), Math::Vec3::UP, Math::Vec3::RIGHT, Math::Vec3::FORWARD,
            cam->getFOV(), cam->getZNear(), cam->getZFar(), cam->getAspectRatio());

        auto mr = go->addComponent<Components::MeshRenderer>();
        mr->setMesh(mesh);
    }
};

//**********************************************************************
class Minimap : public Components::IComponent
{
    Components::Transform* minimapCamTransform;
    Components::Transform* transform;
    F32 m_heightOffset;
    F32 m_size;

public:
    Minimap(F32 heightOffset, F32 size) : m_heightOffset(heightOffset), m_size(size) {}

    void addedToGameObject(GameObject* go) override
    {
        auto minimapCamGo = go->getScene()->createGameObject();
        auto cam = minimapCamGo->addComponent<Components::Camera>();
        cam->setClearMode(Components::Camera::EClearMode::NONE);
        cam->setCameraMode(Components::Camera::EMode::ORTHOGRAPHIC);
        cam->setOrthoParams(-m_size, m_size,-m_size, m_size,0,1000);
        cam->getViewport().height = 0.3f;
        cam->getViewport().width = 0.3f;
        //minimapCamGo->addComponent<DrawFrustum>();

        transform = go->getComponent<Components::Transform>();
        minimapCamTransform = minimapCamGo->getComponent<Components::Transform>();
        minimapCamTransform->position.y = m_heightOffset;
        minimapCamTransform->rotation = Math::Quat::LookRotation(Math::Vec3::DOWN, Math::Vec3::FORWARD);
    }

    void tick(Time::Seconds delta) override
    {
        minimapCamTransform->position.x = transform->position.x;
        minimapCamTransform->position.z = transform->position.z;
    }
};

//**********************************************************************
class MusicManager : public Components::IComponent
{
    ArrayList<AudioClipPtr> m_tracks;

    I32           m_curIndex        = 0;
    I32           m_nextIndex       = 1;
    Time::Seconds m_timer           = 0.0f;
    Time::Seconds m_transitionTime  = 5.0f;

public:
    MusicManager(const ArrayList<OS::Path>& musicFilePaths)
    {
        for (auto& path : musicFilePaths)
            m_tracks.push_back(ASSETS.getAudioClip(path));

        if ( not m_tracks.empty() )
            m_tracks[m_curIndex]->play();
    }

    void tick(Time::Seconds delta) override
    {
        static bool startNextTrack = true;
        if ( m_tracks.size() <= 1 )
            return;

        m_timer += delta;

        if ( m_timer > (m_tracks[m_curIndex]->getLength() - m_transitionTime) )
        {
            if (startNextTrack)
            {
                m_tracks[m_nextIndex]->play();
                startNextTrack = false;
            }

            // Smoothly transition between next two tracks
            F32 volume = (F32)((m_tracks[m_curIndex]->getLength() - m_timer) / m_transitionTime).value;
            volume = Math::clamp( volume, 0.0f, 1.0f );

            m_tracks[m_curIndex]->setVolume( volume );
            m_tracks[m_nextIndex]->setVolume( 1.0f - volume );

            // Check when fully transitioned to next track and stop then
            if ( volume == 0.0f )
            {
                m_tracks[m_curIndex]->stop();
                m_curIndex      = m_nextIndex;
                m_nextIndex     = (m_nextIndex + 1) % m_tracks.size();
                m_timer         = 0.0f;
                startNextTrack  = true;
            }
        }
    }
};

//**********************************************************************
class PlayerInventory : public Components::IComponent, public Core::Input::IMouseListener
{
    static const I32 SLOT_COUNT = 8;
    U32 m_maxCountPerItem;

    struct BlockInfo
    {
        U32 count;
        I32 slot;
    };
    HashMap<Block, BlockInfo> m_inventory;

    I32     m_curSlot = 0;
    Block   m_slots[SLOT_COUNT];

public:
    PlayerInventory(U32 maxCountPerItem = 64) : m_maxCountPerItem(maxCountPerItem) {}

    void OnMouseWheel(I16 delta) override
    {
        // Change slot per mouse wheel
        m_curSlot = (m_curSlot + delta) % SLOT_COUNT;
        m_curSlot = m_curSlot < 0 ? 0 : m_curSlot;
        LOG(toString(), Color::GREEN);
    }

    // @Return:
    //  False, when block can't be added to inventory because its full
    bool add(Block block) 
    { 
        U32 curCount = m_inventory[block].count; // Does it always initialize with 0?
        if (curCount == m_maxCountPerItem)
            return false;

        if (curCount == 0)
        {
            I32 nextFreeSlot = _NextFreeSlot();
            if (nextFreeSlot < 0)
                return false;
            m_inventory[block].slot = nextFreeSlot;
            m_slots[nextFreeSlot] = block;
        }

        m_inventory[block].count++;
        return true;
    }

    // @Return:
    //  False, because the block doesnt exist in the inventory
    bool remove(Block block)
    {
        if (m_inventory.find(block) == m_inventory.end())
            return false;

        m_inventory[block].count--;
        if (m_inventory[block].count == 0)
        {
            m_slots[m_inventory[block].slot] = Block::Air;
            m_inventory.erase(block);
        }
        return true;
    }

    // @Return:
    //  Block count for given block
    U32 get(Block block) { return m_inventory[block].count; }

    // @Return:
    //  Block at current slot
    Block getCurrentBlock()
    {
        Block block = m_slots[m_curSlot];
        if ( not remove( block ) )
            return Block::Air;

        return block;
    }

    String toString()
    {
        String result = "\n<<<< Inventory >>>>";
        if (m_inventory.empty()) return result + "\nEmpty!";
        for (auto& pair : m_inventory)
            result += "\n" + pair.first.toString() + ": " + TS(pair.second.count) + " ["+ TS(pair.second.slot)+ "]";
        result += "\nCurrent Slot: " + TS(m_curSlot) + "(" + m_slots[m_curSlot].toString() + ")";
        return result;
    }

private:
    I32 _NextFreeSlot() 
    {
        for (I32 i = 0; i < SLOT_COUNT; i++)
            if (m_slots[i] == Block::Air)
                return i;
        return -1;
    }
};

//**********************************************************************
class PlayerActions : public Components::IComponent
{
    F32 rayDistance;
    Components::Transform* transform;

public:
    PlayerActions(F32 placeDistance = 10.0f) : rayDistance(placeDistance) {}

    void addedToGameObject(GameObject* go) override
    {
        ACTION_MAPPER.attachMouseEvent(ACTION_NAME_DIG, MouseKey::LButton);
        //ACTION_MAPPER.attachMouseEvent(ACTION_NAME_PLACE, MouseKey::RButton);
        ACTION_MAPPER.attachKeyboardEvent(ACTION_NAME_PLACE, Key::E);
        transform = getComponent<Components::Transform>();
    }

    void tick(Time::Seconds delta) override
    {
        auto viewerDir = transform->rotation.getForward();
        Physics::Ray ray(transform->position, viewerDir * rayDistance);

        // Preview block
        World::Get().RayCast(ray, [](const ChunkRayCastResult& result){
            Math::Vec3 blockSize(BLOCK_SIZE * 0.5f);
            Math::Vec3 min = result.blockCenter - blockSize;
            Math::Vec3 max = result.blockCenter + blockSize;
            DEBUG.drawCube( min, max, Color::GREEN, 0 );
        });

        // Remove blocks
        if ( ACTION_MAPPER.wasKeyPressed(ACTION_NAME_DIG) )
        {
            World::Get().RayCast(ray, [=](const ChunkRayCastResult& result) {
                World::Get().SetVoxelAt((I32)result.blockCenter.x, (I32)result.blockCenter.y, (I32)result.blockCenter.z, Block::Air);
                //DEBUG.drawSphere(result.hitPoint, 0.1f, Color::RED, 10);
                //DEBUG.drawRay(transform->position, ray.getDirection(), Color::BLUE, 10);
            });
        }

        // Place block
        if ( ACTION_MAPPER.wasKeyPressed(ACTION_NAME_PLACE) )
        {
            World::Get().RayCast(ray, [=](const ChunkRayCastResult& result) {
                Math::Vec3 hitDir = result.hitPoint - result.blockCenter;

                // Calculate where to place the new block by checking which component has the largest abs.
                Math::Vec3 axis(0, 0, hitDir.z);
                F32 xAbs = std::abs(hitDir.x);
                F32 yAbs = std::abs(hitDir.y);
                F32 zAbs = std::abs(hitDir.z);
                if (xAbs > yAbs && xAbs > zAbs)
                    axis = Math::Vec3(hitDir.x, 0, 0);
                else if (yAbs > xAbs && yAbs > zAbs)
                    axis = Math::Vec3(0, hitDir.y, 0);

                // Set world voxel
                Math::Vec3 block = result.blockCenter + axis.normalized();
                World::Get().SetVoxelAt((I32)block.x, (I32)block.y, (I32)block.z, Block::Sand);
            });
        }
    }
};

//**********************************************************************
class PlayerController : public Components::IComponent
{
    F32 rayDistance;
    Components::Transform* transform;
    PlayerInventory* inventory;
    U32 curInventoryBlock;
    F32 fpsMouseSensitivity = 0.3f;
    F32 speed;
    F32 playerHeight = 2.0f;
    F32 jumpPower;
    F32 playerSize = 0.5f;
    
    AudioClipPtr digClips[4];

public:
    PlayerController(F32 speed, F32 jumpPower, F32 placeDistance = 10.0f) : speed(speed), jumpPower(jumpPower), rayDistance(placeDistance) {}

    void addedToGameObject(GameObject* go) override
    {
        for(I32 i = 0; i < 4; i++)
            digClips[i] = ASSETS.getAudioClip(OS::Path("/audio/grass"+TS(i+1)+".wav"));

        ACTION_MAPPER.attachMouseEvent(ACTION_NAME_DIG, MouseKey::LButton);
        ACTION_MAPPER.attachMouseEvent(ACTION_NAME_PLACE, MouseKey::RButton);
        ACTION_MAPPER.attachKeyboardEvent(ACTION_NAME_JUMP, Key::Space);
        transform = getComponent<Components::Transform>();
        inventory = getComponent<PlayerInventory>();
        ASSERT(inventory && "Inventory Component is NULL!");
    }

    void tick(Time::Seconds delta) override
    {
        auto viewerDir = transform->rotation.getForward();
        Physics::Ray ray(transform->position, viewerDir * rayDistance);

        // Preview block
        World::Get().RayCast(ray, [](const ChunkRayCastResult& result) {
            Math::Vec3 blockSize(BLOCK_SIZE * 0.5f);
            Math::Vec3 min = result.blockCenter - blockSize;
            Math::Vec3 max = result.blockCenter + blockSize;
            DEBUG.drawCube(min, max, Color::GREEN, 0); // @TODO: Should be replaced with a new gameobject and a custom shader/material
        });

        // Remove blocks
        if (ACTION_MAPPER.wasKeyPressed(ACTION_NAME_DIG))
        {
            World::Get().RayCast(ray, [=](const ChunkRayCastResult& result) {
                World::Get().SetVoxelAt(result.blockCenter, Block::Air);
                inventory->add(result.block);
                LOG(inventory->toString(), Color::GREEN);

                digClips[Math::Random::Int(3)]->play();
                //DEBUG.drawSphere(result.hitPoint, 0.1f, Color::RED, 10);
                //DEBUG.drawRay(transform->position, ray.getDirection(), Color::BLUE, 10);
            });
        }

        // Place block
        if (ACTION_MAPPER.wasKeyPressed(ACTION_NAME_PLACE))
        {
            World::Get().RayCast(ray, [=](const ChunkRayCastResult& result) {
                Math::Vec3 hitDir = result.hitPoint - result.blockCenter;

                // Calculate where to place the new block by checking which component has the largest abs.
                Math::Vec3 axis(0, 0, hitDir.z);
                F32 xAbs = std::abs(hitDir.x);
                F32 yAbs = std::abs(hitDir.y);
                F32 zAbs = std::abs(hitDir.z);
                if (xAbs > yAbs && xAbs > zAbs)
                    axis = Math::Vec3(hitDir.x, 0, 0);
                else if (yAbs > xAbs && yAbs > zAbs)
                    axis = Math::Vec3(0, hitDir.y, 0);

                Math::Vec3 newBlockPos = result.blockCenter + axis.normalized();

                // If the block is too close dont allow to place it
                F32 distanceToPlayer = (newBlockPos - transform->position).magnitude();
                if (distanceToPlayer < 4.0f*playerSize)
                    return;

                // Set world voxel
                Block block = inventory->getCurrentBlock();
                if (block != Block::Air)
                {
                    digClips[Math::Random::Int(3)]->play();
                    World::Get().SetVoxelAt(newBlockPos, block);
                    LOG(inventory->toString(), Color::GREEN);
                }
            });
        }

        _CalculateMovement((F32)delta.value);
    }

 private:
    void _CalculateMovement(F32 delta)
    {
        static Math::Vec3 playerVelocity;

        // LOOK ROTATION
        auto deltaMouse = MOUSE.getMouseDelta();
        transform->rotation *= Math::Quat(transform->rotation.getRight(), deltaMouse.y * fpsMouseSensitivity);
        transform->rotation *= Math::Quat(Math::Vec3::UP, deltaMouse.x * fpsMouseSensitivity);

        // JUMPING
        bool isOnGround = (playerVelocity.y == 0.0f);
        if (isOnGround)
            if ( ACTION_MAPPER.wasKeyPressed( ACTION_NAME_JUMP ) )
                playerVelocity.y += Math::Vec3::UP.y * delta * jumpPower;

        // GRAVITY
        playerVelocity.y += GRAVITY * delta;

        // MOVEMENT
        F32 realSpeed = speed;
        if (KEYBOARD.isKeyDown(Key::Shift))
            realSpeed *= 2.0f;

        Math::Vec3 forward  = transform->rotation.getForward();
        Math::Vec3 left     = transform->rotation.getLeft();

        Math::Vec3 mov = left * (F32)AXIS_MAPPER.getAxisValue("Horizontal") * delta * realSpeed + 
                         forward * (F32)AXIS_MAPPER.getAxisValue("Vertical") * delta * realSpeed;
        playerVelocity.x = mov.x;
        playerVelocity.z = mov.z;

        // APPLY VELOCITY
        Math::Vec3 prevPos = transform->position;
        transform->position += playerVelocity;

        // COLLISION CHECK (X+Z AXIS) - Shoot ray from feet into movement direction
        Math::Vec3 xzVel(playerVelocity.x, 0.0f, playerVelocity.z);
        Physics::Ray rayXZ(transform->position + Math::Vec3::DOWN * playerHeight + 0.1f, xzVel.normalized() * playerSize);
        World::Get().RayCast(rayXZ, [=](const ChunkRayCastResult& result) {
            transform->position.x = prevPos.x;
            transform->position.z = prevPos.z;
            //DEBUG.drawSphere(result.hitPoint, 0.1f, Color::RED, 0, false);
        });

        // COLLISION CHECK (Y-AXIS)
        Physics::Ray rayDown(transform->position, Math::Vec3::DOWN * playerHeight);
        World::Get().RayCast(rayDown, [=](const ChunkRayCastResult& result) {
            transform->position.y = result.hitPoint.y - rayDown.getDirection().y;
            playerVelocity.y = 0.0f;
        });
    }
};

//**********************************************************************
class MyScene : public IScene
{
    Components::FPSCamera* fpsCam;
    PlayerController* playerController;
public:
    MyScene() : IScene("MyScene"){}

    void init() override
    {
        // CAMERA
        auto player = createGameObject("player");
        auto cam = player->addComponent<Components::Camera>();
        player->getComponent<Components::Transform>()->position = Math::Vec3(0, 100, -5);
        player->getComponent<Components::Transform>()->lookAt({0});
        fpsCam = player->addComponent<Components::FPSCamera>(Components::FPSCamera::FPS, 10.0f, 0.3f);
        fpsCam->setActive(false);
        //go->addComponent<Minimap>(500.0f, 50.0f);
        player->addComponent<Components::AudioListener>();
        cam->setClearColor(Color::BLUE);

        U32 blockInventorySize = 64;
        player->addComponent<PlayerInventory>(blockInventorySize);
        F32 placeDistance = 10.0f;
        F32 playerSpeed = 10.0f;
        F32 jumpPower = 15.0f;
        playerController = player->addComponent<PlayerController>(playerSpeed, jumpPower, placeDistance);

        //auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
        //    "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
        //    "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
        //player->addComponent<Components::Skybox>(cubemap);

        // GAMEOBJECTS
        I32 chunkViewDistance = 8;
        auto worldGenerator = createGameObject("World Generation")->addComponent<WorldGeneration>(chunkViewDistance);
    }

    void tick(Time::Seconds delta) override
    {
        static bool b = false;
        if (KEYBOARD.wasKeyPressed(Key::X))
        {
            playerController->setActive(b);
            fpsCam->setActive(!b);
            b = !b;
        }
    }

    void shutdown() override {}
};

class Game : public IGame
{
public:
    //----------------------------------------------------------------------
    void init() override 
    {
        LOG( "Init game..." );
        OS::VirtualFileSystem::mount("textures", "res/textures");
        OS::VirtualFileSystem::mount("shaders", "res/shaders");
        OS::VirtualFileSystem::mount("audio", "res/audio");
        gLogger->setSaveToDisk( false );

        Locator::getEngineClock().setInterval([] {
           //LOG( "Time: " + TS( Locator::getEngineClock().getTime().value ) + " FPS: " + TS( Locator::getProfiler().getFPS() ) );
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
            String newTitle = String(gameName) + " | Time: " + TS(TIME.getTime().value) + " | Delta: " + TS(delta) + "ms (" + TS(fps) + " FPS)";
            Locator::getWindow().setTitle(newTitle.c_str());
        }, 1000);

        getWindow().setCursor( "../dx/res/internal/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "../dx/res/internal/icon.ico" );

        // Create a gameobject in the default scene with a new component and add new scene onto the stack.
        // This way the music manager will stay alife during the whole program.
        SCENE.createGameObject("MusicManager")->addComponent<MusicManager>(ArrayList<OS::Path>{"/audio/minecraft.wav", "/audio/minecraft2.wav"});

        MOUSE.setFirstPersonMode(true); // Hide the mouse cursor has no effect on a separate thread, so just call it here
        Locator::getSceneManager().PushSceneAsync( new MyScene(), false );
    }

    //----------------------------------------------------------------------
    void tick(Time::Seconds delta) override
    {
        if (KEYBOARD.wasKeyPressed(Key::One))
            Locator::getSceneManager().LoadSceneAsync(new MyScene);
        if (KEYBOARD.wasKeyPressed(Key::P))
            PROFILER.logGPU();
        if (KEYBOARD.wasKeyPressed(Key::F1))
            Locator::getRenderer().setGlobalMaterialActive("NONE");
        if (KEYBOARD.wasKeyPressed(Key::F2))
            Locator::getRenderer().setGlobalMaterialActive("Wireframe");
        if (KEYBOARD.isKeyDown(Key::Escape))
            terminate();
    }

    //----------------------------------------------------------------------
    void shutdown() override 
    {
        LOG( "Shutdown game..." );
    }
};
#if DISPLAY_CONSOLE

    int main()
    {
        Game game;
        game.start( gameName, 800, 600 );

        system("pause");
        return 0;
    }

#else

    #undef ERROR
    #include <Windows.h>

    int APIENTRY WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPTSTR    lpCmdLine,
        int       nCmdShow)
    {
        Game game;
        game.start(gameName, 800, 600);

        return 0;
    }

#endif