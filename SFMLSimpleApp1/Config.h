#pragma once

#include "SFML/Window/Keyboard.hpp"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float PADDLE_WIDTH = 50.0f;
const float PADDLE_HEIGHT = 100.0f;
const float PADDLE_SPEED = 400.0f;
const float BALL_SIZE = 20.0f;
const float BALL_START_SPEED = 200.0f;
const float SPEED_GAIN_PER_HIT = 10.0f;
const int POINTS_PER_SCORE = 10;

const sf::Keyboard::Key PLAYER_CONTROLS[2][4] = { { sf::Keyboard::Key::W, sf::Keyboard::Key::S, sf::Keyboard::Key::A, sf::Keyboard::Key::D},
										{ sf::Keyboard::Key::Up, sf::Keyboard::Key::Down, sf::Keyboard::Key::Left, sf::Keyboard::Key::Right} };