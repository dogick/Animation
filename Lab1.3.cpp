#include "stdafx.h"

static const int MAX_COUNT_BLOCK = 9;
static const float SPEED = 0.06f;
static const sf::Time TIME_PER_FRAME = sf::seconds(1.f / 60.f);
static const int WINDOW_HEIGHT = 700;
static const int WINDOW_WIDTH = 700;
static const int DISTANCE_BETWEEN_BLOCKS = 35;
static const sf::Vector2f POSITION_CENTER_BLOCKS = { 340, 270.f };
static const float PI = 3.14159f;
static const float STEP_ANGLE = 0.007f;
static const int CENTER_ELEMENT_INDEX = 4;
static const int MAX_ANGLE = 90;
static const sf::Color BLOCK_COLOR = sf::Color(102, 0, 102);
using namespace std;
using namespace std::placeholders;
enum class Step
{
	FIRST,
	SECOND,
	THIRD,
	FOURTH,
	FIFTH,
	SIXTH,
    SEVENTH,
};


struct Rectangle
{
	Rectangle(sf::Vector2f const& position, sf::Vector2f const& size, sf::Vector2f const& scale, int i);
	sf::RectangleShape shape;
	float currentAngle;
	int index;
	float transparent;
};

typedef std::vector<Rectangle*> Rectangles;
typedef std::map<Step, std::function<void(float elapsedTime, Rectangle * rectangle, Step & step, sf::Vector2f const& center)>> Action;

Rectangle::Rectangle(sf::Vector2f const& position, sf::Vector2f const& size, sf::Vector2f const& scale, int i)
{
	shape.setPosition(position);
	shape.setSize(size);
	shape.setOrigin(size.x / 2, size.y / 2);
	shape.setScale(scale);
	shape.setFillColor(sf::Color(BLOCK_COLOR));
	currentAngle = 0;
	index = i;
	transparent = 255;
}

void AnimateFirst(float elapsedTime, Rectangle* rectangle, Step & step, sf::Vector2f const& center)
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

void AnimateSecond(float elapsedTime, Rectangle* rectangle, Step & step, sf::Vector2f const& center)
{
	sf::Vector2f position = rectangle->shape.getPosition();
	sf::Vector2f size = rectangle->shape.getSize();
	float speed = SPEED * elapsedTime;

	if (int(position.y) != POSITION_CENTER_BLOCKS.y)
	{
		if (rectangle->index >= CENTER_ELEMENT_INDEX)
		{
			rectangle->shape.move(-speed, -speed);
		}
		else if (rectangle->index <= CENTER_ELEMENT_INDEX)
		{
			rectangle->shape.move(speed, speed);
		}
	}
	else if (rectangle->index == MAX_COUNT_BLOCK - 1)
	{
		step = Step::THIRD;
	}
}

void AnimateThird(float elapsedTime, Rectangle* rectangle, Step & step, sf::Vector2f const& center)
{
	sf::Vector2f position = rectangle->shape.getPosition();
	float dx = center.x - position.x;
	float dy = center.y - position.y;
	float distance = hypot(dx, dy);

	if (rectangle->index > CENTER_ELEMENT_INDEX)
	{
		distance *= -1;
	}
	rectangle->shape.setRotation(rectangle->currentAngle * 180 / M_PI);

	float vx = cos(rectangle->currentAngle) * distance;
	float vy = sin(rectangle->currentAngle) * distance;
	rectangle->shape.setPosition(vx + center.x, vy + center.y);
	rectangle->currentAngle += STEP_ANGLE;
    if ((rectangle->index == 8) && (rectangle->shape.getRotation() >= MAX_ANGLE))
    {
        step = Step::FOURTH;
    }
}

void AnimateFouth(float elapsedTime, Rectangle* rectangle, Step & step, sf::Vector2f const& center)
{
	sf::Vector2f position = rectangle->shape.getPosition();
	sf::Vector2f size = rectangle->shape.getSize();
	float speed = SPEED * elapsedTime;
	if (position.x <= 450)
	{
		rectangle->shape.setFillColor(sf::Color(102, 0, 102, static_cast<float>(rectangle->transparent)));
		rectangle->transparent = rectangle->transparent - 0.1f;
		rectangle->shape.move(speed, speed * 2);
	}
	else
	{
		step = Step::FIFTH;
	}
}

