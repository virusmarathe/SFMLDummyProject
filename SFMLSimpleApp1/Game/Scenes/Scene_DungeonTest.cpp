#include "Scene_DungeonTest.h"
#include "Framework/GameEngine.h"
#include "Resources/Assets.h"
#include "Framework/Action.h"
#include "../Settings.h"
#include "System/SRender.h"
#include "System/SMovement.h"
#include "System/SAnimation.h"
#include "System/SPhysics.h"
#include <iostream>
#include "Framework/Primitives.h"
#include "System/SDamage.h"
#include "System/SHealthBar.h"

const float GRID_SIZE = 50.0f;

enum PhysicsLayer
{
    DEFAULT = 1,
    PLAYER = 2,
    ENEMY = 4,
    NEUTRAL = 8,
    ALL = (DEFAULT | PLAYER | ENEMY | NEUTRAL)
};

void Scene_DungeonTest::init()
{
    _assets = _engine->getAssets();

    _camera = _entities.addEntity("Camera");
    auto& transform = _camera.addComponent<CTransform>();
    transform.position = Vector2();
    transform.scale = Vector2(_window->getSize());
    _camera.addComponent<CPhysicsBody>();

    _engine->registerAction(sf::Keyboard::P, "PHYSICS_TOGGLE");
    _engine->registerAction(sf::Keyboard::Num2, "PHYSICS_SCENE");
    _engine->registerAction(sf::Keyboard::W, "P1UP");
    _engine->registerAction(sf::Keyboard::A, "P1LEFT");
    _engine->registerAction(sf::Keyboard::S, "P1DOWN");
    _engine->registerAction(sf::Keyboard::D, "P1RIGHT");
    _engine->registerAction(sf::Keyboard::LShift, "P1RUN");
    _engine->registerAction(sf::Keyboard::R, "RESET_ROOMS");
    _engine->registerAction(sf::Keyboard::F, "CAM_CHANGE");
    _engine->registerAction(GameEngine::MOUSE_SCROLL_UP, "ZOOM_IN");
    _engine->registerAction(GameEngine::MOUSE_SCROLL_DOWN, "ZOOM_OUT");
    _engine->registerAction(GameEngine::MOUSE_LEFT_DOWN, "FIRE");
    _engine->registerAction(GameEngine::MOUSE_MOVED, "FIRE_TARGET");

    _engine->registerSystem(std::make_shared<SPhysics>(&_entities, Priority::PHYSICS));
    _engine->registerSystem(std::make_shared<SDamage>(&_entities, Priority::PHYSICS + 1));
    _engine->registerSystem(std::make_shared<SHealthBar>(&_entities, Priority::PHYSICS + 1));
    _engine->registerSystem(std::make_shared<SMovement>(&_entities, Priority::UPDATE));
    _engine->registerSystem(std::make_shared<SAnimation>(&_entities, Priority::UPDATE, _assets));
    _engine->registerSystem(std::make_shared<SRender>(&_entities, Priority::RENDER, _window, _camera, _assets));

    _engine->playBGMusic("Level1BG");

    resetLevel(20);
}

const float HALLWAY_WIDTH_HALF = GRID_SIZE * 3;

void Scene_DungeonTest::preUpdate(float dt)
{   
    if (!_freeCam && _player1Entity.id < MAX_ENTITIES)
    {
        _camera.getComponent<CTransform>().position = _player1Entity.getComponent<CTransform>().position;
    }

    if (_firing)
    {
        _fireCooldownTimer += dt;
        if (_fireCooldownTimer >= 0.1f)
        {
            //fireBullet(_firePos);
            for (int i = 0; i < 10; i++)
            {
                fireBullet(_firePos + Vector2((float)(rand()%200), (float)(rand()%200)));
            }
            _fireCooldownTimer = 0;
        }
    }
    else _fireCooldownTimer = 0.25f;
}

