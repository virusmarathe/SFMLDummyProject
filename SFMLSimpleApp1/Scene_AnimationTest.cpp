#include "Scene_AnimationTest.h"
#include "Framework/GameEngine.h"
#include "Resources/Assets.h"
#include "Framework/Action.h"
#include "Config.h"

void Scene_AnimationTest::init()
{
	_assets = _engine->getAssets();

	_player1Entity = spawnPlayer();

    _engine->registerAction(sf::Keyboard::Num2, "PHYSICS_SCENE");
    _engine->registerAction(sf::Keyboard::W, "P1UP");
    _engine->registerAction(sf::Keyboard::A, "P1LEFT");
    _engine->registerAction(sf::Keyboard::S, "P1DOWN");
    _engine->registerAction(sf::Keyboard::D, "P1RIGHT");
    _engine->registerAction(sf::Keyboard::LShift, "P1RUN");

    _engine->playBGMusic("Level1BG");
}

void Scene_AnimationTest::update(float dt)
{
    sInput();

    _entities.update();

    sMovement(dt);
    sAnimation(dt);
}

void Scene_AnimationTest::sDoAction(const Action& action)
{
    if (action.name == "PHYSICS_SCENE" && action.type == Action::ActionType::START) _engine->changeScene("PhysicsTest");

    if (action.name == "P1UP") _player1Entity->getComponent<CInput>()->up = action.type == Action::ActionType::START;
    if (action.name == "P1LEFT") _player1Entity->getComponent<CInput>()->left = action.type == Action::ActionType::START;
    if (action.name == "P1DOWN") _player1Entity->getComponent<CInput>()->down = action.type == Action::ActionType::START;
    if (action.name == "P1RIGHT") _player1Entity->getComponent<CInput>()->right = action.type == Action::ActionType::START;
    if (action.name == "P1RUN") _player1Entity->getComponent<CInput>()->run = action.type == Action::ActionType::START;
}

void Scene_AnimationTest::sRender()
{
    for (auto ent : _entities.getEntities())
    {
        if (ent->hasComponent<CSprite>())
        {
            ent->getComponent<CSprite>()->sprite.setPosition(ent->getComponent<CTransform>()->position.x, ent->getComponent<CTransform>()->position.y);
            _window->draw(ent->getComponent<CSprite>()->sprite);
        }
    }
}

std::shared_ptr<Entity> Scene_AnimationTest::spawnPlayer()
{
    auto player = _entities.addEntity("Player");
    player->addComponent<CTransform>(Vector2(200,200));
    std::shared_ptr<CSprite> sprite = player->addComponent<CSprite>(_assets->getTexture("ArcherWalk"));
    sprite->sprite.setScale(0.25f, 0.25f);
    std::shared_ptr<CAnimation> anim = player->addComponent<CAnimation>("ArcherWalk");
    player->addComponent<CRectCollider>(Rect(sprite->sprite.getTextureRect()));
    player->addComponent<CPhysicsBody>();
    player->addComponent<CInput>();
    player->addComponent<CPhysicsAnimator>("ArcherIdle", "ArcherWalk", "ArcherRun", ARCHER_SPEED);

    return player;

}

void Scene_AnimationTest::sAnimation(float dt)
{
    for (auto ent : _entities.getEntities())
    {
        // physics animator system
        if (ent->hasComponent<CPhysicsAnimator>() && ent->hasComponent<CPhysicsBody>() && ent->hasComponent<CAnimation>() && ent->hasComponent<CSprite>())
        {
            std::shared_ptr<CPhysicsAnimator> physicsAnimator = ent->getComponent<CPhysicsAnimator>();
            std::shared_ptr<CPhysicsBody> body = ent->getComponent<CPhysicsBody>();

            std::string animToPlay = body->velocity.magnitudeSqr() > 0 ? physicsAnimator->movingAnimName : physicsAnimator->idleAnimName;
            if (body->velocity.magnitudeSqr() > physicsAnimator->velThreshold * physicsAnimator->velThreshold) animToPlay = physicsAnimator->fastMovingAnimName;

            if (physicsAnimator->currentAnimName != animToPlay)
            {
                physicsAnimator->currentAnimName = animToPlay;
                std::shared_ptr<CAnimation> animComp = ent->getComponent<CAnimation>();
                std::shared_ptr<CSprite> spriteComp = ent->getComponent<CSprite>();
                animComp->currentFrame = -1;
                animComp->name = animToPlay;
                spriteComp->sprite.setTexture(_assets->getAnimation(animComp->name)->texture);
            }
        }

        // animation playing system
        if (ent->hasComponent<CSprite>() && ent->hasComponent<CAnimation>())
        {
            std::shared_ptr<CAnimation> animComp = ent->getComponent<CAnimation>();
            std::shared_ptr<CSprite> spriteComp = ent->getComponent<CSprite>();
            int animCompLastFrame = animComp->currentFrame;

            if (animComp->currentFrame == -1) // first frame set to default animation
            {
                animComp->currentFrame = 0;
                std::shared_ptr<Animation> anim = _assets->getAnimation(animComp->name);
                animComp->duration = anim->duration;
                animComp->numFrames = anim->numFrames;
                animComp->frameWidth = anim->frameWidth;
                animComp->frameHeight = anim->frameHeight;
            }

            animComp->frameTimer += dt;
            if (animComp->frameTimer >= (animComp->duration / animComp->numFrames))
            {
                animComp->currentFrame++;
                if (animComp->currentFrame >= animComp->numFrames) animComp->currentFrame = 0;
                animComp->frameTimer = 0;
            }

            if (animComp->currentFrame != animCompLastFrame)
            {
                spriteComp->sprite.setTextureRect(sf::IntRect(animComp->currentFrame * animComp->frameWidth, 0, animComp->frameWidth, animComp->frameHeight));
            }
        }
    }
}

void Scene_AnimationTest::sInput()
{
    for (auto ent : _entities.getEntities("Player"))
    {
        if (ent->hasComponent<CInput>() && ent->hasComponent<CPhysicsBody>())
        {
            std::shared_ptr<CInput> input = ent->getComponent<CInput>();
            Vector2 vel;

            if (input->up)     vel.y -= ARCHER_SPEED;
            if (input->down)   vel.y += ARCHER_SPEED;
            if (input->left)   vel.x -= ARCHER_SPEED;
            if (input->right)  vel.x += ARCHER_SPEED;

            if (input->run) vel *= 2;

            ent->getComponent<CPhysicsBody>()->velocity = vel;
        }
    }
}

void Scene_AnimationTest::sMovement(float dt)
{
    for (auto ent : _entities.getEntities())
    {
        if (ent->hasComponent<CTransform>() && ent->hasComponent<CPhysicsBody>())
        {
            ent->getComponent<CTransform>()->position += ent->getComponent<CPhysicsBody>()->velocity * dt;
            if (ent->hasComponent<CRectCollider>())
            {
                ent->getComponent<CRectCollider>()->rect.pos = ent->getComponent<CTransform>()->position;
            }
        }
    }
}
