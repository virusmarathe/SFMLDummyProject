#pragma once
#include "System.h"
#include "Animation/Animation.h"
#include "Resources/Assets.h"

class SAnimation : public System
{
public:

    SAnimation(EntityManager* entityManagerRef, int priority, std::shared_ptr<Assets> assets) : _assets(assets), System(entityManagerRef, priority) { }

	virtual void update(float dt) override
	{
        for (auto ent : _entities->getEntities())
        {
            // physics animator system
            if (ent->hasComponent<CPhysicsAnimator>() && ent->hasComponent<CPhysicsBody>() && ent->hasComponent<CAnimation>() && ent->hasComponent<CSprite>())
            {
                std::shared_ptr<CPhysicsAnimator> physicsAnimator = ent->getComponent<CPhysicsAnimator>();
                std::shared_ptr<CPhysicsBody> body = ent->getComponent<CPhysicsBody>();
                std::shared_ptr<CAnimation> animComp = ent->getComponent<CAnimation>();
                std::shared_ptr<CSprite> spriteComp = ent->getComponent<CSprite>();

                std::string animToPlay = body->velocity.magnitudeSqr() > 0 ? physicsAnimator->movingAnimName : physicsAnimator->idleAnimName;
                if (body->velocity.magnitudeSqr() > physicsAnimator->velThreshold * physicsAnimator->velThreshold) animToPlay = physicsAnimator->fastMovingAnimName;

                if (physicsAnimator->currentAnimName != animToPlay)
                {
                    physicsAnimator->currentAnimName = animToPlay;
                    animComp->currentFrame = -1;
                    animComp->name = animToPlay;
                    spriteComp->sprite.setTexture(_assets->getAnimation(animComp->name)->texture);
                }

                if (body->velocity.x > 0 && !physicsAnimator->isMovingRight)
                {
                    animComp->flipX = false;
                    physicsAnimator->isMovingRight = true;
                }
                else if (body->velocity.x < 0 && physicsAnimator->isMovingRight)
                {
                    animComp->flipX = true;
                    physicsAnimator->isMovingRight = false;
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
                    if (animComp->flipX) spriteComp->sprite.setTextureRect(sf::IntRect((animComp->currentFrame + 1) * animComp->frameWidth, 0, -animComp->frameWidth, animComp->frameHeight));
                    else spriteComp->sprite.setTextureRect(sf::IntRect(animComp->currentFrame * animComp->frameWidth, 0, animComp->frameWidth, animComp->frameHeight));
                }
            }
        }
	}

private:
    std::shared_ptr<Assets> _assets;
};