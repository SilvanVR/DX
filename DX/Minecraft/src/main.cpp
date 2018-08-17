/**********************************************************************    
    author: S. Hau
    date: April, 2018

    This is a fairly simple minecraft clone.
    Be aware that the code was written fast, so i might did some things
    which are not very efficient and the code looks not very clean.

    Note that the physics are faked with a couple raycasts and therefore
    arent very accurate (besides there is a little stuttering of the player
    while new chunks are generated, because raycasts against the world must be done after the next generation
    of an chunk has been finished. This causes the player to fall for a moment).
**********************************************************************/

#define USE_VR                  1
#define DISPLAY_CONSOLE         1
#define DEBUG_HUD               1
#define CHUNKS_VIEW_DISTANCE    16

#include "World/world_generator.h"
#include "World/Terrain Generator/flat_terrain_generator.h"
#include "World/Terrain Generator/basic_terrain_generator.h"

#ifdef _DEBUG
    const char* gameName = "[DEBUG] Minecraft";
#else
    const char* gameName = "[RELEASE] Minecraft";
#endif

#define ACTION_NAME_DIG     "dig"
#define ACTION_NAME_PLACE   "place"
#define ACTION_NAME_JUMP    "jump"
#define ACTION_NAME_RUN     "run"

static const F32 GRAVITY = -0.7f;

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
        cam->setClearMode(Graphics::CameraClearMode::None);
        cam->setCameraMode(Graphics::CameraMode::Orthographic);
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
            volume = Math::Clamp( volume, 0.0f, 1.0f );

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
    bool    m_previewBlockEnabled = true;

    GameObject* previewBlock;
    MaterialPtr previewBlockMaterial;
    Events::EventListener m_hmdFocusGainedListener;
    Events::EventListener m_hmdFocusLostListener;

public:
    PlayerInventory(U32 maxCountPerItem = 64) : m_maxCountPerItem(maxCountPerItem) {}

    void addedToGameObject(GameObject* go) override
    {
        previewBlock = go->getScene()->createGameObject("PreviewBlock");

        // Create material / shader
        auto shader = ASSETS.getShader("/shaders/previewBlock.shader");

        previewBlockMaterial = RESOURCES.createMaterial(shader);
        auto renderer = previewBlock->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreateCubeUV(0.15f), previewBlockMaterial);

        // Attach block to camera
        auto transform = previewBlock->getTransform();
        transform->setParent( go->getTransform(), false );

#if USE_VR
        m_hmdFocusGainedListener =  Events::EventDispatcher::GetEvent(EVENT_HMD_FOCUS_GAINED).addListener([this] { setPreviewBlockEnabled(true); });
        m_hmdFocusLostListener = Events::EventDispatcher::GetEvent(EVENT_HMD_FOCUS_LOST).addListener([this] { setPreviewBlockEnabled(false); });
        previewBlock->addComponent<Components::VRTouch>( Graphics::VR::Hand::Right );
        shader->setDepthStencilState({ true, true });
#else
        // Adjust transform
        transform->rotation = Math::Quat::FromEulerAngles( 45.0f, 45.0f, 0.0f );
        transform->position = { 0.35f, -0.3f, 1.0f };

        renderer->setCastShadows(false);
#endif
        // Disable at start
        previewBlock->setActive( false );
    }

#if USE_VR
    void tick(Time::Seconds d) override
    {
        if(CONTROLLER.wasKeyPressed(ControllerKey::X))
            _PreviousBlock();
        if(CONTROLLER.wasKeyPressed(ControllerKey::Y))
            _NextBlock();
    }
#endif

    void OnMouseWheel(I16 delta) override
    {
        // Change slot per mouse wheel
        delta > 0 ? _NextBlock() : _PreviousBlock();
        LOG(toString(), Color::GREEN);
    }

    GameObject* getPreviewBlock() { return previewBlock; }

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
            _UpdatePreviewBlock();
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
            m_slots[m_inventory[block].slot] = AIR_BLOCK;
            m_inventory.erase(block);
            _UpdatePreviewBlock();
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
            return AIR_BLOCK;

        return block;
    }

    String toString()
    {
        String result = "\n<<<< Inventory >>>>";
        if (m_inventory.empty()) return result + "\nEmpty!";
        for (auto& pair : m_inventory)
            result += "\n" + BlockDatabase::Get().getBlockName(pair.first.getMaterial()) + ": " + TS(pair.second.count) + " ["+ TS(pair.second.slot)+ "]";
        result += "\nCurrent Slot: " + TS(m_curSlot) + "(" + BlockDatabase::Get().getBlockName(m_slots[m_curSlot].getMaterial()) + ")";
        return result;
    }

    void setPreviewBlockEnabled(bool enabled){ m_previewBlockEnabled = enabled; _UpdatePreviewBlock(); }

