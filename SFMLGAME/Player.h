#pragma once
#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>

#include <vector>
#include "Message.h"

class Player
{
public:
	Player();
	~Player();

	void Init(sf::Texture texture, sf::Vector2f startPos, sf::Color colour, int ID);

	void UpdateSelf(sf::Time time, sf::Time frameTime);
	void UpdateOther(sf::Time time, sf::Time frameTime);

	int GetID();
	void SetID(int id);

	sf::Vector2f GetPosition();
	void SetPosition(sf::Vector2f pos);

	sf::Vector2f GetVelocity();
	void SetVelocity(sf::Vector2f velocity);

	sf::Sprite GetSprite();

	sf::Vector2f PredictPosition(float time);
	void AddMessage(const Message &msg);

	sf::IpAddress GetIP();
	void SetIP(sf::IpAddress IP);
	unsigned short GetPort();
	void SetPort(unsigned short port);

private:
	int m_ID;
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	sf::Vector2f m_position;
	sf::Color m_colour;
	sf::Vector2f m_velocity;
	sf::Vector2f m_scale = sf::Vector2f(0.03f, 0.03f);
	sf::Vector2f m_dir;
	float MAXSPEED = 1.0f;
	float m_acceleration;
	float DRAG = 1.0005f;

	sf::Time m_frameTime;

	std::vector<Message> m_messages;
	std::vector<Message> m_predictionHistory;

	float x;
	float y;
	int m_reqMessages = 3;

	sf::IpAddress m_IP;
	unsigned short m_port;

	sf::Vector2f NormaliseVector2(sf::Vector2f vector, float speed);

	float MagnitudeVector2(sf::Vector2f vector);
};

