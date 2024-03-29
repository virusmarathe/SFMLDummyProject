// SFMLSimpleApp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Settings.h"
#include "Framework/GameEngine.h"
#include "Scenes/Scene_PhysicsTest.h"
#include "Scenes/Scene_DungeonTest.h"
#include "Scenes/Scene_Menu.h"

int main()
{
    Settings::LoadSettingsFile("resources/config.asset");
    GameEngine engine;
    engine.init("Rat Engine Tests!", Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "resources/resources.asset");
    engine.registerScene("PhysicsTest", std::make_shared<Scene_PhysicsTest>());
    engine.registerScene("DungeonTest", std::make_shared<Scene_DungeonTest>());
    engine.registerScene("Menu", std::make_shared<Scene_Menu>());
    engine.changeScene("Menu");
    engine.run();

    return 0;
}
