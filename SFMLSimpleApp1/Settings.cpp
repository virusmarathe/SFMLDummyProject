#include "Settings.h"
#include <fstream>
#include <iostream>

int Settings::WINDOW_WIDTH = 1280;
int Settings::WINDOW_HEIGHT = 720;
float Settings::PADDLE_SPEED = 400.0f;
float Settings::BALL_SIZE = 50.0f;
float Settings::BALL_START_SPEED = 200.0f;
float Settings::ARCHER_SPEED = 200.0f;
float Settings::BULLET_SPEED = 600.0f;
std::string Settings::SERVER_IP = "127.0.0.1";
unsigned short Settings::SERVER_PORT = 6000;

#define LoadSetting(val) if (name == #val) { fin >> val; }

void Settings::LoadSettingsFile(std::string fileName)
{
	std::ifstream fin(fileName);
	std::string name;

	while (fin >> name)
	{
		LoadSetting(WINDOW_WIDTH);
		LoadSetting(WINDOW_HEIGHT);
		LoadSetting(PADDLE_SPEED);
		LoadSetting(BALL_SIZE);
		LoadSetting(BALL_START_SPEED);
		LoadSetting(ARCHER_SPEED);
		LoadSetting(BULLET_SPEED);
		LoadSetting(SERVER_IP);
		LoadSetting(SERVER_PORT);
	}
}
