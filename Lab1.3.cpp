#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <iostream>

static const int MAX_COUNT_BLOCK = 9;
static const float SPEED = 0.07f;
static const sf::Time TIME_PER_FRAME = sf::seconds(1.f / 60.f);
static const int WINDOW_HEIGHT = 700;
static const int WINDOW_WIDTH = 700;
static const int DISTANCE_BETWEEN_BLOCKS = 30;
static const sf::Vector2f POSITION_CENTER_BLOCKS = {340, 270.f };
static const float PI = 3.14159f;

enum class Step
{
	FIRST,
	SECOND,
	THIRD,
};

struct Rectangle
{
	Rectangle(sf::Vector2f const& position, sf::Vector2f const& size, sf::Vector2f const& scale, int i);
	sf::RectangleShape shape;
	float currentAngle;
	int index;
	bool isRotation;
};

typedef std::vector<Rectangle*> Rectangles;

Rectangle::Rectangle(sf::Vector2f const& position, sf::Vector2f const& size, sf::Vector2f const& scale, int i)
{
	shape.setPosition(position);
	shape.setSize(size);
	shape.setOrigin(size.x/2, size.y/2);
	shape.setScale(scale);
	shape.setFillColor(sf::Color(102, 0, 102));
	currentAngle = 0;
	index = i;
	isRotation = false;
}

void AnimateFirst(float elapsedTime, Rectangle* rectangle, Step & step)
{
	sf::Vector2f position = rectangle->shape.getPosition();
	sf::Vector2f size = rectangle->shape.getSize();
	if (position.x + size.x / 2 <= WINDOW_WIDTH / 2)
	{
		rectangle->shape.move(SPEED * elapsedTime, 0);
	}
	else
	{
		step = Step::SECOND;
	}
}

void AnimateSecond(float elapsedTime, Rectangle* rectangle, Step & step)
{
	sf::Vector2f position = rectangle->shape.getPosition();
	sf::Vector2f size = rectangle->shape.getSize();
	if (int(position.y) != POSITION_CENTER_BLOCKS.y)
	{
		if (rectangle->index >= 5)
		{
			rectangle->shape.move(-SPEED * elapsedTime, -SPEED * elapsedTime);
		}
		else if(rectangle->index <= 5)
		{
			rectangle->shape.move(SPEED * elapsedTime, SPEED * elapsedTime);
		}
	}
	else if(rectangle->index == MAX_COUNT_BLOCK - 1)
	{
		step = Step::THIRD;
	}
}


/*void AnimateFourth(float elapsedTime, Rectangle* rectangle, Step & step, sf::Vector2f & center)
{
	sf::Vector2f position = rectangle->shape.getPosition();
	sf::Vector2f size = rectangle->shape.getSize();
	if (int(rectangle->currentAngle) != 45)
	{
		float dx = center.x - position.x;
		float dy = center.y - position.y;
		float distance = hypot(dx, dy);
		if (rectangle->index == 4)
		{
		//	std::cout << rectangle->shape.getPosition().x << std::endl;
		}
		float vx = cos(rectangle->currentAngle) * distance;
		float vy = sin(rectangle->currentAngle) * distance;
		rectangle->shape.setRotation(rectangle->currentAngle);

		rectangle->shape.setPosition(vx + center.x, vy+ center.y);
		rectangle->currentAngle += 0.1;
	}
}*/


void AnimateThird(Rectangle* rectangle, Step & step, sf::Vector2f & center)
{
	sf::Vector2f position = rectangle->shape.getPosition();
	float dx = center.x - position.x;
	float dy = center.y - position.y;
	float distance = hypot(dx, dy);
	if (rectangle->index > 4)
	{
		distance *= -1;
	}
	rectangle->shape.setRotation(cos(rectangle->currentAngle));
	float vx = cos(rectangle->currentAngle) * distance;
	float vy = sin(rectangle->currentAngle) * distance;
	rectangle->shape.setPosition(vx + center.x, vy+ center.y);
	rectangle->currentAngle += 0.01f;
}

