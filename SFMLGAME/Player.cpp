#include <cassert>

#include "Player.h"



Player::Player()
{
	m_acceleration = 0.025f;
	m_velocity = sf::Vector2f(1, 0);

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
	m_dir.x = sinf((3.14159 / 180) * m_sprite.getRotation());
	m_dir.y = -cosf((3.14159 / 180) * m_sprite.getRotation());
	m_dir = NormaliseVector2(m_dir, 0.1f);//unsure if need this

	//TODO update controls
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_sprite.rotate(-0.3f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_sprite.rotate(0.3f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		if (MagnitudeVector2(m_velocity) < MAXSPEED)
		{
			m_velocity += m_dir * m_acceleration;
		}
	}

	if (MagnitudeVector2(m_velocity) > MAXSPEED)
	{
		m_velocity = NormaliseVector2(m_velocity, MAXSPEED);
	}
	else if (MagnitudeVector2(m_velocity) > 0)// && !sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		m_velocity /= DRAG;
	}
	sf::Vector2f ne = m_velocity * time.asSeconds();
	m_sprite.move(m_velocity * time.asSeconds() / 100.0f);
	m_position = m_sprite.getPosition();
}

void Player::UpdateOther(sf::Time time, sf::Time frameTime)
{
	m_frameTime = frameTime;
	m_position = PredictPosition(time.asSeconds());
	//TODO predict and set opponent velocity
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

sf::Vector2f Player::PredictPosition(float time) 
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
	//float velx = (msg0.posx - msg1.posx) / (msg0.timeSent - msg1.timeSent);
	//float vely = (msg0.posy - msg1.posy) / (msg0.timeSent - msg1.timeSent);
	//x += velx * (time - msg0.timeSent);
	//y += vely * (time - msg0.timeSent);



	//Quadratic prediction
	float velx0 = (msg0.x - msg1.x) / (msg0.timeSent - msg1.timeSent);
	float vely0 = (msg0.y - msg1.y) / (msg0.timeSent - msg1.timeSent);
	float velx1 = (msg1.x - msg2.x) / (msg1.timeSent - msg2.timeSent);
	float vely1 = (msg1.y - msg2.y) / (msg1.timeSent - msg2.timeSent);
	
	float accx = (velx0 - velx1) / (msg0.timeSent - msg1.timeSent);
	float accy = (vely0 - vely1) / (msg0.timeSent - msg1.timeSent);
	
	float dTime = time - msg0.timeSent;
	
	x += (velx0 * dTime) + 0.5f * (accx * (dTime * dTime));
	y += (vely0 * dTime) + 0.5f * (accy * (dTime * dTime));

	//Interpolated prediction
	Message newPrediction;
	newPrediction.x = x;
	newPrediction.y = y;
	newPrediction.velx = velx0;
	newPrediction.vely = vely0;
	newPrediction.timeSent = time;
	newPrediction.id = msg0.id;
	m_predictionHistory.push_back(newPrediction);

	while (m_predictionHistory.size() > m_reqMessages)
	{
		m_predictionHistory.erase(m_predictionHistory.begin());
	}

	if (m_predictionHistory.size() == m_reqMessages)
	{
		const Message& imsg0 = m_predictionHistory[m_reqMessages - 1];
		const Message& imsg1 = m_predictionHistory[m_reqMessages - 2];
		const Message& imsg2 = m_predictionHistory[m_reqMessages - 3];
		float ix = imsg0.x;
		float iy = imsg0.y;

		float ivelx = (imsg0.x - imsg1.x) / (imsg0.timeSent - imsg1.timeSent);
		float ively = (imsg0.y - imsg1.y) / (imsg0.timeSent - imsg1.timeSent);
		ix += ivelx * (time - imsg0.timeSent);
		iy += ively * (time - imsg0.timeSent);

		x = (x - ix) / 2;
		y = (y - iy) / 2;
	}

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
			int x = i;
			while (m_messages[i].timeSent > m_messages[i + 1].timeSent)
			{
				x++;
				Message temp = m_messages[i];
				m_messages[i] = m_messages[x];
				m_messages[x] = temp;
			}
		}
	}
}

Player::~Player()
{
}