private:
    I32 _NextFreeSlot() 
    {
        for (I32 i = 0; i < SLOT_COUNT; i++)
            if (m_slots[i] == AIR_BLOCK)
                return i;
        return -1;
    }

    void _NextBlock()
    {
        m_curSlot = (m_curSlot + 1) % SLOT_COUNT;
        _UpdatePreviewBlock();
    }

    void _PreviousBlock()
    {
        m_curSlot -= 1;
        m_curSlot = m_curSlot < 0 ? 0 : m_curSlot;
        _UpdatePreviewBlock();
    }

    void _UpdatePreviewBlock()
    {
        Block block = m_slots[m_curSlot];
        if (block != AIR_BLOCK && m_previewBlockEnabled)
        {
            previewBlock->setActive(true);

            auto tex = ASSETS.getTexture2D(BlockDatabase::Get().getBlockInfo(block.getMaterial()).topBottom);
            tex->setAnisoLevel(1);
            tex->setFilter(Graphics::TextureFilter::Point);
       
            previewBlockMaterial->setTexture("tex", tex);
        }
        else
        {
            previewBlock->setActive(false);
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
    F32 mousePitchDeg = 0;
    F32 mouseYawDeg = 0;
    AudioClipPtr digClips[4];
    Components::VRCamera* vrCam;

public:
    PlayerController(F32 speed, F32 jumpPower, F32 placeDistance = 10.0f) : speed(speed), jumpPower(jumpPower), rayDistance(placeDistance) {}

    void addedToGameObject(GameObject* go) override
    {
        for(I32 i = 0; i < 4; i++)
            digClips[i] = ASSETS.getAudioClip(OS::Path("/audio/grass"+TS(i+1)+".wav"));

        ACTION_MAPPER.attachMouseEvent(ACTION_NAME_DIG, MouseKey::LButton);
        ACTION_MAPPER.attachMouseEvent(ACTION_NAME_PLACE, MouseKey::RButton);
        ACTION_MAPPER.attachKeyboardEvent(ACTION_NAME_JUMP, Key::Space);
        ACTION_MAPPER.attachKeyboardEvent(ACTION_NAME_RUN, Key::Shift);

#if USE_VR
        vrCam = getGameObject()->getComponent<Components::VRCamera>();
        ACTION_MAPPER.attachControllerEvent(ACTION_NAME_DIG, ControllerKey::RIndexTrigger);
        ACTION_MAPPER.attachControllerEvent(ACTION_NAME_PLACE, ControllerKey::RHandTrigger);
        ACTION_MAPPER.attachControllerEvent(ACTION_NAME_JUMP, ControllerKey::A);
        ACTION_MAPPER.attachControllerEvent(ACTION_NAME_RUN, ControllerKey::LIndexTrigger);
        playerHeight += 1.0f;
#endif
        transform = getGameObject()->getComponent<Components::Transform>();
        inventory = getGameObject()->getComponent<PlayerInventory>();
        ASSERT(inventory && "Inventory Component is NULL!");

        auto eulers = transform->rotation.toEulerAngles();
        mousePitchDeg = eulers.x;
        mouseYawDeg = eulers.y;
    }

    void onActive() override
    {
        auto eulers = transform->rotation.toEulerAngles();
        mousePitchDeg = eulers.x;
        mouseYawDeg = eulers.y;
    }

    void tick(Time::Seconds delta) override
    {
#if USE_VR
        auto inv = getGameObject()->getComponent<PlayerInventory>();
        auto prevBlockTransform = inv->getPreviewBlock()->getTransform();
        auto dir = prevBlockTransform->getWorldRotation().getForward();
        auto pos = prevBlockTransform->getWorldPosition();
#else
        auto dir = transform->rotation.getForward();
        auto pos = transform->position;
#endif
        Physics::Ray ray(pos, dir * rayDistance);

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
                World::Get().SetVoxelAt(result.blockCenter, AIR_BLOCK);
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
                if (block != AIR_BLOCK)
                {
                    digClips[Math::Random::Int(3)]->play();
                    World::Get().SetVoxelAt(newBlockPos, block);
                    LOG(inventory->toString(), Color::GREEN);
                }
            });
        }