void Scene_DungeonTest::sDoAction(const Action& action)
{
    if (action.name == "PHYSICS_TOGGLE" && action.type == Action::ActionType::START) GameEngine::DEBUG_MODE = !GameEngine::DEBUG_MODE;
    if (action.name == "CAM_CHANGE" && action.type == Action::ActionType::START) _freeCam = !_freeCam;
    if (action.name == "PHYSICS_SCENE" && action.type == Action::ActionType::START) _engine->changeScene("PhysicsTest");

    float scale = _camera.getComponent<CTransform>().scale.x / _window->getSize().x;

    if (_freeCam)
    {
        if (action.name == "P1UP") _camera.getComponent<CPhysicsBody>().velocity.y = action.type == Action::ActionType::START ? -200 * scale : 0;
        if (action.name == "P1LEFT") _camera.getComponent<CPhysicsBody>().velocity.x = action.type == Action::ActionType::START ? -200 * scale : 0;
        if (action.name == "P1DOWN") _camera.getComponent<CPhysicsBody>().velocity.y = action.type == Action::ActionType::START ? 200 * scale : 0;
        if (action.name == "P1RIGHT") _camera.getComponent<CPhysicsBody>().velocity.x = action.type == Action::ActionType::START ? 200 * scale : 0;
    }
    else
    {
        if (_player1Entity.id < MAX_ENTITIES)
        {
            if (action.name == "P1UP") _player1Entity.getComponent<CInput>().up = action.type == Action::ActionType::START;
            if (action.name == "P1LEFT") _player1Entity.getComponent<CInput>().left = action.type == Action::ActionType::START;
            if (action.name == "P1DOWN") _player1Entity.getComponent<CInput>().down = action.type == Action::ActionType::START;
            if (action.name == "P1RIGHT") _player1Entity.getComponent<CInput>().right = action.type == Action::ActionType::START;
            if (action.name == "P1RUN") _player1Entity.getComponent<CInput>().run = action.type == Action::ActionType::START;
            if (action.name == "FIRE") _firing = action.type == Action::ActionType::START;
            if (action.name == "FIRE_TARGET") _firePos = action.pos;
        }
    }

    if (action.name == "ZOOM_IN") _camera.getComponent<CTransform>().scale /= 1.05f;
    if (action.name == "ZOOM_OUT") _camera.getComponent<CTransform>().scale *= 1.05f;

    if (action.name == "RESET_ROOMS" && action.type == Action::ActionType::START)
    {
        for (auto ent : _entities.getEntities())
        {
            if (ent.tag() == "Room" || ent.tag() == "Wall" || ent.tag() == "Destructable")
            {
                ent.destroy();
            }
        }
        resetLevel(100);
    }

    sInput();
}

void Scene_DungeonTest::postUpdate(float dt)
{
    sHandleCollision();
}

void Scene_DungeonTest::resetLevel(int numRooms)
{
    _dungeon.generateDungeon(numRooms, GRID_SIZE);
    Vector2 startPos = _dungeon.getRooms()[0].pos;

    for (auto& room : _dungeon.getRooms())
    {
        createRoom(room, "Ground");
    }
    for (auto& wall : _dungeon.getWalls())
    {
        createWall(wall, "Wall");
    }

    _player1Entity.destroy();
    _player1Entity = spawnPlayer();
    _player1Entity.getComponent<CTransform>().position = Vector2(startPos) + Vector2(300, 300);

    for (int i = 0; i < 5; i++)
    {
        auto barrel = Primitives::ScaledSprite(Rect(_player1Entity.getComponent<CTransform>().position + Vector2((float)i * 100, 100), Vector2(100, 100)), _assets->getTexture("Barrel"), true, "Destructable");
        auto& healthComp = barrel.addComponent<CHealth>();
        healthComp.health = 100.0f;
        healthComp.maxHealth = 100.0f;
        healthComp.hitLayer = PhysicsLayer::ALL;
        healthComp.background.setSize(sf::Vector2f(100, 10));
        healthComp.background.setFillColor(sf::Color::Red);
        healthComp.foreground.setSize(sf::Vector2f(100, 10));
        healthComp.foreground.setFillColor(sf::Color::Green);
    }
}

void Scene_DungeonTest::createRoom(Rect rect, std::string assetName)
{
    Entity room = Primitives::TiledSprite(rect, GRID_SIZE * 2, _assets->getTexture(assetName), "Room");
}

void Scene_DungeonTest::createWall(Rect wallRect, std::string assetName)
{
    Entity wall = Primitives::TiledSprite(wallRect, GRID_SIZE, _assets->getTexture(assetName), "Wall");
    auto& rectCollider = wall.addComponent<CRectCollider>();
    rectCollider.rect = wallRect;
}

