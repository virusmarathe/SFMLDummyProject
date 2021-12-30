#include "Assets.h"
#include <fstream>
#include <iostream>

std::string FONT_KEY = "Font";
std::string TEXTURE_KEY = "Texture";

bool Assets::loadAssets(std::string filePath)
{
	std::ifstream fin(filePath);
	std::string type, name, path;
	bool status = true;

	while (fin >> type)
	{
		fin >> name >> path;

		if (type == FONT_KEY) status = addFont(name, path);
		if (status == false) return false;

		if (type == TEXTURE_KEY) status = addTexture(name, path);
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
