#pragma once
#include "SFML/Graphics.hpp"
#include <string>
#include <map>
#include <Animation/Animation.h>

class Assets
{
public:
	bool loadAssets(std::string filePath);

	sf::Texture& getTexture(std::string name) { return _textures[name]; }
	sf::Font& getFont(std::string name) { return _fonts[name]; }
	std::shared_ptr<Animation> getAnimation(std::string name) { return _animations[name]; }

private:

	bool addTexture(std::string name, std::string path);
	bool addFont(std::string name, std::string path);
	bool addAnimation(std::string name, std::string textureName, int numFrames, float duration);

	std::map<std::string, sf::Texture> _textures;
	std::map<std::string, sf::Font> _fonts;
	std::map<std::string, std::shared_ptr<Animation>> _animations;
};

