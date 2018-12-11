#include <cassert>

#include "Player.h"



Player::Player()
{
	m_velocity = sf::Vector2f(0, 0);
}

void Player::Init(sf::Texture texture, sf::Vector2f startPos, sf::Color colour, int ID)
{
	m_texture = texture;
	m_position = startPos;
	m_colour = colour;
	m_ID = ID;

	m_sprite.setTexture(m_texture);
	m_sprite.setPosition(m_position);
	m_sprite.setScale(m_scale);
	m_sprite.setColor(m_colour);
	m_sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));

	m_acceleration = 0.025f;
	m_velocity = sf::Vector2f(0, 0);

}

void Player::UpdateSelf(sf::Time time, sf::Time frameTime)
{
	MAXSPEED2 = MAXSPEED;// *frameTime.asSeconds();
	m_acceleration = 40.025f * frameTime.asSeconds();

	m_dir.x = sinf((3.14159 / 180) * m_sprite.getRotation());
	m_dir.y = -cosf((3.14159 / 180) * m_sprite.getRotation());
	m_dir = NormaliseVector2(m_dir, 0.1f);//unsure if need this


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_sprite.rotate(-100.3f * frameTime.asSeconds());
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_sprite.rotate(100.3f * frameTime.asSeconds());
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		if (MagnitudeVector2(m_velocity) < MAXSPEED2)
		{
			m_velocity += m_dir * m_acceleration;
		}
	}

	if (MagnitudeVector2(m_velocity) > MAXSPEED2)
	{
		m_velocity = NormaliseVector2(m_velocity, MAXSPEED2);
	}
	else if (MagnitudeVector2(m_velocity) > 0)// && !sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		if (frameTime.asSeconds() != 0)
		{
			//m_velocity /= DRAG;
		}
	}
	m_position += m_velocity;
	m_sprite.setPosition(m_position);
}

void Player::UpdateOther(sf::Time time, sf::Time frameTime)
{

	m_sprite.setRotation(1.5f * 3.14159f - atan2f(m_dir.y, m_dir.x));

	m_position = PredictPosition(time.asSeconds(), frameTime.asSeconds());
	m_sprite.setPosition(m_position);
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

sf::Vector2f Player::GetPosition()
{
	return m_position;
}
void Player::SetPosition(sf::Vector2f pos)
{
	m_position = pos;
	m_sprite.setPosition(m_position);
}
sf::Vector2f Player::GetDir()
{
	return m_dir;
}
void Player::SetDir(sf::Vector2f dir)
{
	m_dir = dir;
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

sf::Vector2f Player::PredictPosition(float time, float frameTime)
{
	const int msize = m_messages.size();
	//assert(msize == m_reqMessages);
	if (msize != m_reqMessages)
	{
		//wrong messageList size;
		return m_position;

	}
	const Message& msg0 = m_messages[msize - 1];
	const Message& msg1 = m_messages[msize - 2];
	const Message& msg2 = m_messages[msize - 3];


	x = msg0.x;
	y = msg0.y;

	
	//Linear prediction
	float velx = (msg0.x - msg1.x) / (msg0.timeSent - msg1.timeSent);
	float vely = (msg0.y - msg1.y) / (msg0.timeSent - msg1.timeSent);
	m_velocity = sf::Vector2f(velx, vely) * frameTime;
	
	m_dir.x = msg0.dirx;
	m_dir.y = msg0.diry;
	
	
	x += velx * (time - msg0.timeSent) * frameTime;
	y += vely * (time - msg0.timeSent) * frameTime;

	return sf::Vector2f(x, y);

}
void Player::AddMessage(const Message &message)
{

	while (m_messages.size() >= m_reqMessages)
	{
		m_messages.erase(m_messages.begin());
	}
	m_messages.push_back(message);

	//Sorts the messages based on the their time value. In case recieved out of order
	if (m_messages.size() > 1)
	{
		for (int i = 0; i < m_messages.size() - 1; i++)
		{
			int t = i;
			while (m_messages[i].timeSent > m_messages[i + 1].timeSent)
			{
				t++;
				Message temp = m_messages[i];
				m_messages[i] = m_messages[t];
				m_messages[t] = temp;
			}
		}
	}
}

Player::~Player()
{
}