Entity Scene_DungeonTest::spawnPlayer()
{
    auto player = _entities.addEntity("Player");
    auto& transform = player.addComponent<CTransform>();
    transform.position = Vector2(200, 200);
    auto& sprite = player.addComponent<CSprite>();
    sprite.sprite = sf::Sprite(_assets->getTexture("ArcherWalk"));
    sprite.sprite.setScale(0.25f, 0.25f);
    auto& anim = player.addComponent<CAnimation>();
    anim.name = "ArcherWalk";
    auto& rectCollider = player.addComponent<CRectCollider>();
    rectCollider.rect = Rect(0, 0, 70, 80);
    rectCollider.offset = Vector2(20, 5);
    rectCollider.collisionLayer = PLAYER;
    auto& physics = player.addComponent<CPhysicsBody>();
    physics.collidesWith = DEFAULT | NEUTRAL | ENEMY;
    player.addComponent<CInput>();
    auto& animator = player.addComponent<CPhysicsAnimator>();
    animator.idleAnimName = "ArcherIdle";
    animator.movingAnimName = "ArcherWalk";
    animator.fastMovingAnimName = "ArcherRun";
    animator.velThreshold = Settings::ARCHER_SPEED;

    return player;
}

void Scene_DungeonTest::fireBullet(Vector2 mouseLocation)
{
    auto bullet = _entities.addEntity("Bullet");
    Rect playerRect = _player1Entity.getComponent<CRectCollider>().rect;
    Vector2 pos = _player1Entity.getComponent<CTransform>().position + (playerRect.size / 2.0f);
    auto& transform = bullet.addComponent<CTransform>();
    transform.position = pos;
    Vector2 vel = (mouseLocation - pos);
    vel.normalize();
    vel *= Settings::BULLET_SPEED;
    auto& physics = bullet.addComponent<CPhysicsBody>();
    physics.velocity = vel;
    physics.collidesWith = DEFAULT | NEUTRAL | ENEMY;
    auto& sprite = bullet.addComponent<CSprite>();
    auto& quadComp = bullet.addComponent<CQuad>();
    sf::Vector2u texSize = _assets->getTexture("Ball").getSize();
    Rect box(0, 0, 12.5, 12.5);
    quadComp.texCoords = Rect(0, 0, (float)texSize.x, (float)texSize.y);
    quadComp.rect = box;
    quadComp.textureID = _assets->getTextureIDFromName("Ball");
    auto& rectCollider = bullet.addComponent<CRectCollider>();
    rectCollider.rect = box;
    rectCollider.isTrigger = true;
    rectCollider.collisionLayer = PLAYER;
    auto& damage = bullet.addComponent<CDamage>();
    damage.damage = 10.0f;
    damage.layer = PLAYER;
}

void Scene_DungeonTest::sInput()
{
    for (auto ent : _entities.getEntities("Player"))
    {
        if (ent.hasComponent<CInput>() && ent.hasComponent<CPhysicsBody>())
        {
            auto& input = ent.getComponent<CInput>();
            Vector2 vel;

            if (input.up)     vel.y -= Settings::ARCHER_SPEED;
            if (input.down)   vel.y += Settings::ARCHER_SPEED;
            if (input.left)   vel.x -= Settings::ARCHER_SPEED;
            if (input.right)  vel.x += Settings::ARCHER_SPEED;

            if (input.run) vel *= 2;

            ent.getComponent<CPhysicsBody>().velocity = vel;
        }
    }
}

void Scene_DungeonTest::sHandleCollision()
{
    for (auto ent : _entities.getEntities("CollisionEvent"))
    {
        auto& cEvent = ent.getComponent<CCollisionEvent>();
        Entity ent1 = _entities[cEvent.ent1];
        Entity ent2 = _entities[cEvent.ent2];
        // handle ball collision with something
        if (ent1.tag() == "Bullet")
        {
            if (ent2.tag() == "Wall" || ent2.tag() == "Destructable")
            {
                float distSqr = (_player1Entity.getComponent<CTransform>().position - ent1.getComponent<CTransform>().position).magnitudeSqr();
                ent1.getComponent<CRectCollider>().enabled = false;
                ent1.destroy();
                if (distSqr < 100000.0f) _engine->playSound("WallImpact");
            }
        }
    }
}
