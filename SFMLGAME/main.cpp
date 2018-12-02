

//#include "pch.h"
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

sf::Vector2f NormaliseVector2(sf::Vector2f vector, float speed = 1);
float MagnitudeVector2(sf::Vector2f vector);
void Reset();

sf::Vector2f vel1;
sf::Vector2f vel2;
sf::Vector2f vel3;
sf::Vector2f dir;
sf::Vector2f dir2;
int screenWidth = 1000;
int screenHeight = 600;
float MAXSPEED = 2.0f;
float acceleration = 0.025f;

sf::Font font;

sf::Texture texture;
sf::Texture ballTexture;
sf::Texture goalTexture;

sf::Text text;

sf::Sprite player1;
sf::Sprite player2;
sf::Sprite goal1;
sf::Sprite goal2;
sf::Sprite ball;

sf::Vector2f player1StartPos(200, 100);
sf::Vector2f player2StartPos(800, 100);
sf::Vector2f goal1StartPos(800, 300);
sf::Vector2f goal2StartPos(200, 300);
sf::Vector2f ballStartPos(500, 400);

int player1Score = 0;
int player2Score = 0;

//sf::CircleShape ball(100.f);
//ball.setTexture(&ballTexture,false);
//ball.setPosition(500, 400);
int main()
{
	int screenWidth = 1000;
	int screenHeight = 600;
	float MAXSPEED = 2.0f;
	float acceleration = 0.025f;
	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "SFML works!", sf::Style::Resize);
	window.setKeyRepeatEnabled(false);
	//window.setPosition(sf::Vector2i(500, 50));
	//window.setJoystickThreshold(100);

	if (!font.loadFromFile("font.ttf"))
	{
		// error...
	}

	if (!texture.loadFromFile("image.png"))
	{
		// error...
	}

	if (!ballTexture.loadFromFile("ball.png"))
	{
		// error...
	}

	if (!goalTexture.loadFromFile("goal.png"))
	{
		// error...
	}


	// select the font
	text.setFont(font); // font is a sf::Font

						// set the string to display
	text.setString("Test");

	// set the character size
	text.setCharacterSize(36); // in pixels, not points!

							   // set the color
	text.setFillColor(sf::Color::Red);

	// set the text style
	text.setStyle(sf::Text::Bold | sf::Text::Underlined);

	text.setPosition((screenWidth / 2) - (text.getString().getSize() * text.getCharacterSize()), 10);

	player1.setTexture(texture);
	player1.setPosition(player1StartPos);
	player1.setScale(sf::Vector2f(0.03f, 0.03f));
	player1.setColor(sf::Color(0, 0, 255, 250));
	player1.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));

	player2.setTexture(texture);
	player2.setPosition(player2StartPos);
	player2.setScale(sf::Vector2f(0.03f, 0.03f));
	player2.setColor(sf::Color(255, 0, 0, 250));
	player2.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));

	goal1.setTexture(goalTexture);
	goal1.setPosition(goal1StartPos);
	goal1.setScale(sf::Vector2f(0.1f, 0.1f));
	goal1.setColor(sf::Color(0, 0, 255, 250));
	goal1.setOrigin(sf::Vector2f(goalTexture.getSize().x / 2, goalTexture.getSize().y / 2));

	goal2.setTexture(goalTexture);
	goal2.setPosition(goal2StartPos);
	goal2.setScale(sf::Vector2f(0.1f, 0.1f));
	goal2.setColor(sf::Color(255, 0, 0, 250));
	goal2.setOrigin(sf::Vector2f(goalTexture.getSize().x / 2, goalTexture.getSize().y / 2));

	ball.setTexture(ballTexture);
	ball.setPosition(ballStartPos);
	ball.setScale(sf::Vector2f(0.2f, 0.2f));
	ball.setColor(sf::Color(255, 255, 255, 255));
	ball.setOrigin(sf::Vector2f(ballTexture.getSize().x / 2, ballTexture.getSize().y / 2));

	//sf::CircleShape ball(100.f);
	//ball.setTexture(&ballTexture,false);
	//ball.setPosition(500, 400);

	vel1 = sf::Vector2f(0, 0);
	vel2 = sf::Vector2f(0, 0);
	vel3 = sf::Vector2f(0, 0);
	dir = sf::Vector2f(0, 0);
	dir2 = sf::Vector2f(0, 0);

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
		int i = text.getString().getSize();
		std::string str = "Score " + std::to_string(player1Score) + " - " + std::to_string(player2Score);
		//text.setPosition((screenWidth / 2) - ((text.getString().getSize() * text.getCharacterSize()) / 2), 10);
		text.setPosition((screenWidth / 2) - 100, 10);
		
		text.setString(str);

		dir.x = sinf((3.14159 / 180) * player1.getRotation());
		dir.y = -cosf((3.14159 / 180) * player1.getRotation());

		dir2.x = sinf((3.14159 / 180) * player2.getRotation());
		dir2.y = -cosf((3.14159 / 180) * player2.getRotation());

		dir = NormaliseVector2(dir, 0.1f);
		dir2 = NormaliseVector2(dir2, 0.1f);


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			player1.rotate(-0.3f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			player1.rotate(0.3f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			if (MagnitudeVector2(vel1) < MAXSPEED)
			{
				vel1 += dir * acceleration;
			}
		}

		if (player1.getPosition().x > screenWidth)
		{
			player1.setPosition(0, player1.getPosition().y);
		}
		if (player1.getPosition().x < 0)
		{
			player1.setPosition(screenWidth, player1.getPosition().y);
		}
		if (player1.getPosition().y > screenHeight)
		{
			player1.setPosition(player1.getPosition().x, 0);
		}
		if (player1.getPosition().y < 0)
		{
			player1.setPosition(player1.getPosition().x, screenHeight);
		}

		if (player2.getPosition().x > screenWidth)
		{
			player2.setPosition(0, player2.getPosition().y);
		}
		if (player2.getPosition().x < 0)
		{
			player2.setPosition(screenWidth, player2.getPosition().y);
		}
		if (player2.getPosition().y > screenHeight)
		{
			player2.setPosition(player2.getPosition().x, 0);
		}
		if (player2.getPosition().y < 0)
		{
			player2.setPosition(player2.getPosition().x, screenHeight);
		}

		if (ball.getPosition().x > screenWidth)
		{
			vel3.x *= -1;
			if (vel3.x >= 0)
			{
				vel3.x = -0.1f;
			}
		}
		if (ball.getPosition().x < 0)
		{
			vel3.x *= -1;
			if (vel3.x <= 0)
			{
				vel3.x = 0.1f;
			}
		}
		if (ball.getPosition().y > screenHeight)
		{
			vel3.y *= -1;
			if (vel3.y >= 0)
			{
				vel3.y = -0.1f;
			}
		}
		if (ball.getPosition().y < 0)
		{
			vel3.y *= -1;
			if (vel3.y <= 0)
			{
				vel3.y = 0.1f;
			}
		}

		

		if (player1.getGlobalBounds().intersects(player2.getGlobalBounds()))
		{
			float totalSpeed = MagnitudeVector2(vel1) + MagnitudeVector2(vel2);
			sf::Vector2f dir1 = NormaliseVector2(player1.getPosition() - player2.getPosition());
			//sf::Vector2f dir2 = NormaliseVector2(player2.getPosition()) - NormaliseVector2(player1.getPosition());
			sf::Vector2f dir2 = -dir1;
			vel1 = dir1 * (totalSpeed / 2);
			vel2 = dir2 * (totalSpeed / 2);

			//vel2 = sf::Vector2f(0.0f, 1.0f);
		}
		else if (player1.getGlobalBounds().intersects(ball.getGlobalBounds()))
		{
			float totalSpeed = MagnitudeVector2(vel1) + MagnitudeVector2(vel3);
			sf::Vector2f dir1 = NormaliseVector2(player1.getPosition() - ball.getPosition());
			sf::Vector2f dir3 = -dir1;
			sf::Vector2f newVel1 = dir1 * (totalSpeed / 4);

			vel1.x = (vel1.x + newVel1.x) / 2;
			vel1.y = (vel1.y + newVel1.y) / 2;

			vel3 = dir3 * (totalSpeed * 3 / 4);

			if (MagnitudeVector2(vel3) == 0)
			{
				vel3 = dir3 / 5.0f;
			}
		}
		else if (player2.getGlobalBounds().intersects(ball.getGlobalBounds()))
		{
			float totalSpeed = MagnitudeVector2(vel2) + MagnitudeVector2(vel3);
			sf::Vector2f dir2 = NormaliseVector2(player2.getPosition() - ball.getPosition());
			sf::Vector2f dir3 = -dir2;
			sf::Vector2f newVel2 = dir2 * (totalSpeed / 4);

			vel2.x = (vel2.x + newVel2.x) / 2;
			vel2.y = (vel2.y + newVel2.y) / 2;

			vel3 = dir3 * (totalSpeed * 3 / 4);

			
			if (MagnitudeVector2(vel3) == 0)
			{
				vel3 = dir3 / 5.0f;
			}
		}


		
		if (MagnitudeVector2(vel1) > MAXSPEED)
		{
			vel1 = NormaliseVector2(vel1, MAXSPEED);
		}
		else if (MagnitudeVector2(vel1) > 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			vel1 /= 1.0005f;
		}
		if (MagnitudeVector2(vel2) > MAXSPEED)
		{
			vel2 = NormaliseVector2(vel2, MAXSPEED);
		}
		else if (MagnitudeVector2(vel2) > 0)
		{
			vel2 /= 1.0005f;
		}
		if (MagnitudeVector2(vel3) > MAXSPEED)
		{
			vel3 = NormaliseVector2(vel3, MAXSPEED);
		}
		else if (MagnitudeVector2(vel3) > 0)
		{
			vel3 /= 1.0003f;
		}

		player1.move(vel1);
		player2.move(vel2);
		ball.move(vel3);

		std::cout << " ball speed " << vel1.x << " , " << vel1.y << std::endl;

		if (ball.getGlobalBounds().intersects(goal1.getGlobalBounds()))
		{
			player1Score++;
			Reset();
		}
		else if (ball.getGlobalBounds().intersects(goal2.getGlobalBounds()))
		{
			player2Score++;
			Reset();
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
		window.draw(goal1);
		window.draw(goal2);
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

float MagnitudeVector2(sf::Vector2f vector)
{
	return sqrt((vector.x * vector.x) + (vector.y * vector.y));
}

void Reset()
{
	player1.setPosition(player1StartPos);
	player2.setPosition(player2StartPos);

	goal1.setPosition(goal1StartPos);
	goal2.setPosition(goal2StartPos);

	ball.setPosition(ballStartPos);

	vel1 = sf::Vector2f(0, 0);
	vel2 = sf::Vector2f(0, 0);
	vel3 = sf::Vector2f(0, 0);
	dir = sf::Vector2f(0, 0);
	dir2 = sf::Vector2f(0, 0);

}
