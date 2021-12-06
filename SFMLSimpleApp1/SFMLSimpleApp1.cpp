// SFMLSimpleApp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <vector>
#include "Paddle.h"
#include "Ball.h"

bool hasCollision(const sf::Shape& shape1, const sf::Shape& shape2)
{
    bool foundCollision = false;

    sf::FloatRect shape1Rect = shape1.getGlobalBounds();
    sf::FloatRect shape2Rect = shape2.getGlobalBounds();

    if (shape1Rect.left < shape2Rect.left + shape2Rect.width && shape1Rect.left + shape1Rect.width > shape2Rect.left &&
        shape1Rect.top < shape2Rect.top + shape2Rect.height && shape1Rect.top + shape1Rect.height > shape2Rect.top) return true;

    return foundCollision;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Hello SFML!");

    Ball ball(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f), 10.0f, &window);
    Ball ball2(sf::Vector2f(300,300), 10.0f, &window);
    Paddle player1Paddle(sf::Color::Red, sf::Vector2f(window.getSize().x - 10.0f, 0), &window);
    Paddle player2Paddle(sf::Color::Blue, sf::Vector2f(0,0), &window);

    std::vector<GameObject*> gameObjects;
    gameObjects.push_back(&ball);
    gameObjects.push_back(&player1Paddle);
    gameObjects.push_back(&player2Paddle);
    gameObjects.push_back(&ball2);

    sf::Clock deltaTimer;

    sf::Font font;
    if (!font.loadFromFile("BalooBhaijaan2-VariableFont_wght.ttf"))
    {
        std::cout << "Font doesn't exist";
        return -1;
    }

    sf::Text redScoreText("Player 2: 0", font);
    redScoreText.setCharacterSize(24);
    redScoreText.setFillColor(sf::Color::Red);
    redScoreText.setPosition(sf::Vector2f(window.getSize().x/2.0f - redScoreText.getLocalBounds().width / 2.0f, 20));

    sf::Text blueScoreText("Player 1: 0", font);
    blueScoreText.setCharacterSize(24);
    blueScoreText.setFillColor(sf::Color::Blue);
    blueScoreText.setPosition(sf::Vector2f(window.getSize().x / 2.0f - blueScoreText.getLocalBounds().width / 2.0f, 60));

    int redScore = 0;
    int blueScore = 0;
    std::stringstream redScoreSS;
    std::stringstream blueScoreSS;

    while (window.isOpen())
    {
        sf::Event currEvent;

        while (window.pollEvent(currEvent))
        {
            if (currEvent.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))    player1Paddle.SetVelocity(1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))      player1Paddle.SetVelocity(-1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))       player2Paddle.SetVelocity(-1);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))       player2Paddle.SetVelocity(1);

        sf::Time deltaTime = deltaTimer.restart();

        if (hasCollision(ball.getShape(), player1Paddle.getShape()) || hasCollision(ball.getShape(), player2Paddle.getShape()))
        {
            ball.reverse();
        }

        if (hasCollision(ball2.getShape(), player1Paddle.getShape()) || hasCollision(ball2.getShape(), player2Paddle.getShape()))
        {
            ball2.reverse();
        }

        redScoreSS.clear();
        redScoreSS.str("");
        blueScoreSS.clear();
        blueScoreSS.str("");
        redScoreSS << "Player 2: " << redScore;
        blueScoreSS << "Player 1: " << blueScore;
        redScoreText.setString(redScoreSS.str());
        blueScoreText.setString(blueScoreSS.str());

        for (int i = 0; i < gameObjects.size(); i++)
        {
            gameObjects[i]->update(deltaTime.asSeconds());
        }

        window.clear();

        for (int i = 0; i < gameObjects.size(); i++) 
        {
            gameObjects[i]->render();
        }
        window.draw(redScoreText);
        window.draw(blueScoreText);

        window.display();
    }

    return 0;
}
