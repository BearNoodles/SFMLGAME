

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
#include "Ball.h"
#include "Message.h"

#define MAXPLAYERS 8
#define FPS 60

void InitHost();
void InitClient();
void Init();
void HostOrClient();
bool WaitForPlayers();
void OutOfBounds();
void CheckCollisions();
void Goal(int score1, int score2);
sf::Vector2f NormaliseVector2(sf::Vector2f vector, float speed = 1);
float MagnitudeVector2(sf::Vector2f vector);
void Reset();


int screenWidth = 1000;
int screenHeight = 600;
float MAXSPEED = 2.0f;
//float acceleration = 0.025f;

sf::Font font;

sf::Texture texture;
sf::Texture ballTexture;
sf::Texture goalTexture;
sf::Text text;

Player myPlayer;
Player opponent;
Ball ball;
std::list<Player> playerList;
int playerCount;
int myID;
int ballID;
sf::Color* playerColours;
int startPosX = 300;
int startPosY = 100;

sf::Time currentTime;
sf::Time frameTime;
sf::Time messageTime;
bool msgReady;

sf::Sprite goal1;
sf::Sprite goal2;

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
	playerColours = new sf::Color[MAXPLAYERS]{ sf::Color(255, 0, 0, 255),
		sf::Color(0, 0, 255, 255),
		sf::Color(255, 0, 0, 255),
		sf::Color(255, 0, 0, 255),
		sf::Color(255, 0, 0, 255),
		sf::Color(255, 0, 0, 255),
		sf::Color(255, 0, 0, 255),
		sf::Color(255, 0, 0, 255) };

	sf::Color ballColour(255, 255, 255);


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

	//ball.setTexture(ballTexture);
	//ball.setPosition(ballStartPos);
	//ball.setScale(sf::Vector2f(0.2f, 0.2f));
	//ball.setColor(sf::Color(255, 255, 255, 255));
	//ball.setOrigin(sf::Vector2f(ballTexture.getSize().x / 2, ballTexture.getSize().y / 2));
	ballID = 0;

	//sf::CircleShape ball(100.f);
	//ball.setTexture(&ballTexture,false);
	//ball.setPosition(500, 400);


	sf::Clock timerClock; // starts the clock
	sf::Clock frameClock; // starts the clock
	sf::Clock messageClock; // starts the clock


	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "SFML works!");
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(FPS);


	ball.Init(ballTexture, ballStartPos, ballColour, screenWidth, screenHeight, ballID);

	//window.setPosition(sf::Vector2i(500, 50));
	//window.setJoystickThreshold(100);
	while (window.isOpen())
	{
		window.clear();
		window.draw(myPlayer.GetSprite());
		window.draw(goal1);
		window.draw(goal2);
		window.draw(ball.GetSprite());
		window.draw(text);
		window.display();
		//Setup or join a game
		HostOrClient();
		break;
	}


	//Wait for players to join and for host to start game
	while (true)
	{
		if (WaitForPlayers())
		{
			break;
		}
		window.clear();

		if (playerCount > 1)
		{
			window.draw(opponent.GetSprite());
		}
		window.draw(myPlayer.GetSprite());
		window.draw(goal1);
		window.draw(goal2);
		window.draw(ball.GetSprite());
		window.draw(text);
		window.display();

		//for (auto player : playerList)
		//{
		//	std::cout << "x " << player.GetSprite().getPosition().x << " y " << player.GetSprite().getPosition().y << std::endl;
		//	window.draw(player.GetSprite());
		//}
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

	msgReady = false;
	while (window.isOpen())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
		{
			Reset();
		}

		frameTime = frameClock.restart();

		currentTime = timerClock.getElapsedTime();

		messageTime = messageClock.getElapsedTime();

		if (messageTime.asSeconds() > 0.01f)
		{
			messageClock.restart();
			msgReady = true;
		}

		//send current position to opponent(s)
		if (myID == 1 && msgReady == true)
		{
			Message msg;
			sf::Int8 id = myID;
			float x = myPlayer.GetPosition().x;
			float y = myPlayer.GetPosition().y;
			float dx = myPlayer.GetDir().x;
			float dy = myPlayer.GetDir().y;
			float timeSent = currentTime.asSeconds();
			sf::Packet packet;
			packet << id << x << y << dx << dy << timeSent;

			if (socket.send(packet, clientIP, clientPort) != sf::Socket::Done)
			{
				// error...
				//send failed try it again
				std::cout << "Send message failed" << std::endl;
			}

			Message msg2;
			sf::Int8 id2 = ballID;
			float x2 = ball.GetPosition().x;
			float y2 = ball.GetPosition().y;
			float dx2 = 0;
			float dy2 = 0;
			float timeSent2 = currentTime.asSeconds();
			sf::Packet packet2;
			packet2 << id2 << x2 << y2 << dx2 << dy2 << timeSent2;

			if (socket.send(packet2, clientIP, clientPort) != sf::Socket::Done)
			{
				// error...
				//send failed try it again
				std::cout << "Send message failed" << std::endl;
			}
		}
		else if (msgReady == true)
		{
			Message msg;
			sf::Int8 id = myID;
			float x = myPlayer.GetPosition().x;
			float y = myPlayer.GetPosition().y;
			float dx = myPlayer.GetDir().x;
			float dy = myPlayer.GetDir().y;
			float timeSent = currentTime.asSeconds();
			sf::Packet packet;
			packet << id << x << y << dx << dy << timeSent;

			if (socket.send(packet, hostIP, hostPort) != sf::Socket::Done)
			{
				// error...
				//send failed try it again
				std::cout << "Send message failed" << std::endl;
			}
		}

		bool check = true;;
		//check for messages received
		if (myID == 1)
		{
			while (true)
			{
				sf::Int8 id;
				float x;
				float y;
				float dx;
				float dy;
				float timeSent;
				sf::Packet packet;
				sf::IpAddress address;
				unsigned short port;
				if (socket.receive(packet, address, port) != sf::Socket::Done)
				{
					// error...
					//recieve failed send hello again
					//std::cout << "no messages yet" << std::endl;
					check = false;
					break;
				}
				if (address != clientIP || port != clientPort)
				{
					check == false;
				}
				if ((packet >> id >> x >> y >> dx >> dy >> timeSent) && check)
				{
					//good
					Message msg;
					msg.id = id;
					msg.x = x;
					msg.y = y;
					msg.dirx = dx;
					msg.diry = dy;
					msg.timeSent = timeSent;

					messages.push_back(msg);

				}
				else if (check)
				{
					std::cout << "Couldnt receive message" << std::endl;
				}
			}
		}
		else
		{
			while (true)
			{
				//Recieve player position
				sf::Int8 id;
				float x;
				float y;
				float dx;
				float dy;
				float timeSent;
				sf::Packet packet;
				sf::IpAddress address;
				unsigned short port;
				if (socket.receive(packet, address, port) != sf::Socket::Done)
				{
					// error...
					//recieve failed send hello again
					//std::cout << "no messages yet" << std::endl;
					check = false;
					break;
				}
				if (address != hostIP || port != hostPort)
				{
					check == false;
				}
				if ((packet >> id >> x >> y >> dx >> dy >> timeSent) && check)
				{
					//good
					Message msg;
					msg.id = id;
					msg.x = x;
					msg.y = y;
					msg.dirx = dx;
					msg.diry = dy;
					msg.timeSent = timeSent;

					messages.push_back(msg);

				}
				else if (check)
				{
					std::cout << "Couldnt receive message" << std::endl;
				}


				check = true;
				//Recieve ball position
				sf::Int8 id2;
				float x2;
				float y2;
				float dx2;
				float dy2;
				float timeSent2;
				sf::Packet packet2;
				sf::IpAddress address2;
				unsigned short port2;
				if (socket.receive(packet2, address2, port2) != sf::Socket::Done)
				{
					// error...
					//recieve failed send hello again
					//std::cout << "no messages yet" << std::endl;
					check = false;
					break;
				}
				if (address2 != hostIP || port2 != hostPort)
				{
					check == false;
				}
				if ((packet2 >> id2 >> x2 >> y2 >> dx2 >> dy2 >> timeSent2) && check)
				{
					//good
					Message msg2;
					msg2.id = id2;
					msg2.x = x2;
					msg2.y = y2;
					msg2.dirx = dx2;
					msg2.diry = dy2;
					msg2.timeSent = timeSent;

					messages.push_back(msg2);

				}
				else if (check)
				{
					std::cout << "Couldnt receive message" << std::endl;
				}
			}

		}

		//give messages to appropriate players
		while (!messages.empty())
		{
			if (messages.front().id == opponent.GetID())
			{
				opponent.AddMessage(messages.front());
				messages.pop_front();
			}
			else if (messages.front().id == ballID && myID != 1)
			{
				ball.AddMessage(messages.front());
				messages.pop_front();
			}
			else if (messages.front().id == 100 && myID != 1)
			{
				while (true)
				{
					Message msg;
					sf::Int8 id = 100;
					float x = 0;
					float y = 0;
					float dx = 0;
					float dy = 0;
					float timeSent = 0;
					sf::Packet packet;
					packet << id << x << y << dx << dy << timeSent;

					if (socket.send(packet, hostIP, hostPort) != sf::Socket::Done)
					{
						// error...
						//send failed try it again
						std::cout << "Send message failed" << std::endl;
						continue;
					}
					break;
				}
				player1Score += messages.front().x;
				player2Score += messages.front().y;
				Reset();
				messages.pop_front();
			}
			else if (messages.front().id == 99 && myID != 1)
			{
				sf::Vector2f ballVelocity = ball.GetVelocity();
				myPlayer.SetVelocity(sf::Vector2f((ball.GetVelocity().x) / 2, (ball.GetVelocity().y / 2)));
				float totalSpeed = MagnitudeVector2(myPlayer.GetVelocity()) + MagnitudeVector2(ballVelocity);
				sf::Vector2f pdir = NormaliseVector2(myPlayer.GetSprite().getPosition() - ball.GetSprite().getPosition());
				sf::Vector2f newVel = pdir * (totalSpeed / 4);

				myPlayer.SetVelocity(sf::Vector2f((myPlayer.GetVelocity().x + newVel.x) / 2, (myPlayer.GetVelocity().y + newVel.y) / 2));
				
				ball.SetVelocity(sf::Vector2f(messages.front().x, messages.front().y));
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
		std::string str = std::to_string(myID) + " Score " + std::to_string(player1Score) + " - " + std::to_string(player2Score);
		//text.setPosition((screenWidth / 2) - ((text.getString().getSize() * text.getCharacterSize()) / 2), 10);
		text.setPosition((screenWidth / 2) - 100, 10);

		text.setString(str);

		if (myID == 1)
		{
			ball.UpdateHost(currentTime, frameTime);
		}
		else
		{
			ball.UpdateClient(currentTime, frameTime);
		}


		CheckCollisions();
		myPlayer.UpdateSelf(currentTime, frameTime);
		opponent.UpdateOther(currentTime, frameTime);

		//for (auto player : playerList)
		//{
		//	if (player.GetID() == myID)
		//	{
		//		player.UpdateSelf(currentTime, frameTime);
		//	}
		//	else
		//	{
		//		player.UpdateOther(currentTime, frameTime);
		//	}
		//}
		msgReady = false;

		OutOfBounds();


		window.clear();
		//for (auto player : playerList)
		//{
		//	std::cout << "x " << player.GetSprite().getPosition().x << " y " << player.GetSprite().getPosition().y << std::endl;
		//	window.draw(player.GetSprite());
		//}

		window.draw(opponent.GetSprite());
		window.draw(myPlayer.GetSprite());
		window.draw(goal1);
		window.draw(goal2);
		window.draw(ball.GetSprite());
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
		if (socket.receive(packet, hostIP, hostPort) != sf::Socket::Done)
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
	myPlayer.SetPosition(sf::Vector2f(startPosX * myID, startPosY));
	ball.SetPosition(ballStartPos);

	myPlayer.SetVelocity(sf::Vector2f(0,0));
	opponent.SetVelocity(sf::Vector2f(0,0));
	ball.SetVelocity(sf::Vector2f(0,0));

	//player2.setPosition(player2StartPos);
	//
	//goal1.setPosition(goal1StartPos);
	//goal2.setPosition(goal2StartPos);
	//
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
		myPlayer.SetPosition(sf::Vector2f(myPlayer.GetPosition().x, screenHeight));
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
		else
		{
			hostIP = choice;

			while (true)
			{
				std::cout << "Enter port number of host" << std::endl;
				unsigned short portChoice;
				std::cin >> portChoice;
				hostPort = portChoice;
				InitClient();
				break;
			}
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

		if (socket.receive(packet, senderIP, senderPort) != sf::Socket::Done)
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
		if (socket.receive(packet, hostIP, hostPort) != sf::Socket::Done)
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

void CheckCollisions()
{
	if (myPlayer.GetSprite().getGlobalBounds().intersects(opponent.GetSprite().getGlobalBounds()))
	{
		float totalSpeed = MagnitudeVector2(myPlayer.GetVelocity()) + MagnitudeVector2(opponent.GetVelocity());
		sf::Vector2f dir1 = NormaliseVector2(myPlayer.GetSprite().getPosition() - opponent.GetSprite().getPosition());
		//sf::Vector2f dir2 = NormaliseVector2(player2.getPosition()) - NormaliseVector2(player1.getPosition());
		sf::Vector2f dir2 = -dir1;
		myPlayer.SetVelocity(dir1 * (totalSpeed / 2));
		//opponent.SetVelocity(dir2 * (totalSpeed / 2));

		//vel2 = sf::Vector2f(0.0f, 1.0f);
	}

	if (myID != 1)
	{
		//if (myPlayer.GetSprite().getGlobalBounds().intersects(ball.GetSprite().getGlobalBounds()))
		//{
		//	
		//	sf::Vector2f ballVelocity = ball.GetVelocity();
		//	float totalSpeed = MagnitudeVector2(myPlayer.GetVelocity()) + MagnitudeVector2(ballVelocity);
		//	sf::Vector2f pdir = NormaliseVector2(myPlayer.GetSprite().getPosition() - ball.GetSprite().getPosition());
		//	sf::Vector2f bdir = -pdir;
		//	sf::Vector2f newVel = pdir * (totalSpeed / 4);
		//
		//	myPlayer.SetVelocity(sf::Vector2f((myPlayer.GetVelocity().x + newVel.x) / 2, (myPlayer.GetVelocity().y + newVel.y) / 2));
		//	ballVelocity = bdir * (totalSpeed * 3 / 4);
		//	
		//	if (MagnitudeVector2(ballVelocity) == 0)
		//	{
		//		ballVelocity = bdir / 5.0f;
		//	}
		//
		//	//ball.SetVelocity(ballVelocity);
		//
		//	//Tell host you hit the ball
		//	Message msg;
		//	sf::Int8 id = 99;
		//	float x = ballVelocity.x;
		//	float y = ballVelocity.y;
		//	float timeSent = currentTime.asSeconds();
		//	sf::Packet packet;
		//	packet << id << x << y << timeSent;
		//
		//	if (socket.send(packet, hostIP, hostPort) != sf::Socket::Done)
		//	{
		//		// error...
		//		//send failed try it again
		//		std::cout << "Send message failed" << std::endl;
		//	}
		//}
		return;
	}

	//Only host past this point

	//check if any player collided with ball
	if (myPlayer.GetSprite().getGlobalBounds().intersects(ball.GetSprite().getGlobalBounds()))
	{
		sf::Vector2f ballVelocity = ball.GetVelocity();
		float totalSpeed = MagnitudeVector2(myPlayer.GetVelocity()) + MagnitudeVector2(ballVelocity);
		sf::Vector2f pdir = NormaliseVector2(myPlayer.GetSprite().getPosition() - ball.GetSprite().getPosition());
		sf::Vector2f bdir = -pdir;
		sf::Vector2f newVel = pdir * (totalSpeed / 4);

		myPlayer.SetVelocity(sf::Vector2f((myPlayer.GetVelocity().x + newVel.x) / 2, (myPlayer.GetVelocity().y + newVel.y) / 2));
		ballVelocity = bdir * (totalSpeed * 3 / 4);

		if (MagnitudeVector2(ballVelocity) == 0)
		{
			ballVelocity = bdir / 5.0f;
		}

		ball.SetVelocity(ballVelocity);
	}


	else if (opponent.GetSprite().getGlobalBounds().intersects(ball.GetSprite().getGlobalBounds()))
	{
		sf::Vector2f ballVelocity = ball.GetVelocity();
		float totalSpeed = MagnitudeVector2(opponent.GetVelocity()) + MagnitudeVector2(ballVelocity);
		sf::Vector2f pdir = NormaliseVector2(opponent.GetSprite().getPosition() - ball.GetSprite().getPosition());
		sf::Vector2f bdir = -pdir;
		sf::Vector2f newVel = pdir * (totalSpeed / 4);
	
		opponent.SetVelocity(sf::Vector2f((opponent.GetVelocity().x + newVel.x) / 2, (opponent.GetVelocity().y + newVel.y) / 2));
		ballVelocity = bdir * (totalSpeed * 3 / 4);
	
		if (MagnitudeVector2(ballVelocity) == 0)
		{
			ballVelocity = bdir / 5.0f;
		}
	
		ball.SetVelocity(ballVelocity);

		Message msg;
		sf::Int8 id = 99;
		float x = ballVelocity.x;
		float y = ballVelocity.y;
		float dx = 0;
		float dy = 0;
		float timeSent = currentTime.asSeconds();
		sf::Packet packet;
		packet << id << x << y << dx << dy << timeSent;

		if (socket.send(packet, clientIP, clientPort) != sf::Socket::Done)
		{
			// error...
			//send failed try it again
			std::cout << "Send message failed" << std::endl;
		}
	}

	if (ball.GetSprite().getGlobalBounds().intersects(goal1.getGlobalBounds()))
	{
		Goal(1, 0);
		Reset();
	}
	else if (ball.GetSprite().getGlobalBounds().intersects(goal2.getGlobalBounds()))
	{
		Goal(0, 1);
		Reset();
	}
	
}


void Goal(int score1, int score2)
{
	while (true)
	{
		player1Score += score1;
		player2Score += score2;
		Message msg;
		sf::Int8 id = 100;
		float player1 = score1;
		float player2 = score2;
		float dirx = 0;
		float diry = 0;
		float timeSent = 0;
		sf::Packet packet;
		packet << id << player1 << player2 << dirx << diry << timeSent;

		if (socket.send(packet, clientIP, clientPort) != sf::Socket::Done)
		{
			// error...
			//send failed try it again
			std::cout << "Send message failed" << std::endl;
			continue;
		}
		break;
	}

	while (true)
	{
		bool check = true;
		sf::Int8 id;
		float x;
		float y;
		float dx;
		float dy;
		float timeSent;
		sf::Packet packet;
		sf::IpAddress address;
		unsigned short port;
		if (socket.receive(packet, address, port) != sf::Socket::Done)
		{
			// error...
			//recieve failed send hello again
			//std::cout << "no messages yet" << std::endl;
			check = false;
			continue;
		}
		if (address != clientIP || port != clientPort)
		{
			check == false;
		}
		if ((packet >> id >> x >> y >> dx >> dy >> timeSent) && check)
		{
			//good
			Message msg;
			msg.id = id;
			msg.x = x;
			msg.y = y;
			msg.dirx = dx;
			msg.diry = dy;
			msg.timeSent = timeSent;

			if (msg.id == 100 && msg.x == 0 && msg.y == 0 && msg.timeSent == 0)
			{
				break;
			}

		}
		else if (check)
		{
			std::cout << "Couldnt receive message" << std::endl;
		}
	}
	Reset();
}
