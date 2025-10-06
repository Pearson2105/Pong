#include <SFML/Graphics.hpp>
#include <iostream>

const sf::Keyboard::Key controls[4] =
{
	sf::Keyboard::A,      //Player1 UP
	sf::Keyboard::Z,      //Player1 Down
	sf::Keyboard::Up,     //Player2 UP
	sf::Keyboard::Down    //Player2 Down
};

//Parameters
const sf::Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;
const float time_step = 0.017f; //60fps

//Ball physics
const float velocityMultiplier = 1.1f;
const float initialVelocityX = 200.f;
const float initialVelocityY = 120.f;

//Objects of the game
sf::CircleShape ball;
sf::RectangleShape paddles[2];

//Ball vel and state
sf::Vector2f ballVelocity;
bool isPlayer1Serving = true;

//Score
int score1 = 0;
int score2 = 0;

sf::Font font;
sf::Text text;

void reset(bool player1Serve = true)
{
	isPlayer1Serving = player1Serve;
	ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);
	ballVelocity = { (isPlayer1Serving ? initialVelocityX : -initialVelocityX), initialVelocityY };

	//reset paddle position
	paddles[0].setPosition(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f);
	paddles[1].setPosition(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f);
	
	text.setString(std::to_string(score1) + "   |   " + std::to_string(score2));

	text.setPosition((gameWidth * 0.5f) - (text.getLocalBounds().width * 0.5f), 10.f);
}


void init()
{
	if (!font.loadFromFile("C:/Users/Pearson/Pong/resources/Fonts/static/RobotoMono-Regular.ttf"))
	{
		std::cerr << "Error: Could not load font" << std::endl;
	}

	//Config score text
	text.setFont(font);
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::White);
	text.setStyle(sf::Text::Bold);

	//Set size and origin of paddles
	for (sf::RectangleShape& p : paddles)
	{
		p.setSize(paddleSize);
		p.setOrigin(paddleSize / 2.f);
	}
	//Set size and origin of ball
	ball.setRadius(ballRadius);
	ball.setOrigin(ballRadius, ballRadius);

	reset();
}

void update(float dt)
{
	//Player1 mov
	float direction1 = 0.0f;
	if (sf::Keyboard::isKeyPressed(controls[0])) direction1 -= 1.f;
	if (sf::Keyboard::isKeyPressed(controls[1])) direction1 += 1.f;
	paddles[0].move(sf::Vector2f(0.f, direction1 * paddleSpeed * dt));

	//Player2 mov
	float direction2 = 0.0f;
	if (sf::Keyboard::isKeyPressed(controls[2])) direction2 -= 1.f;
	if (sf::Keyboard::isKeyPressed(controls[3])) direction2 += 1.f;
	paddles[1].move(sf::Vector2f(0.f, direction2 * paddleSpeed * dt));

	//Keep padles in bounds
	for (sf::RectangleShape& p : paddles)
	{
		sf::Vector2f pos = p.getPosition();
		if (pos.y - paddleSize.y / 2.f < 0)
			p.setPosition(pos.x, paddleSize.y / 2.f);
		if (pos.y + paddleSize.y / 2.f > gameHeight)
			p.setPosition(pos.x, gameHeight - paddleSize.y / 2.f);
	}

	//Ball mov
	ball.move(ballVelocity * dt);

	float bx = ball.getPosition().x;
	float by = ball.getPosition().y;

	//Wall coli
	if (by - ballRadius < 0.f)
	{
		ballVelocity.y = std::abs(ballVelocity.y);
	}
	else if (by + ballRadius > gameHeight)
	{
		ballVelocity.y = -std::abs(ballVelocity.y);
	}

	//Left paddle coli
	sf::FloatRect ballBounds = ball.getGlobalBounds();
	sf::FloatRect leftBounds = paddles[0].getGlobalBounds();
	sf::FloatRect rightBounds = paddles[1].getGlobalBounds();

	if (ballBounds.intersects(leftBounds))
	{
		ballVelocity.x = std::abs(ballVelocity.x) * velocityMultiplier;
		ballVelocity.y *= velocityMultiplier;
	}
	else if (ballBounds.intersects(rightBounds))
	{
		ballVelocity.x = -std::abs(ballVelocity.x) * velocityMultiplier;
		ballVelocity.y *= velocityMultiplier;
	}

	//Scoring
	if (bx - ballRadius < 0.f)
	{
		score2++;
		reset(false);
	}
	else if (bx + ballRadius > gameWidth)
	{
		score1++;
		reset(true);
	}
}

void render(sf::RenderWindow &window)
{
	window.draw(paddles[0]);
	window.draw(paddles[1]);
	window.draw(ball);
	window.draw(text);
}


int main()
{
	sf::RenderWindow window(sf::VideoMode({ gameWidth, gameHeight }), "PONG");
	//init nd load
	init();

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		float dt = clock.restart().asSeconds();

		update(dt);

		window.clear(sf::Color::Black);
		render(window);
		window.display();

		sf::sleep(sf::seconds(time_step));
	}
	//unload nd shutdown
	return 0;
}