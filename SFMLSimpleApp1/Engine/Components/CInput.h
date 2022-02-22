#pragma once

#include "SFML/Graphics.hpp"
#include "Component.h"

class CInput : public Component
{
public:
	bool up;
	bool down;
	bool left;
	bool right;
	bool run;

	CInput() :up(false), down(false), left(false), right(false), run(false) {}

	friend sf::Packet& operator<< (sf::Packet& packet, const std::shared_ptr<CInput> input) { return packet << input->up << input->down << input->left << input->right << input->run; }
	friend sf::Packet& operator>> (sf::Packet& packet, std::shared_ptr<CInput> input) { return packet >> input->up >> input->down >> input->left >> input->right >> input->run; }
};