void Update(float elapsedTime, Rectangles & rectangles, Step & step)
{
	sf::Vector2f origin = rectangles[4]->shape.getOrigin();
	sf::Vector2f center = rectangles[4]->shape.getPosition();
	std::cout << center.x << " | " << center.y << std::endl;
	if (step != Step::THIRD)
	{
		for (auto rectangle : rectangles)
		{
			switch (step)
			{
			case Step::FIRST:
				AnimateFirst(elapsedTime, rectangle, step);
				break;
			case Step::SECOND:
				AnimateSecond(elapsedTime, rectangle, step);
				break;
			default:
				break;
			}
		}
	}
	else
	{
		for(auto rectangle : rectangles)
		{
			if (rectangle->shape.getRotation() <= 90)
			{
				AnimateThird(rectangle, step, center);
				break;
			}
		}
	}
}


void HandleEvents(sf::RenderWindow & window)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
		}
	}
}

void Render(sf::RenderWindow & window, Rectangles const& rectangles, float elapsedTime)
{
	window.clear(sf::Color::White);
	for (auto rectangle : rectangles)
	{
		window.draw(rectangle->shape);
	}
	window.display();
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML!");
	Rectangles rectangles;
	float stepRec = 0;
	for (int i = 0; i != MAX_COUNT_BLOCK; ++i)
	{
		rectangles.push_back(new Rectangle(sf::Vector2f(0, 150 + stepRec), sf::Vector2f(20, 20), sf::Vector2f(1, 1), i));
		stepRec += DISTANCE_BETWEEN_BLOCKS;
	}
	sf::Clock clock;

	Step step = Step::FIRST;
	while (window.isOpen())
	{
		float elapsedTime = clock.getElapsedTime().asMicroseconds(); 
		clock.restart(); 
		elapsedTime = elapsedTime / 800; 
		HandleEvents(window);
		Update(elapsedTime, rectangles, step);
		Render(window, rectangles, elapsedTime);
	}

	return 0;
}
/*class CRectangle
{
public:
	CRectangle(sf::Vector2f const& position, sf::Vector2f const& size, sf::Vector2f const& scale, Step const& direction);
	void Update(float elapsedTime);
	void Draw(sf::RenderWindow & window);
	void GenerateRectangle();
private:
	sf::RectangleShape m_rectangle;
	Step m_direction;
	void AnimateRight(float elapsedTime);
	void AnimateDiagonally();
};

void CRectangle::GenerateRectangle()
{

}

CRectangle::CRectangle(sf::Vector2f const& position, sf::Vector2f const& size, sf::Vector2f const& scale, Step const& direction)
{
	m_rectangle.setPosition(position);
	m_rectangle.setSize(size);
	m_rectangle.setOrigin(size.x / 2, size.y / 2);
	m_rectangle.setScale(scale);
	m_rectangle.setFillColor(sf::Color(102, 0, 102));
	m_direction = direction;
}

void CRectangle::AnimateRight(float elapsedTime)
{
	sf::Vector2f position = m_rectangle.getPosition();
	sf::Vector2f size = m_rectangle.getSize();

	if (position.x + size.x / 2 <= WINDOW_WIDTH)
	{
		//m_rectangle.setRotation(m_rectangle.getRotation() + 0.1);
		m_rectangle.move(SPEED * elapsedTime, 0);
	}
}

void CRectangle::AnimateDiagonally()
{

}

void CRectangle::Update(float elapsedTime)
{
	switch (m_direction)
	{
	case Step::FIRST:
		AnimateRight(elapsedTime);
		break;

	default:
		break;
	}
}

void CRectangle::Draw(sf::RenderWindow & window)
{
	window.draw(m_rectangle);
}

void Render(sf::RenderWindow & window, std::vector<CRectangle*> & rectangles, float elapsedTime)
{
	window.clear(sf::Color::White);
	for (auto rectangle : rectangles)
	{
		rectangle->Update(elapsedTime);
		rectangle->Draw(window);
	}
	window.display();
}

void HandleEvents(sf::RenderWindow & window)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
		}
	}
}
int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML!");
	std::vector<CRectangle*> rectangles;
	float step = 0;
	for (int i = 0; i != MAX_COUNT_BLOCK; ++i)
	{
		step += 50;
		rectangles.push_back(new CRectangle(sf::Vector2f(0, 100 + step) , sf::Vector2f(30, 30), sf::Vector2f(1, 1), Step::FIRST));
	}

	sf::Clock clock;
	while (window.isOpen())
	{
		float elapsedTime = clock.getElapsedTime().asMicroseconds(); //дать прошедшее время в микросекундах
		clock.restart(); //перезагружает время
		elapsedTime = elapsedTime / 800; //скорость игры
		HandleEvents(window);
		Render(window, rectangles, elapsedTime);
			
	}

	return 0;
}*/