#if USE_VR
        _CalculateMovementVR((F32)delta.value);
#else
        _CalculateMovement((F32)delta.value);
#endif
    }

 private:
    void _CalculateMovement(F32 delta)
    {
        static Math::Vec3 playerVelocity;
        static F32 mouseSensitivity = 0.15f;

        // LOOK ROTATION
        auto deltaMouse = MOUSE.getMouseDelta();
        mousePitchDeg += deltaMouse.y * mouseSensitivity;
        mouseYawDeg += deltaMouse.x * mouseSensitivity;

        // Smoothly lerp to desired rotation
        Math::Quat desiredRotation = Math::Quat::FromEulerAngles(mousePitchDeg, mouseYawDeg, 0.0f);
        transform->rotation = Math::Quat::Slerp(transform->rotation, desiredRotation, 0.3f);

        // JUMPING
        bool isOnGround = (playerVelocity.y == 0.0f);
        if (isOnGround)
            if ( ACTION_MAPPER.wasKeyPressed( ACTION_NAME_JUMP ) )
                playerVelocity.y += Math::Vec3::UP.y * delta * jumpPower;

        // GRAVITY
        playerVelocity.y += GRAVITY * delta;

        // MOVEMENT
        F32 realSpeed = speed;
        if (ACTION_MAPPER.isKeyDown(ACTION_NAME_RUN))
            realSpeed *= 2.0f;

        Math::Vec3 forward  = transform->rotation.getForward();
        Math::Vec3 left     = transform->rotation.getLeft();

        Math::Vec3 mov = left * (F32)AXIS_MAPPER.getAxisValue("Horizontal") * realSpeed + 
                         forward * (F32)AXIS_MAPPER.getAxisValue("Vertical") * realSpeed;
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

    void _CalculateMovementVR(F32 delta)
    {
        static Math::Vec3 playerVelocity;

        // JUMPING
        bool isOnGround = (playerVelocity.y == 0.0f);
        if (isOnGround)
            if (ACTION_MAPPER.wasKeyPressed(ACTION_NAME_JUMP))
                playerVelocity.y += Math::Vec3::UP.y * delta * jumpPower;

        // GRAVITY
        playerVelocity.y += GRAVITY * delta;

        // MOVEMENT
        F32 realSpeed = speed;
        if (ACTION_MAPPER.isKeyDown(ACTION_NAME_RUN))
            realSpeed *= 2.0f;

        // ROTATE WITH RIGHT THUMBSTICK
        auto rightThumb = CONTROLLER.getThumbstick(Core::Input::ESide::Right);
        static bool rotated = false;
        if (rightThumb.x > -0.5f && rightThumb.x < 0.5f)
            rotated = false;
        else if (rightThumb.x > 0.5f && not rotated)
        {
            transform->rotation *= Math::Quat({ 0, 1, 0 }, 20.0f);
            rotated = true;
        }
        else if (rightThumb.x < -0.5f && not rotated)
        {
            transform->rotation *= Math::Quat({ 0, 1, 0 }, -20.0f);
            rotated = true;
        }

        auto playerRot = vrCam->getHeadTransform()->getWorldRotation();

        // MOVE WITH LEFT THUBMSTICK
        auto axis = CONTROLLER.getThumbstick(Core::Input::ESide::Left);
        Math::Vec3 mov = playerRot.getRight() * axis.x * realSpeed + playerRot.getForward() * axis.y * realSpeed;
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
class Water : public Components::IComponent
{
    F32 m_waterLevel;
    Components::Transform* transform;

public:
    Water(F32 waterLevel) : m_waterLevel(waterLevel){}

    void addedToGameObject(GameObject* go) override
    {
        auto mat = RESOURCES.createMaterial( ASSETS.getShader("/shaders/water.shader") );
        mat->setTexture("tex", ASSETS.getTexture2D("/textures/water.jpg", false));
        mat->setFloat("opacity", 0.8f);

        auto water = go->getScene()->createGameObject();
        auto mr = water->addComponent<Components::MeshRenderer>(Core::MeshGenerator::CreatePlane(1000.0f), mat);
        mr->setCastShadows(false);
        transform = water->getTransform();
        transform->rotation *= Math::Quat(Math::Vec3::RIGHT, 90);
    }

    void tick(Time::Seconds delta) override
    {
        // Plane moves with the viewer in the X-Z plane.
        auto viewer = SCENE.getMainCamera()->getGameObject()->getTransform();
        transform->position = viewer->position;

        transform->position.y = m_waterLevel + sinf((F32)TIME.getTime().value) * 0.2f;
    }
};

//**********************************************************************
class Sun : public Components::IComponent
{
    Components::DirectionalLight* dirLight;
public:
    void addedToGameObject(GameObject* go) override
    {
#if USE_VR
        //dirLight = go->addComponent<Components::DirectionalLight>(0.75f, Color::WHITE, Graphics::ShadowType::None);
#else
#endif
        ArrayList<F32> splitRanges{5.0f, 20.0f, 70.0f, 200.0f};
        dirLight = go->addComponent<Components::DirectionalLight>(0.75f, Color::WHITE, Graphics::ShadowType::CSMSoft, splitRanges);
        dirLight->setShadowMapQuality(Graphics::ShadowMapQuality::Insane);
        dirLight->setShadowRange(50.0f);

        go->getTransform()->rotation = Math::Quat::LookRotation( Math::Vec3{ 0, -1, 0 }, Math::Vec3::RIGHT );
    }

    Components::DirectionalLight* getDirLight() { return dirLight; }

    void tick(Time::Seconds delta) override
    {
        F32 speed = 1.0f;
        if (KEYBOARD.isKeyDown(Key::E))
            getGameObject()->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, speed * (F32)delta);
        if (KEYBOARD.isKeyDown(Key::Q))
            getGameObject()->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, -speed * (F32)delta);
    }
};

