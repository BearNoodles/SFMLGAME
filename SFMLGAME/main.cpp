

//#include "pch.h"
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

sf::Vector2f NormaliseVector2(sf::Vector2f vector, float speed);

int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 600), "SFML works!", sf::Style::Resize);
	window.setKeyRepeatEnabled(false);
	//window.setPosition(sf::Vector2i(500, 50));
	//window.setJoystickThreshold(100);

	sf::Font font;
	if (!font.loadFromFile("font.ttf"))
	{
		// error...
	}

	sf::Texture texture;
	if (!texture.loadFromFile("image.png"))
	{
		// error...
	}

	sf::Text text;

	// select the font
	text.setFont(font); // font is a sf::Font

						// set the string to display
	text.setString("Test");

	// set the character size
	text.setCharacterSize(24); // in pixels, not points!

							   // set the color
	text.setFillColor(sf::Color::Red);

	// set the text style
	text.setStyle(sf::Text::Bold | sf::Text::Underlined);

	text.setPosition(100, 100);

	sf::Sprite player1;
	player1.setTexture(texture);
	player1.setPosition(200, 100);
	player1.setScale(sf::Vector2f(0.02f, 0.02f));
	player1.setColor(sf::Color(0, 0, 255, 250));
	player1.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));

	sf::Sprite player2;
	player2.setTexture(texture);
	player2.setPosition(500, 100);
	player2.setScale(sf::Vector2f(0.02f, 0.02f));
	player2.setColor(sf::Color(255, 0, 0, 250));
	player2.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));

	sf::CircleShape ball(100.f);
	ball.setFillColor(sf::Color::Green);
	ball.setPosition(500, 400);

	sf::Vector2f vel1;
	sf::Vector2f vel2;
	sf::Vector2f vel3;
	sf::Vector2f dir;

	vel1 = sf::Vector2f(0, 0);
	vel2 = sf::Vector2f(0, 0);
	vel3 = sf::Vector2f(0, 0);
	dir = sf::Vector2f(0, 0);

	sf::Clock clock; // starts the clock

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::Resized)
			{
				std::cout << "new width: " << event.size.width << std::endl;
				std::cout << "new height: " << event.size.height << std::endl;
			}
			//if (event.type == sf::Event::TextEntered)
			//{
			//	if (event.text.unicode < 128)
			//		std::cout << "ASCII character typed: " << static_cast<char>(event.text.unicode) << std::endl;
			//}
			//if (event.type == sf::Event::KeyPressed)
			//{
			//	if (event.key.code == sf::Keyboard::Escape)
			//	{
			//		std::cout << "the escape key was pressed" << std::endl;
			//		std::cout << "control:" << event.key.control << std::endl;
			//		std::cout << "alt:" << event.key.alt << std::endl;
			//		std::cout << "shift:" << event.key.shift << std::endl;
			//		std::cout << "system:" << event.key.system << std::endl;
			//	}
			//}
			//if (event.type == sf::Event::MouseWheelScrolled)
			//{
			//	if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
			//		std::cout << "wheel type: vertical" << std::endl;
			//	else if (event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
			//		std::cout << "wheel type: horizontal" << std::endl;
			//	else
			//		std::cout << "wheel type: unknown" << std::endl;
			//	std::cout << "wheel movement: " << event.mouseWheelScroll.delta << std::endl;
			//	std::cout << "mouse x: " << event.mouseWheelScroll.x << std::endl;
			//	std::cout << "mouse y: " << event.mouseWheelScroll.y << std::endl;
			//}
			//if (event.type == sf::Event::MouseButtonPressed)
			//{
			//	if (event.mouseButton.button == sf::Mouse::Right)
			//	{
			//		std::cout << "the right button was pressed" << std::endl;
			//		std::cout << "mouse x: " << event.mouseButton.x << std::endl;
			//		std::cout << "mouse y: " << event.mouseButton.y << std::endl;
			//	}
			//}
			//if (event.type == sf::Event::MouseMoved)
			//{
			//	std::cout << "new mouse x: " << event.mouseMove.x << std::endl;
			//	std::cout << "new mouse y: " << event.mouseMove.y << std::endl;
			//}
			//if (event.type == sf::Event::MouseEntered)
			//	std::cout << "the mouse cursor has entered the window" << std::endl;
			//
			//if (event.type == sf::Event::MouseLeft)
			//	std::cout << "the mouse cursor has left the window" << std::endl;
			//if (event.type == sf::Event::JoystickButtonPressed)
			//{
			//	std::cout << "joystick button pressed!" << std::endl;
			//	std::cout << "joystick id: " << event.joystickButton.joystickId << std::endl;
			//	std::cout << "button: " << event.joystickButton.button << std::endl;
			//}
			//if (event.type == sf::Event::JoystickMoved)
			//{
			//	if (event.joystickMove.axis == sf::Joystick::X)
			//	{
			//		std::cout << "X axis moved!" << std::endl;
			//		std::cout << "joystick id: " << event.joystickMove.joystickId << std::endl;
			//		std::cout << "new position: " << event.joystickMove.position << std::endl;
			//	}
			//}
			//if (event.type == sf::Event::JoystickConnected)
			//	std::cout << "joystick connected: " << event.joystickConnect.joystickId << std::endl;
			//
			//if (event.type == sf::Event::JoystickDisconnected)
			//std::cout << "joystick disconnected: " << event.joystickConnect.joystickId << std::endl;
		}

		dir.x = sinf((3.14159 / 180) * player1.getRotation());
		dir.y = -cosf((3.14159 / 180) * player1.getRotation());

		dir = NormaliseVector2(dir, 0.1f);
		std::cout << "dir is (" << dir.x << ", " << dir.y << ")\n";

		player1.move(vel1);
		player2.move(vel2);
		ball.move(vel3);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			player1.rotate(-0.3f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			player1.rotate(0.3f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			vel1 = dir;
		}

		if (player1.getGlobalBounds().intersects(player2.getGlobalBounds()))
		{
			vel2 = sf::Vector2f(0.0f, 1.0f);
		}



		// get the global mouse position (relative to the desktop)
		sf::Vector2i globalPosition = sf::Mouse::getPosition();

		// get the local mouse position (relative to a window)
		sf::Vector2i localPosition = sf::Mouse::getPosition(window); // window is a sf::Window
																	 //sf::Time elapsed1 = clock.getElapsedTime();
																	 //std::cout << elapsed1.asSeconds() << std::endl;
																	 //clock.restart();
																	 //
																	 //sf::Time elapsed2 = clock.getElapsedTime();
																	 //std::cout << elapsed2.asSeconds() << std::endl;

		window.clear();
		window.draw(player1);
		window.draw(player2);
		window.draw(ball);
		window.draw(text);
		window.display();
	}



	return 0;
}

sf::Vector2f NormaliseVector2(sf::Vector2f vector, float speed)
{
	float length = sqrt((vector.x * vector.x) + (vector.y * vector.y));
	vector.x = (vector.x / length) * speed;
	vector.y = (vector.y / length) * speed;

	return vector;
}
