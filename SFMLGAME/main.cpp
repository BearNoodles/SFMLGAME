

//#include "pch.h"
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>

#include <list>
#include <vector>
#include <string>

#include "Player.h"
#include "Message.h"

#define MAXPLAYERS 8
#define FPS 60

void InitHost();
void InitClient();
void Init();
void HostOrClient();
bool WaitForPlayers();
void OutOfBounds();
sf::Vector2f NormaliseVector2(sf::Vector2f vector, float speed = 1);
float MagnitudeVector2(sf::Vector2f vector);
void Reset();

sf::Vector2f ballVelocity;
int screenWidth = 1000;
int screenHeight = 600;
float MAXSPEED = 2.0f;
float acceleration = 0.025f;

sf::Font font;

sf::Texture texture;
sf::Texture ballTexture;
sf::Texture goalTexture;

sf::Text text;

Player myPlayer;
Player opponent;
std::list<Player> playerList;
int playerCount;
int myID;
sf::Color* playerColours;
int startPosX = 100;
int startPosY = 100;

sf::Time currentTime;
sf::Time frameTime;

sf::Sprite goal1;
sf::Sprite goal2;
sf::Sprite ball;

sf::Vector2f goal1StartPos(800, 300);
sf::Vector2f goal2StartPos(200, 300);
sf::Vector2f ballStartPos(500, 400);

int player1Score = 0;
int player2Score = 0;

std::list<Message> messages;

sf::UdpSocket socket;
sf::IpAddress hostIP = "127.0.0.1";
unsigned short hostPort = 54444;

sf::IpAddress clientIP;
unsigned short clientPort;

