// SFMLSimpleApp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Config.h"
#include "Engine/Framework/GameEngine.h"
#include "Scene_PhysicsTest.h"
#include "Scene_AnimationTest.h"

int main()
{
    GameEngine engine;
    engine.init("Pong!", WINDOW_WIDTH, WINDOW_HEIGHT, "resources/resources.asset");
    engine.registerScene("PhysicsTest", std::make_shared<Scene_PhysicsTest>());
    engine.registerScene("AnimationTest", std::make_shared<Scene_AnimationTest>());
    engine.changeScene("AnimationTest");
    engine.run();

    return 0;
}