//**********************************************************************
class MyScene : public IScene
{
    Components::FPSCamera*   fpsCam;
    Components::VRFPSCamera* vrFpsCam;
    PlayerController*        playerController;
    PlayerInventory*         playerInventory;
    Components::SpotLight*   lamp;

public:
    MyScene() : IScene("MyScene"){}

    void init() override
    {
        // Sun
        auto sun = createGameObject("Sun");
        sun->addComponent<Sun>();

        // PLAYER
        auto player = createGameObject("player");
        player->getComponent<Components::Transform>()->position = Math::Vec3(0, 100, -5);

#if USE_VR
        player->addComponent<Components::VRCamera>(Components::ScreenDisplay::LeftEye, Graphics::MSAASamples::Four);
        vrFpsCam = player->addComponent<Components::VRFPSCamera>(Components::VRFPSCamera::Mode::Fixed, 10.0f);
        vrFpsCam->setActive(false);
#else
        F64 acceleration = 4.0;
        F64 damping = 10.0;
        AXIS_MAPPER.updateAxis("Vertical", acceleration, damping);
        AXIS_MAPPER.updateAxis("Horizontal", acceleration, damping);

        player->addComponent<Components::Camera>(45.0f, 0.1f, 500.0f, Graphics::MSAASamples::Eight);
        player->getComponent<Components::Transform>()->lookAt({0});
        fpsCam = player->addComponent<Components::FPSCamera>(Components::FPSCamera::FPS);
        fpsCam->setActive(false);
        player->addComponent<Components::AudioListener>();
        lamp = player->addComponent<Components::SpotLight>(2.0f, Color::WHITE, 25.0f, 20.0f, false);
        lamp->setActive(false);

#if DEBUG_HUD
        player->addComponent<Components::GUI>();
        player->addComponent<Components::GUICustom>([=] {
            static F32 ambient = 0.4f;
            ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
            Locator::getRenderer().setGlobalFloat(SID("_Ambient"), ambient);

            if (ImGui::TreeNode("Directional Light"))
            {
                auto dl = sun->getComponent<Sun>()->getDirLight();

                static F32 animateSpeed = 0.0f;
                ImGui::SliderFloat("Speed", &animateSpeed, -20.0f, 20.0f);
                dl->getGameObject()->getTransform()->rotation *= Math::Quat(Math::Vec3::RIGHT, animateSpeed * (F32)PROFILER.getDelta());

                static Math::Vec3 deg{ 45.0f, 0.0f, 0.0f };
                if (ImGui::SliderFloat2("Rotation", &deg.x, 0.0f, 360.0f))
                    sun->getTransform()->rotation = Math::Quat::FromEulerAngles(deg);

                static F32 color[4] = { 1,1,1,1 };
                if (ImGui::ColorEdit4("Color", color))
                    dl->setColor(Color(color));

                CString type[] = { "None", "Hard", "Soft", "CSM", "CSMSoft" };
                static I32 type_current = 1;
                type_current = (I32)dl->getShadowType();
                if (ImGui::Combo("Shadow Type", &type_current, type, 5))
                    dl->setShadowType((Graphics::ShadowType)(type_current));

                CString qualities[] = { "Low", "Medium", "High", "Insane" };
                static I32 quality_current = 2;
                quality_current = (I32)dl->getShadowMapQuality();
                if (ImGui::Combo("Quality", &quality_current, qualities, 4))
                    dl->setShadowMapQuality((Graphics::ShadowMapQuality)(quality_current));

                static F32 size = 200.0f;
                ImGui::SliderFloat("ShadowMap Display Size", &size, 100.0f, 1000.0f);

                if (type_current > 0 && type_current <= 2)
                {
                    static F32 dlRange;
                    dlRange = dl->getShadowRange();
                    if (ImGui::SliderFloat("Shadow Range", &dlRange, 5.0f, 100.0f))
                        dl->setShadowRange(dlRange);
                    ImGui::Image(dl->getShadowMap(), 0, Math::Vec2{ size, size });
                }
                else if (type_current > 2)
                {
                    ImGui::Text("Shadow-maps");
                    for (I32 i = 0; i < dl->getCascadeCount(); ++i)
                        ImGui::Image(dl->getShadowMap(), i, Math::Vec2{ size, size });
                }
                ImGui::TreePop();
            }
        });
#endif
#endif
        U32 blockInventorySize = 64;
        playerInventory = player->addComponent<PlayerInventory>(blockInventorySize);

        F32 placeDistance = 10.0f;
        F32 playerSpeed = 0.2f;
        F32 jumpPower = 15.0f;
        playerController = player->addComponent<PlayerController>(playerSpeed, jumpPower, placeDistance);

        // WORLD
        {
            auto world = createGameObject("World");

            auto cubemap = ASSETS.getCubemap("/cubemaps/tropical_sunny_day/Left.png", "/cubemaps/tropical_sunny_day/Right.png",
                "/cubemaps/tropical_sunny_day/Up.png", "/cubemaps/tropical_sunny_day/Down.png",
                "/cubemaps/tropical_sunny_day/Front.png", "/cubemaps/tropical_sunny_day/Back.png");
            world->addComponent<Components::Skybox>(cubemap);

            I32 seed = Math::Random::Int(0,285092);
            LOG("Current World Seed: " + TS(seed), Color::RED);
            auto generator = std::make_shared<BasicTerrainGenerator>(seed);

            //auto generator = std::make_shared<FlatTerrainGenerator>();
            auto worldGenerator = world->addComponent<WorldGeneration>(player->getTransform(), generator, CHUNKS_VIEW_DISTANCE);
            world->addComponent<Water>(WATER_LEVEL);
        }
    }