int main()
{
	socket.setBlocking(false);
	playerColours = new sf::Color[MAXPLAYERS]{  sf::Color(255, 0, 0, 255),
												sf::Color(0, 0, 255, 255),
												sf::Color(255, 0, 0, 255),
												sf::Color(255, 0, 0, 255),
												sf::Color(255, 0, 0, 255),
												sf::Color(255, 0, 0, 255),
												sf::Color(255, 0, 0, 255),
												sf::Color(255, 0, 0, 255) };
	

	if (!font.loadFromFile("font.ttf"))
	{
		// error...
	}

	if (!texture.loadFromFile("image.png"))
	{
		// error...
		std::cout << "error loading image.png" << std::endl;
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

	ballVelocity = sf::Vector2f(0, 0);

	sf::Clock timerClock; // starts the clock
	sf::Clock frameClock; // starts the clock


	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "SFML works!", sf::Style::Resize);
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(FPS);
	//window.setPosition(sf::Vector2i(500, 50));
	//window.setJoystickThreshold(100);

	//Setup or join a game
	HostOrClient();
	


	//Wait for players to join and for host to start game
	while (true)
	{
		if (WaitForPlayers())
		{
			break;
		}
		window.clear();

		for (auto player : playerList)
		{
			std::cout << "x " << player.GetSprite().getPosition().x << " y " << player.GetSprite().getPosition().y << std::endl;
			window.draw(player.GetSprite());
		}
		window.draw(opponent.GetSprite());
		window.draw(myPlayer.GetSprite());
		window.draw(goal1);
		window.draw(goal2);
		window.draw(ball);
		window.draw(text);
		window.display();
	}

	/*for (auto player : playerList)
	{
		if (myID == player.GetID())
		{
			myPlayer = &player;
		}
	}*/
	int currentPlayer = 0;
	for (auto player : playerList)
	{
		currentPlayer++;
		sf::Vector2f startPos(currentPlayer * startPosX, startPosY);
		player.Init(texture, startPos, playerColours[currentPlayer], currentPlayer);
	}


	while (window.isOpen())
	{
		frameTime = frameClock.restart();

		currentTime = timerClock.getElapsedTime();

		//send current position to opponent(s)
		if (myID == 1)
		{
			Message msg;
			msg.id = myID;
			msg.x = myPlayer.GetPosition().x;
			msg.y = myPlayer.GetPosition().y;
			msg.timeSent = currentTime.asSeconds();
			sf::Packet packet;
			packet << msg.id << msg.x << msg.y << msg.timeSent;

			if (socket.send(packet, clientIP, clientPort) != sf::Socket::Done)
			{
				// error...
				//send failed try it again
				std::cout << "Send message failed" << std::endl;
			}
		}
		else 
		{
			Message msg;
			msg.id = myID;
			msg.x = myPlayer.GetPosition().x;
			msg.y = myPlayer.GetPosition().y;
			msg.timeSent = currentTime.asSeconds();
			sf::Packet packet;
			packet << msg.id << msg.x << msg.y << msg.timeSent;

			if (socket.send(packet, hostIP, hostPort) != sf::Socket::Done)
			{
				// error...
				//send failed try it again
				std::cout << "Send message failed" << std::endl;
			}
		}

		//check for messages received
		if (myID == 1)
		{
			sf::Int8 id;
			float x;
			float y;
			float time;
			sf::Packet packet;
			if (socket.receive(packet, clientIP, clientPort) == sf::Socket::NotReady)
			{
				// error...
				//recieve failed send hello again
				//std::cout << "no messages yet" << std::endl;
			}
			packet >> id >> x >> y >> time;
			Message msg;
			msg.id = id;
			msg.x = x;
			msg.y = y;
			msg.timeSent = time;

			messages.push_back({ id, x, y, time });
		}
		else
		{
			sf::Int8 id;
			float x;
			float y;
			float time;
			sf::Packet packet;
			if (socket.receive(packet, hostIP, hostPort) == sf::Socket::NotReady)
			{
				// error...
				//recieve failed send hello again
				//std::cout << "no messages yet" << std::endl;
			}
			packet >> id >> x >> y >> time;
			Message msg;
			msg.id = id;
			msg.x = x;
			msg.y = y;
			msg.y = y;

			messages.push_back({ id, x, y, time });
		}

		//give messages to appropriate players
		while (!messages.empty())
		{
			if (messages.front().id == opponent.GetID())
			{
				opponent.AddMessage(messages.front());
				messages.pop_front();
			}
			else
			{
				messages.pop_front();
			}
		}

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
		}

		int i = text.getString().getSize();
		std::string str = "Score " + std::to_string(player1Score) + " - " + std::to_string(player2Score);
		//text.setPosition((screenWidth / 2) - ((text.getString().getSize() * text.getCharacterSize()) / 2), 10);
		text.setPosition((screenWidth / 2) - 100, 10);
		
		text.setString(str);
		
		for (auto player1 : playerList)
		{
			for (auto player2 : playerList)
			{
				if (player1.GetID() == player2.GetID())
				{
					continue;
				}
				if (player1.GetSprite().getGlobalBounds().intersects(player2.GetSprite().getGlobalBounds()))
				{
					float totalSpeed = MagnitudeVector2(player1.GetVelocity()) + MagnitudeVector2(player2.GetVelocity());
					sf::Vector2f dir1 = NormaliseVector2(player1.GetSprite().getPosition() - player2.GetSprite().getPosition());
					//sf::Vector2f dir2 = NormaliseVector2(player2.getPosition()) - NormaliseVector2(player1.getPosition());
					sf::Vector2f dir2 = -dir1;
					player1.SetVelocity(dir1 * (totalSpeed / 2));
					player2.SetVelocity(dir2 * (totalSpeed / 2));

					//vel2 = sf::Vector2f(0.0f, 1.0f);
				}
			}

			//check if any player collided with ball
			if (player1.GetSprite().getGlobalBounds().intersects(ball.getGlobalBounds()))
			{
				float totalSpeed = MagnitudeVector2(player1.GetVelocity()) + MagnitudeVector2(ballVelocity);
				sf::Vector2f pdir = NormaliseVector2(player1.GetSprite().getPosition() - ball.getPosition());
				sf::Vector2f bdir = -pdir;
				sf::Vector2f newVel = pdir * (totalSpeed / 4);

				player1.SetVelocity(sf::Vector2f((player1.GetVelocity().x + newVel.x) / 2, (player1.GetVelocity().y + newVel.y) / 2));
				ballVelocity = bdir * (totalSpeed * 3 / 4);

				if (MagnitudeVector2(ballVelocity) == 0)
				{
					ballVelocity = bdir / 5.0f;
				}
			}
		}
		
		if (MagnitudeVector2(ballVelocity) > MAXSPEED)
		{
			ballVelocity = NormaliseVector2(ballVelocity, MAXSPEED);
		}
		else if (MagnitudeVector2(ballVelocity) > 0)
		{
			ballVelocity /= 1.0003f;
		}

		ball.move(ballVelocity);


		if (ball.getGlobalBounds().intersects(goal1.getGlobalBounds()))
		{
			player1Score++;
			//Reset();
		}
		else if (ball.getGlobalBounds().intersects(goal2.getGlobalBounds()))
		{
			player2Score++;
			//Reset();
		}

		myPlayer.UpdateSelf(currentTime, frameTime);
		opponent.UpdateOther(currentTime, frameTime);

		for (auto player : playerList)
		{
			if (player.GetID() == myID)
			{
				player.UpdateSelf(currentTime, frameTime);
			}
			else
			{
				player.UpdateOther(currentTime, frameTime);
			}
		}

		OutOfBounds();

		
		window.clear();
		for (auto player : playerList)
		{ 
			std::cout << "x " << player.GetSprite().getPosition().x << " y " << player.GetSprite().getPosition().y << std::endl;
			window.draw(player.GetSprite());
		}

		window.draw(opponent.GetSprite());
		window.draw(myPlayer.GetSprite());
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

void InitHost()
{
	// bind the socket to a port
	if (socket.bind(hostPort) != sf::Socket::Done)
	{
		// error...
	}

	myID = 1;
	playerCount = myID;
	sf::Vector2f startPos(playerCount * startPosX, startPosY);

	myPlayer.Init(texture, startPos, playerColours[playerCount], playerCount);
	//Player tempPlayer(texture, startPos, playerColours[playerCount], playerCount);
	//playerList.push_back(Player());
}

void InitClient()
{
	// bind the socket to a port
	if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
	{
		// error...
	}

	while (true)
	{
		std::string s = "hello";

		sf::Packet packet;
		packet << s;

		if (socket.send(packet, hostIP, hostPort) != sf::Socket::Done)
		{
			// error...
			//send failed try it again
			std::cout << "Send greeting failed, retrying" << std::endl;
			continue;
		}

		packet.clear();
		break;
	}
	while (true)
	{
		std::string r;
		sf::Packet packet;
		if (socket.receive(packet, hostIP, hostPort) == sf::Socket::NotReady)
		{
			// error...
			//receive failed send hello again
			std::cout << "Receive failed, retrying" << std::endl;
			continue;
		}

		packet >> r;

		if (std::stoi(r) > 1)
		{
			myID = std::stoi(r);
			break;
		}
		else
		{
			std::cout << "Didnt receive number > 1, retrying" << std::endl;
		}
	}

	//myID = std::stoi(r);
	playerCount = myID;

	for (int i = 0; i < playerCount; i++)
	{
		sf::Vector2f startPos(playerCount * startPosX, startPosY);
		//Player tempPlayer(texture, startPos, playerColours[playerCount], playerCount);
		//playerList.push_back(tempPlayer);

	}

	sf::Vector2f startPos(playerCount * startPosX, startPosY);
	myPlayer.Init(texture, startPos, playerColours[playerCount], playerCount);
	startPos = sf::Vector2f((playerCount - 1) * startPosX, startPosY);
	opponent.Init(texture, startPos, playerColours[playerCount - 1], playerCount - 1);

}

void Init()
{

}

void Reset()
{
	//player1.setPosition(player1StartPos);
	//player2.setPosition(player2StartPos);
	//
	//goal1.setPosition(goal1StartPos);
	//goal2.setPosition(goal2StartPos);
	//
	//ball.setPosition(ballStartPos);
	//
	//vel1 = sf::Vector2f(0, 0);
	//vel2 = sf::Vector2f(0, 0);
	//ballVelocity = sf::Vector2f(0, 0);
	//dir = sf::Vector2f(0, 0);
	//dir2 = sf::Vector2f(0, 0);
}

void OutOfBounds()
{
	//check who is out of bounds
		//for (auto player : playerList)
		//{
		//	if (player.GetSprite.GetPosition().x > screenWidth)
		//	{
		//		player.GetSprite.SetPosition(0.0f, player.GetSprite.GetPosition().y);
		//	}
		//}

	//only check if self is out of bounds
	if (myPlayer.GetPosition().x > screenWidth)
	{
		myPlayer.SetPosition(sf::Vector2f(0, myPlayer.GetPosition().y));
	}
	if (myPlayer.GetPosition().x < 0)
	{
		myPlayer.SetPosition(sf::Vector2f(screenWidth, myPlayer.GetPosition().y));
	}

	if (myPlayer.GetPosition().y > screenHeight)
	{
		myPlayer.SetPosition(sf::Vector2f(myPlayer.GetPosition().x, 0));
	}
	if (myPlayer.GetPosition().y < 0)
	{
		myPlayer.SetPosition(sf::Vector2f(myPlayer.GetPosition().y, screenHeight));
	}

	//check if ball touched edges //Maybe only for host?
	if (ball.getPosition().x > screenWidth)
	{
		ballVelocity.x *= -1;
		if (ballVelocity.x >= 0)
		{
			ballVelocity.x = -0.1f;
		}
	}
	if (ball.getPosition().x < 0)
	{
		ballVelocity.x *= -1;
		if (ballVelocity.x <= 0)
		{
			ballVelocity.x = 0.1f;
		}
	}
	if (ball.getPosition().y > screenHeight)
	{
		ballVelocity.y *= -1;
		if (ballVelocity.y >= 0)
		{
			ballVelocity.y = -0.1f;
		}
	}
	if (ball.getPosition().y < 0)
	{
		ballVelocity.y *= -1;
		if (ballVelocity.y <= 0)
		{
			ballVelocity.y = 0.1f;
		}
	}
}

void HostOrClient()
{
	while (true)
	{
		std::string choice;
		std::cout << "Type 1 to start a new game or enter IP of host (press 2 for default host)" << std::endl;
		std::cin >> choice;

		if (choice == "1")
		{
			InitHost();
			break;
		}
		else if (choice == "2")
		{
			InitClient();
			break;
		}
	}
}

bool WaitForPlayers()
{
	
	//TODO draw other players while waiting
	if (myID == 1)
	{
		//start game if host presses space
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			std::string s = "begin";
			sf::Packet packet;
			packet << s;

			if (socket.send(packet, clientIP, clientPort) != sf::Socket::Done)
			{
				// error...
				//send failed try it again
				std::cout << "Send begin failed, try again" << std::endl;
				return false;
			}
			return true;
		}
		sf::IpAddress senderIP;
		unsigned short senderPort;
		sf::Packet packet;
		std::string greeting;

		if (socket.receive(packet, senderIP, senderPort) == sf::Socket::NotReady)
		{
			// error...
			//recieve failed send hello again
			//std::cout << "no messages yet" << std::endl;
			return false;
		}

		if (packet >> greeting)
		{
			//good
		}
		else
		{
			std::cout << "couldnt read packet" << std::endl;
			return false;
		}

		if (greeting == "hello")
		{
			clientIP = senderIP;
			clientPort = senderPort;
			packet.clear();

			std::string s = std::to_string(playerCount + 1);
			packet << s;
			while (true)
			{
				if (socket.send(packet, clientIP, clientPort) != sf::Socket::Done)
				{
					// error...
					//recieve failed send hello again
					std::cout << "playerCount send failed, rety" << std::endl;
					continue;
				}
				break;
			}
			playerCount++;
			sf::Vector2f startPos(startPosX * playerCount, startPosY);
			//Player tempPlayer(texture, startPos, playerColours[playerCount], playerCount);
			//playerList.push_back(tempPlayer);

			opponent.Init(texture, startPos, playerColours[playerCount], playerCount);
		}
	}

	else
	{
		sf::IpAddress hostIP;
		unsigned short hostPort;
		sf::Packet packet;
		std::string beginMessage;
		if (socket.receive(packet, hostIP, hostPort) == sf::Socket::NotReady)
		{
			// error...
			//recieve failed send hello again
			//std::cout << "no messages yet" << std::endl;
			return false;
		}

		packet >> beginMessage;
		if (beginMessage == "begin")
		{
			return true;
		}
	}
	return false;
}
