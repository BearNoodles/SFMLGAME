#include "Player.h"



Player::Player(sf::Texture texture, sf::Vector2f startPos, sf::Color colour, int ID)
{
	m_sprite.setTexture(texture);
	m_sprite.setPosition(startPos);
	m_sprite.setScale(m_scale);
	m_sprite.setColor(colour);
	m_sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));
}

void Player::UpdateSelf()
{
	m_dir.x = sinf((3.14159 / 180) * m_sprite.getRotation());
	m_dir.y = -cosf((3.14159 / 180) * m_sprite.getRotation());
	m_dir = NormaliseVector2(m_dir, 0.1f);//unsure if need this

	//TODO update controls
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		if (MagnitudeVector2(m_velocity) < MAXSPEED)
		{
			m_velocity += m_dir * m_acceleration;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_sprite.rotate(-0.3f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_sprite.rotate(0.3f);
	}

	if (MagnitudeVector2(m_velocity) > MAXSPEED)
	{
		m_velocity = NormaliseVector2(m_velocity, MAXSPEED);
	}
	else if (MagnitudeVector2(m_velocity) > 0 && !sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		m_velocity /= DRAG;
	}

	m_sprite.move(m_velocity);
}

void Player::UpdateOther()
{

}

int Player::GetID()
{
	return m_ID;
}
void Player::SetID(int id)
{
	m_ID = id;
}
sf::Vector2f Player::GetVelocity()
{
	return m_velocity;
}

void Player::SetVelocity(sf::Vector2f velocity)
{
	m_velocity = velocity;
}

sf::Sprite Player::GetSprite()
{
	return m_sprite;
}

void Player::SetPosition(sf::Vector2f pos)
{
	m_sprite.setPosition(pos);
}
sf::IpAddress Player::GetIP()
{
	return m_IP;
}
void Player::SetIP(sf::IpAddress IP)
{
	m_IP = IP;
}
unsigned short Player::GetPort()
{
	return m_port;
}
void Player::SetPort(unsigned short port)
{
	m_port = port;
}

sf::Vector2f Player::NormaliseVector2(sf::Vector2f vector, float speed)
{
	float length = sqrt((vector.x * vector.x) + (vector.y * vector.y));
	vector.x = (vector.x / length) * speed;
	vector.y = (vector.y / length) * speed;

	return vector;
}

float Player::MagnitudeVector2(sf::Vector2f vector)
{
	return sqrt((vector.x * vector.x) + (vector.y * vector.y));
}

Player::~Player()
{
}
