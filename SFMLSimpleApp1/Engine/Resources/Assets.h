#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
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
	sf::Music& getMusic(std::string name) { return _music[name]; }
	sf::SoundBuffer& getSound(std::string name) { return _sounds[name]; }
	unsigned char getTextureIDFromName(std::string name) { return _textureIDs[name]; }
	sf::Texture& getTextureFromID(unsigned char id) { return _textures[_textureNames[id]]; }

private:

	bool addTexture(std::string name, std::string path, int repeated);
	bool addFont(std::string name, std::string path);
	bool addAnimation(std::string name, std::string textureName, int numFrames, float duration);
	bool addMusic(std::string name, std::string path);
	bool addSound(std::string name, std::string path);

	std::map<std::string, sf::Texture> _textures;
	std::map<std::string, sf::Font> _fonts;
	std::map<std::string, std::shared_ptr<Animation>> _animations;
	std::map<std::string, sf::Music> _music;
	std::map<std::string, sf::SoundBuffer> _sounds;
	std::map<std::string, unsigned char> _textureIDs;
	std::map<unsigned char, std::string> _textureNames;

	int textureCounter = 0;
};