    void tick(Time::Seconds delta) override
    {
        static bool b = false;
#if USE_VR
        if (CONTROLLER.wasKeyPressed(ControllerKey::Enter))
        {
            playerController->setActive(b);
            vrFpsCam->setActive(!b);
            b = !b;
        }
#else
        if (KEYBOARD.wasKeyPressed(Key::X))
        {
            playerInventory->setPreviewBlockEnabled(b);
            MOUSE.setFirstPersonMode(b);
            playerController->setActive(b);
            fpsCam->setActive(!b);
            b = !b;
        }
#endif
        if(KEYBOARD.wasKeyPressed(Key::F))
            lamp->setActive(!lamp->isActive());
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
        gLogger->setSaveToDisk( false );

        Locator::getEngineClock().setInterval([] {
            U32 fps = PROFILER.getFPS();
            F64 delta = (1000.0 / fps);
            String newTitle = String(gameName) + " | Time: " + TS(TIME.getTime().value) + " | Delta: " + TS(delta) + "ms (" + TS(fps) + " FPS)";
            Locator::getWindow().setTitle(newTitle.c_str());
        }, 1000);

        getWindow().setCursor( "/cursors/Areo Cursor Red.cur" );
        getWindow().setIcon( "/engine/icon.ico" );

        // Enable asset reloading
        ASSETS.setHotReloading(true);

        Locator::getRenderer().setGlobalFloat(SID("_Ambient"), 0.1f);

        // Create a gameobject in the default scene with a new component and add new scene onto the stack.
        // This way the music manager will stay alife during the whole program.
        SCENE.createGameObject("MusicManager")->addComponent<MusicManager>(ArrayList<OS::Path>{"/audio/minecraft.wav", "/audio/minecraft2.wav"});

#if USE_VR 
#else
        MOUSE.setFirstPersonMode(true); // Hide the mouse cursor has no effect on a separate thread, so just call it here
#endif
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