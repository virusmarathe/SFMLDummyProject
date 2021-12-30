#pragma once
#include "SFML/Graphics.hpp"
#include <string>
#include <map>

class Assets
{
public:
	bool loadAssets(std::string filePath);

	sf::Texture& getTexture(std::string name) { return _textures[name]; }
	sf::Font& getFont(std::string name) { return _fonts[name]; }

private:

	bool addTexture(std::string name, std::string path);
	bool addFont(std::string name, std::string path);

	std::map<std::string, sf::Texture> _textures;
	std::map<std::string, sf::Font> _fonts;
};

