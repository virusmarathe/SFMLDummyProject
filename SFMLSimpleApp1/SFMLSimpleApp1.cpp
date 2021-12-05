// SFMLSimpleApp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>

sf::Vector2f velocity(1, 1);
float speed = 200;
sf::Vector2f paddle1Vel(0, 0);
sf::Vector2f paddle2Vel(0, 0);
float paddleSpeed = 400;

std::string intToString(int integer)
{
    char numstr[10]; // enough to hold all numbers up to 32-bits
    sprintf_s(numstr, "%i", integer);
    return numstr;
}

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
    sf::CircleShape ball(10.0f);
    sf::RectangleShape paddle1(sf::Vector2f(10,100));
    sf::RectangleShape paddle2(sf::Vector2f(10, 100));

    ball.setFillColor(sf::Color::Green);
    paddle1.setFillColor(sf::Color::Red);
    paddle2.setFillColor(sf::Color::Blue);
    paddle1.setPosition(sf::Vector2f(window.getSize().x - paddle2.getSize().x, 0));
    sf::Clock deltaTimer;

    sf::Font font;
    if (!font.loadFromFile("BalooBhaijaan2-VariableFont_wght.ttf"))
    {
        std::cout << "Font doesn't exist";
        return -1;
    }

    sf::Text redScoreText("Red: 0", font);
    redScoreText.setCharacterSize(24);
    redScoreText.setFillColor(sf::Color::Red);
    redScoreText.setPosition(sf::Vector2f(window.getSize().x/2.0f - redScoreText.getGlobalBounds().width / 2.0f, 20));

    sf::Text blueScoreText("Blue: 0", font);
    blueScoreText.setCharacterSize(24);
    blueScoreText.setFillColor(sf::Color::Blue);
    blueScoreText.setPosition(sf::Vector2f(window.getSize().x / 2.0f - blueScoreText.getGlobalBounds().width / 2.0f, 60));

    int redScore = 0;
    int blueScore = 0;

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

        paddle1Vel.y = 0;
        paddle2Vel.y = 0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        {
            paddle1Vel.y += 1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        {
            paddle1Vel.y -= 1;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            paddle2Vel.y -= 1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            paddle2Vel.y += 1;
        }

        sf::Time deltaTime = deltaTimer.restart();

        sf::Vector2f curPos = ball.getPosition();

        if (curPos.x + ball.getLocalBounds().width > window.getSize().x) 
        {
            ball.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));
            blueScore++;
            speed = 200;
        }
        else if (curPos.x < 0) 
        {
            ball.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));
            redScore++;
            speed = 200;
        }
        if (curPos.y + ball.getLocalBounds().height > window.getSize().y)
        {
            velocity.y = -1;
            speed += 10;
        }
        else if (curPos.y < 0)
        {
            velocity.y = 1;
            speed += 10;
        }

        if (hasCollision(ball, paddle1))
        {
            velocity.x = -1;
        }
        else if (hasCollision(ball, paddle2))
        {
            velocity.x = 1;
        }

        redScoreText.setString("Score: " + intToString(redScore));
        blueScoreText.setString("Score: " + intToString(blueScore));

        ball.setPosition(ball.getPosition() + velocity * speed * deltaTime.asSeconds());
        paddle1.setPosition(paddle1.getPosition() + paddle1Vel * paddleSpeed * deltaTime.asSeconds());
        paddle2.setPosition(paddle2.getPosition() + paddle2Vel * paddleSpeed * deltaTime.asSeconds());

        if (paddle1.getPosition().y < 0)
        {
            paddle1.setPosition(sf::Vector2f(paddle1.getPosition().x, 0));
        }
        if (paddle1.getPosition().y + paddle1.getLocalBounds().height > window.getSize().y)
        {
            paddle1.setPosition(sf::Vector2f(paddle1.getPosition().x, window.getSize().y - paddle1.getLocalBounds().height));
        }
        if (paddle2.getPosition().y < 0)
        {
            paddle2.setPosition(sf::Vector2f(paddle2.getPosition().x, 0));
        }
        if (paddle2.getPosition().y + paddle2.getLocalBounds().height > window.getSize().y)
        {
            paddle2.setPosition(sf::Vector2f(paddle2.getPosition().x, window.getSize().y - paddle2.getLocalBounds().height));
        }

        window.clear();
        window.draw(ball);
        window.draw(paddle1);
        window.draw(paddle2);
        window.draw(redScoreText);
        window.draw(blueScoreText);
        window.display();
    }

    return 0;
}
