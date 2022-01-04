#include "Assets.h"
#include <fstream>
#include <iostream>

std::string FONT_KEY = "Font";
std::string TEXTURE_KEY = "Texture";
std::string ANIMATION_KEY = "Animation";
std::string MUSIC_KEY = "Music";
std::string SOUND_KEY = "Sound";

bool Assets::loadAssets(std::string filePath)
{
	std::ifstream fin(filePath);
	std::string type, name, path, textureName;
	int numFrames;
	float duration;
	bool status = true;

	while (fin >> type)
	{
		if (type == FONT_KEY)
		{
			fin >> name >> path;
			status = addFont(name, path);
		}
		if (status == false) return false;

		if (type == TEXTURE_KEY)
		{
			fin >> name >> path;
			status = addTexture(name, path);
		}
		if (status == false) return false;

		if (type == ANIMATION_KEY)
		{
			fin >> name >> textureName >> numFrames >> duration;
			status = addAnimation(name, textureName, numFrames, duration);
		}
		if (status == false) return false;

		if (type == MUSIC_KEY)
		{
			fin >> name >> path;
			status = addMusic(name, path);
		}
		if (status == false) return false;

		if (type == SOUND_KEY)
		{
			fin >> name >> path;
			status = addSound(name, path);
		}
		if (status == false) return false;
	}

	fin.close();

	return status;
}

bool Assets::addTexture(std::string name, std::string path)
{
	if (!_textures[name].loadFromFile(path))
	{
		std::cout << "Couldn't find image " << name << std::endl;
		return false;
	}

	return true;
}

bool Assets::addFont(std::string name, std::string path)
{
	if (!_fonts[name].loadFromFile(path))
	{
		std::cout << "Couldn't find font " << path << std::endl;
		return false;
	}

	return true;
}

bool Assets::addAnimation(std::string name, std::string textureName, int numFrames, float duration)
{
	sf::Texture& tex = getTexture(textureName);
	_animations[name] = std::make_shared<Animation>(tex, numFrames, duration);
	return true;
}

bool Assets::addMusic(std::string name, std::string path)
{
	if (!_music[name].openFromFile(path))
	{
		std::cout << "Couldn't find music " << path << std::endl;
		return false;
	}
	_music[name].setLoop(true);
	_music[name].setVolume(50);

	return true;
}

bool Assets::addSound(std::string name, std::string path)
{
	if (!_sounds[name].loadFromFile(path))
	{
		std::cout << "Couldn't find sound " << path << std::endl;
		return false;
	}
	return true;
}
