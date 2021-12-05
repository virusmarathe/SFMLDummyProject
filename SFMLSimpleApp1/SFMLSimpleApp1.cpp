// SFMLSimpleApp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>

sf::Vector2f velocity(1, 1);
float speed = 100;
sf::Vector2f paddle1Vel(0, 0);
sf::Vector2f paddle2Vel(0, 0);
float paddleSpeed = 100;

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Hello SFML!");
    sf::CircleShape ball(10.0f);
    sf::RectangleShape paddle1(sf::Vector2f(10,100));
    sf::RectangleShape paddle2(sf::Vector2f(10, 100));

    ball.setFillColor(sf::Color::Green);
    paddle1.setFillColor(sf::Color::Red);
    paddle2.setFillColor(sf::Color::Blue);
    paddle2.setPosition(sf::Vector2f(window.getSize().x - paddle2.getSize().x, 0));
    sf::Clock deltaTimer;

    while (window.isOpen())
    {
        sf::Event currEvent;

        while (window.pollEvent(currEvent))
        {
            if (currEvent.type == sf::Event::Closed)
            {
                window.close();
            }

            if (currEvent.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
                {
                    paddle1Vel.y = 1;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
                {
                    paddle1Vel.y = -1;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
                {
                    paddle2Vel.y = -1;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
                {
                    paddle2Vel.y = 1;
                }
            }
            if (currEvent.type == sf::Event::KeyReleased)
            {
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
                {
                    paddle1Vel.y = 0;
                }
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
                {
                    paddle2Vel.y = 0;
                }
            }
        }

        sf::Time deltaTime = deltaTimer.restart();

        sf::Vector2f curPos = ball.getPosition();

        if (curPos.x + ball.getLocalBounds().width > window.getSize().x) 
        {
            velocity.x = -1;
            speed += 10;
        }
        else if (curPos.x < 0) 
        {
            velocity.x = 1;
            speed += 10;
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

        ball.setPosition(ball.getPosition() + velocity * speed * deltaTime.asSeconds());
        paddle1.setPosition(paddle1.getPosition() + paddle1Vel * paddleSpeed * deltaTime.asSeconds());
        paddle2.setPosition(paddle2.getPosition() + paddle2Vel * paddleSpeed * deltaTime.asSeconds());

        window.clear();
        window.draw(ball);
        window.draw(paddle1);
        window.draw(paddle2);
        window.display();
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
