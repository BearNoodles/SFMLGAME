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

void Player::UpdateSelf()
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

	m_sprite.move(m_velocity);
	m_position = m_sprite.getPosition();
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

void Player::PredictPosition(float time) {
	const int msize = m_messages.size();
	std::assert(msize == m_reqMessages);
	const Message& msg0 = m_messages[msize - 1];
	const Message& msg1 = messages_[msize - 2];
	const Message& msg2 = messages_[msize - 3];

	// FIXME: Implement prediction here!
	// You have:
	// - the history of position messages received, in "messages_"
	//   (msg0 is the most recent, msg1 the 2nd most recent, msg2 the 3rd most recent)
	// - the current time, in "time"
	// You need to update:
	// - the predicted position at the current time, in "x_" and "y_"

	x_ = msg0.x;
	y_ = msg0.y;

	//Linear prediction
	float velx = (msg0.x - msg1.x) / (msg0.time - msg1.time);
	float vely = (msg0.y - msg1.y) / (msg0.time - msg1.time);
	x_ += velx * (time - msg0.time);
	y_ += vely * (time - msg0.time);



	//Quadratic prediction
	//float velx0 = (msg0.x - msg1.x) / (msg0.time - msg1.time);
	//float vely0 = (msg0.y - msg1.y) / (msg0.time - msg1.time);
	//float velx1 = (msg1.x - msg2.x) / (msg1.time - msg2.time);
	//float vely1 = (msg1.y - msg2.y) / (msg1.time - msg2.time);
	//
	//float accx = (velx0 - velx1) / (msg0.time - msg1.time);
	//float accy = (vely0 - vely1) / (msg0.time - msg1.time);
	//
	//float dTime = time - msg0.time;
	//
	//x_ += (velx0 * dTime) + 0.5f * (accx * (dTime * dTime));
	//y_ += (vely0 * dTime) + 0.5f * (accy * (dTime * dTime));

	//Interpolated prediction
	TankMessage newPrediction;
	newPrediction.x = x_;
	newPrediction.y = y_;
	newPrediction.time = time;
	newPrediction.id = msg0.id;
	predictionHistory.push_back(newPrediction);

	if (predictionHistory.size() == reqMessages)
	{
		const TankMessage& imsg0 = predictionHistory[reqMessages - 1];
		const TankMessage& imsg1 = predictionHistory[reqMessages - 2];
		const TankMessage& imsg2 = predictionHistory[reqMessages - 3];
		float ix_ = imsg0.x;
		float iy_ = imsg0.y;

		float ivelx = (imsg0.x - imsg1.x) / (imsg0.time - imsg1.time);
		float ively = (imsg0.y - imsg1.y) / (imsg0.time - imsg1.time);
		ix_ += ivelx * (time - imsg0.time);
		iy_ += ively * (time - imsg0.time);

		x_ = (x_ - ix_) / 2;
		y_ = (y_ - iy_) / 2;
	}

}
void Player::AddMessage(const Message &message)
{

	if (m_messages.size() == m_reqMessages)
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
