// SFMLSimpleApp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Config.h"
#include "Engine/Framework/GameEngine.h"
#include "Scene_PhysicsTest.h"
#include "Scene_DungeonTest.h"

int main()
{
    GameEngine engine;
    engine.init("Pong!", WINDOW_WIDTH, WINDOW_HEIGHT, "resources/resources.asset");
    engine.registerScene("PhysicsTest", std::make_shared<Scene_PhysicsTest>());
    engine.registerScene("DungeonTest", std::make_shared<Scene_DungeonTest>());
    engine.changeScene("DungeonTest");
    engine.run();

    return 0;
}
