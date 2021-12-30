// SFMLSimpleApp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Config.h"
#include "Engine/Framework/GameEngine.h"

int main()
{
    GameEngine engine;
    engine.init("Pong!", WINDOW_WIDTH, WINDOW_HEIGHT, "resources/resources.asset");
    engine.run();

    return 0;
}
