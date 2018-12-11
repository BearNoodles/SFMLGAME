#include <cassert>

#include "Ball.h"



Ball::Ball()
{
	m_velocity = sf::Vector2f(0, 0);
}

void Ball::Init(sf::Texture texture, sf::Vector2f startPos, sf::Color colour, int screenW, int screenH, int ID)
{
	m_texture = texture;
	m_position = startPos;
	m_colour = colour;
	m_ID = ID;
	m_screenW = screenW;
	m_screenH = screenH;

	m_sprite.setTexture(m_texture);
	m_sprite.setPosition(m_position);
	m_sprite.setScale(m_scale);
	m_sprite.setColor(m_colour);
	m_sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2, texture.getSize().y / 2));

	m_velocity = sf::Vector2f(0, 0);

}

void Ball::UpdateHost(sf::Time time, sf::Time frameTime)
{
	MAXSPEED2 = MAXSPEED;// *frameTime.asSeconds();
										 
	//check if ball touched edges //Maybe only for host?
	if (m_position.x > m_screenW)
	{
		m_velocity.x *= -1;
		if (m_velocity.x >= 0)
		{
			m_velocity.x = -0.1f;
		}
	}
	if (m_position.x < 0)
	{
		m_velocity.x *= -1;
		if (m_velocity.x <= 0)
		{
			m_velocity.x = 0.1f;
		}
	}
	if (m_position.y > m_screenH)
	{
		m_velocity.y *= -1;
		if (m_velocity.y >= 0)
		{
			m_velocity.y = -0.1f;
		}
	}
	if (m_position.y < 0)
	{
		m_velocity.y *= -1;
		if (m_velocity.y <= 0)
		{
			m_velocity.y = 0.1f;
		}
	}


	if (MagnitudeVector2(m_velocity) > MAXSPEED2)
	{
		m_velocity = NormaliseVector2(m_velocity, MAXSPEED2);
	}
	else if (MagnitudeVector2(m_velocity) > 0)
	{
		if (frameTime.asSeconds() != 0)
		{
			m_velocity /= DRAG;
		}
	}
	m_position += m_velocity;// *frameTime.asSeconds();
	m_sprite.setPosition(m_position);
}

void Ball::UpdateClient(sf::Time time, sf::Time frameTime)
{

	m_position = PredictPosition(time.asSeconds());
	m_sprite.setPosition(m_position);
}

int Ball::GetID()
{
	return m_ID;
}
void Ball::SetID(int id)
{
	m_ID = id;
}
sf::Vector2f Ball::GetVelocity()
{
	return m_velocity;
}

void Ball::SetVelocity(sf::Vector2f velocity)
{
	m_velocity = velocity;
}

sf::Sprite Ball::GetSprite()
{
	return m_sprite;
}

sf::Vector2f Ball::GetPosition()
{
	return m_position;
}
void Ball::SetPosition(sf::Vector2f pos)
{
	m_position = pos;
	m_sprite.setPosition(m_position);
}


sf::Vector2f Ball::NormaliseVector2(sf::Vector2f vector, float speed)
{
	float length = sqrt((vector.x * vector.x) + (vector.y * vector.y));
	vector.x = (vector.x / length) * speed;
	vector.y = (vector.y / length) * speed;

	return vector;
}

float Ball::MagnitudeVector2(sf::Vector2f vector)
{
	return sqrt((vector.x * vector.x) + (vector.y * vector.y));
}

sf::Vector2f Ball::PredictPosition(float time)
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

	return sf::Vector2f(x, y);

	//Linear prediction
	float velx = (msg0.x - msg1.x) / (msg0.timeSent - msg1.timeSent);
	float vely = (msg0.y - msg1.y) / (msg0.timeSent - msg1.timeSent);
	x += velx * (time - msg0.timeSent);
	y += vely * (time - msg0.timeSent);

	//m_velocity = (sf::Vector2f(x, y) + sf::Vector2f(velx, vely)) - m_position;

	//Quadratic prediction
	//float velx0 = (msg0.x - msg1.x) / (msg0.timeSent - msg1.timeSent);
	//float vely0 = (msg0.y - msg1.y) / (msg0.timeSent - msg1.timeSent);
	//float velx1 = (msg1.x - msg2.x) / (msg1.timeSent - msg2.timeSent);
	//float vely1 = (msg1.y - msg2.y) / (msg1.timeSent - msg2.timeSent);
	//
	//float accx = (velx0 - velx1) / (msg0.timeSent - msg1.timeSent);
	//float accy = (vely0 - vely1) / (msg0.timeSent - msg1.timeSent);
	//
	//float dTime = time - msg0.timeSent;
	//
	//x += (velx0 * dTime) + 0.5f * (accx * (dTime * dTime));
	//y += (vely0 * dTime) + 0.5f * (accy * (dTime * dTime));
	//
	//float velx = velx0;
	//float vely = vely0;

	//Interpolated prediction
	//Message newPrediction;
	//newPrediction.x = x;
	//newPrediction.y = y;
	//newPrediction.velx = velx;
	//newPrediction.vely = vely;
	//newPrediction.timeSent = time;
	//newPrediction.id = msg0.id;
	//m_predictionHistory.push_back(newPrediction);
	//
	//while (m_predictionHistory.size() > m_reqMessages)
	//{
	//	m_predictionHistory.erase(m_predictionHistory.begin());
	//}
	//
	//if (m_predictionHistory.size() == m_reqMessages)
	//{
	//	const Message& imsg0 = m_predictionHistory[m_reqMessages - 1];
	//	const Message& imsg1 = m_predictionHistory[m_reqMessages - 2];
	//	const Message& imsg2 = m_predictionHistory[m_reqMessages - 3];
	//	float ix = imsg0.x;
	//	float iy = imsg0.y;
	//
	//	float ivelx = (imsg0.x - imsg1.x) / (imsg0.timeSent - imsg1.timeSent);
	//	float ively = (imsg0.y - imsg1.y) / (imsg0.timeSent - imsg1.timeSent);
	//	ix += ivelx * (time - imsg0.timeSent);
	//	iy += ively * (time - imsg0.timeSent);
	//
	//
	//	//m_velocity = sf::Vector2f(ivelx, ively);
	//	x = (x - ix) / 2;
	//	y = (y - iy) / 2;
	//}

	//return sf::Vector2f(x, y);
	return sf::Vector2f(x, y);

}
void Ball::AddMessage(const Message &message)
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

Ball::~Ball()
{
}