void AnimateFifth(float elapsedTime, Rectangle* rectangle, Step & step, sf::Vector2f const& center)
{
	sf::Vector2f scale = rectangle->shape.getScale();
	sf::Vector2f position = rectangle->shape.getPosition();
	sf::Vector2f size = rectangle->shape.getSize();
	float speed = SPEED * elapsedTime;
	if (position.x >= WINDOW_WIDTH / 2)
	{
		if (scale.x >= -1)
		{
			rectangle->shape.setScale(scale.x - 0.01f, scale.x - 0.01f);
		}
		rectangle->shape.move(-speed, 0);
	}
	else
	{
		step = Step::SIXTH;
	}
}

void AnimateSixth(float elapsedTime, Rectangle* rectangle, Step & step, sf::Vector2f const& center)
{
	sf::Vector2f position = rectangle->shape.getPosition();
	float dx = center.x - position.x;
	float dy = center.y - position.y;
	float distance = hypot(dx, dy);
	rectangle->shape.setRotation(rectangle->currentAngle * 180.f / M_PI);
	if (rectangle->transparent <= 255)
	{
		rectangle->transparent += 0.6;
	}
	rectangle->shape.setFillColor(sf::Color(102, 0, 102, static_cast<float>(rectangle->transparent)));
	float vx = cos(rectangle->currentAngle) * distance;
	float vy = sin(rectangle->currentAngle) * distance;
	rectangle->shape.setPosition(vx + center.x, vy + center.y);
	rectangle->currentAngle += STEP_ANGLE;
    if ((rectangle->index == 8) && (rectangle->shape.getRotation() >= 270))
    {
        step = Step::SEVENTH;
    }
}

struct tm GetLocalSystemTime()
{
    std::time_t currentTime = std::time(NULL);
    struct tm ptm;
    localtime_s(&ptm, &currentTime);
    return ptm;
}

void AnimateSeventh(float elapsedTime, Rectangle * rectangle, Step & step, sf::Vector2f const& center)
{
}


void Update(float elapsedTime, Rectangles & rectangles, Step & step)// Use std::vector and std::function to encapsulate actions
{
    const sf::Vector2f positionTheCenterElement = rectangles[4]->shape.getPosition();
    const sf::Vector2f positionTheFirstElement = rectangles[8]->shape.getPosition();
    const Action action{
        { Step::FIRST, std::bind(AnimateFirst, _1, _2, _3, _4) },
        { Step::SECOND, std::bind(AnimateSecond, _1, _2, _3, _4) },
        { Step::THIRD, std::bind(AnimateThird, _1, _2, _3, _4) },
        { Step::FOURTH, std::bind(AnimateFouth, _1, _2, _3, _4) },
        { Step::FIFTH, std::bind(AnimateFifth, _1, _2, _3, _4) },
        { Step::SIXTH, std::bind(AnimateSixth, _1, _2, _3, positionTheFirstElement) },
        { Step::SEVENTH, std::bind(AnimateSeventh, _1, _2, _3, positionTheFirstElement) },
    };
	for (auto rectangle : rectangles)
	{
        auto it = action.find(step);
        if (it != action.end())
        {
            if ((step == Step::THIRD) && (rectangle->shape.getRotation() <= MAX_ANGLE) || (step == Step::SIXTH) && (rectangle->shape.getRotation() <= 270))
            {
                it->second(elapsedTime, rectangle, step, positionTheCenterElement);
                break;
            }
            else if ((step != Step::THIRD) && (step != Step::SIXTH))
            {
                it->second(elapsedTime, rectangle, step, positionTheCenterElement);
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

void CreateRectangles(Rectangles & rectangles)
{
	float stepRec = 0;
	for (int i = 0; i != MAX_COUNT_BLOCK; ++i)
	{
		rectangles.push_back(new Rectangle(sf::Vector2f(150, 150 + stepRec), sf::Vector2f(25, 25), sf::Vector2f(1, 1), i));
		stepRec += DISTANCE_BETWEEN_BLOCKS;
	}
}

float GetElapsedTime(sf::Clock & clock)
{
	float elapsedTime = static_cast<float>(clock.getElapsedTime().asMicroseconds());
	clock.restart();
	return elapsedTime / 800;
}
int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML!", sf::Style::Default, settings);
	Rectangles rectangles;
	CreateRectangles(rectangles);
	sf::Clock clock;
	Step step = Step::FIRST;
	while (window.isOpen())
	{
		float elapsedTime = GetElapsedTime(clock);
		HandleEvents(window);
		Update(elapsedTime, rectangles, step);
		Render(window, rectangles, elapsedTime);
	}

	return 0;
}