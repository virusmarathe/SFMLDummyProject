#include "Scene_AnimationTest.h"
#include "Framework/GameEngine.h"
#include "Resources/Assets.h"
#include "Framework/Action.h"

void Scene_AnimationTest::init()
{
	_assets = _engine->getAssets();

	_player1Entity = spawnPlayer();

    _engine->registerAction(sf::Keyboard::Num2, "PHYSICS_SCENE");
}

void Scene_AnimationTest::update(float dt)
{
    _entities.update();

    sAnimation(dt);
}

void Scene_AnimationTest::sDoAction(const Action& action)
{
    if (action.name == "PHYSICS_SCENE" && action.type == Action::ActionType::START) _engine->changeScene("PhysicsTest");
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
    std::shared_ptr<CAnimation> anim = player->addComponent<CAnimation>("ArcherWalk");
    player->addComponent<CRectCollider>(Rect(sprite->sprite.getTextureRect()));
    player->addComponent<CPhysicsBody>();
    player->addComponent<CInput>();

    return player;

}

void Scene_AnimationTest::sAnimation(float dt)
{
    for (auto ent : _entities.getEntities())
    {
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
