#pragma once

#include <string>

class Settings
{
public:
	static int WINDOW_WIDTH;
	static int WINDOW_HEIGHT;
	static float PADDLE_SPEED;
	static float BALL_SIZE;
	static float BALL_START_SPEED;
	static float ARCHER_SPEED;
	static float BULLET_SPEED;
	static std::string SERVER_IP;
	static unsigned short SERVER_PORT;
	static unsigned short TEST_PORT;

	static void LoadSettingsFile(std::string fileName);
};

