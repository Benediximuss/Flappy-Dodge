#include "SFML.h"

#include <cstdlib>
#include <sstream>

using namespace sf;

int main()
{

	bool pause = false;

	srand(time(NULL));

	sf::RenderWindow window(VideoMode(640, 480), "Flappy Dodge");
	window.setFramerateLimit(60);
	
	window.setMouseCursorVisible(false);

	//Bomb
	Texture bombTex;
	Texture explosionTex;
	Sprite bomb;

	if (!bombTex.loadFromFile("Textures/bomb.png"))
		throw "Could not load bomb.png!";

	if (!explosionTex.loadFromFile("Textures/explosion.png"))
		throw "Could not load bomb.png!";

	bomb.setTexture(bombTex);
	bomb.setScale(Vector2f(0.2f, 0.2f));
	int bombSpawnTimer = 15;

	std::vector<Sprite> bombs;
	bombs.push_back(Sprite(bomb));

	//BIRD
	Texture birdTex;
	Texture healedTex;
	Sprite bird;

	int hp = 10;

	std::vector<RectangleShape> hpBar;

	int k = 0;
	while (k < 10)
	{
		RectangleShape hpThick;
		hpThick.setFillColor(Color::Red);
		hpThick.setSize(Vector2f(20.f, 20.f));
		hpThick.setPosition((float)200.f + k * 20, 10.f);
		hpThick.setOutlineThickness(1.f);
		hpThick.setOutlineColor(Color::White);

		hpBar.push_back(hpThick);

		k++;
	}

	RectangleShape hpBarOut;
	hpBarOut.setSize(Vector2f(200.f, 20.f));
	hpBarOut.setPosition(200.f, 10.f);
	hpBarOut.setFillColor(Color(128, 139, 150, 255));
	hpBarOut.setOutlineThickness(3.f);
	hpBarOut.setOutlineColor(Color::White);

	if (!birdTex.loadFromFile("Textures/bird.png"))
		throw "Could not load bird.png!";

	if (!healedTex.loadFromFile("Textures/healedbird.png"))
		throw "Could not load healedbird.png!";

	bird.setTexture(birdTex);
	bird.setScale(Vector2f(0.15f, 0.15f));

	Font font;
	font.loadFromFile("Fonts/BRLNSR.TTF");

	Text pauseText;

	pauseText.setFont(font);
	pauseText.setCharacterSize(24);
	pauseText.setFillColor(Color::White);
	pauseText.setPosition(0, 0);
	pauseText.setString("PAUSED!");

	Text scoreText;

	scoreText.setFont(font);
	scoreText.setCharacterSize(24);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(0, 0);
	scoreText.setString("SCORE: 0");

	Text gameOverText;

	gameOverText.setFont(font);
	gameOverText.setCharacterSize(40);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setString("GAME OVER!");
	gameOverText.setPosition(300.f, 400.f);
	gameOverText.setOutlineThickness(1.f);
	gameOverText.setOutlineColor(sf::Color::White);


	int score = 0;

	int healthPackStatus = 0;

	Texture healthPackTex;
	Sprite healthPack;

	if (!healthPackTex.loadFromFile("Textures/healthpack.png"))
		throw "Could not load healthpack.png!";

	healthPack.setTexture(healthPackTex);
	healthPack.setScale(Vector2f(0.1f, 0.1f));

	bool isHit = false;
	int texToken = 0;
	const int damageTimerMax = 40;
	int damageTimer = 0;

	bool isHealed = false;
	int texToken2 = 0;
	const int healTimerMax = 40;
	int healTimer = 0;

	//GAME LOOP
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
				window.close();
			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space)
				pause = (pause == false) ? true : false;
		}

		if (hp > 0)
		{
			//UPDATE
			if (!pause)
			{
				//bird(PLAYER)
				bird.setPosition(bird.getPosition().x, Mouse::getPosition(window).y);

				if (bird.getPosition().y > window.getSize().y - bird.getGlobalBounds().height)
					bird.setPosition(bird.getPosition().x, window.getSize().y - bird.getGlobalBounds().height);

				if (bird.getPosition().y < 0)
					bird.setPosition(bird.getPosition().x, 0);

				// The explosion effect when hit bomb
				if (isHit)
				{
					if (damageTimer < damageTimerMax)
					{
						if (damageTimer % 10 == 0)
						{
							if (texToken == 0)
							{
								bird.setTexture(explosionTex);
								texToken = 1;
							}
							else
							{
								bird.setTexture(birdTex);
								texToken = 0;
							}
						}
						damageTimer++;
					}
					else
					{
						damageTimer = 0;
						isHit = false;
						bird.setTexture(birdTex);
					}
				}

				// The healing effect when get healthpack
				if (isHealed)
				{
					if (healTimer < healTimerMax)
					{
						if (healTimer % 10 == 0)
						{
							if (texToken2 == 0)
							{
								bird.setTexture(healedTex);
								texToken2 = 1;
							}
							else
							{
								bird.setTexture(birdTex);
								texToken2 = 0;
							}
						}
						healTimer++;
					}
					else
					{
						isHealed = false;
						healTimer = 0;
						bird.setTexture(birdTex);
					}
				}

				//BOMBS (ENEMIES)
				for (size_t i = 0; i < bombs.size(); i++)
				{
					bombs[i].move(-5.f, 0.f);

					if (bombs[i].getPosition().x < 0 - bomb.getGlobalBounds().width)
					{
						bombs.erase(bombs.begin() + i);
						score++;

						if (score % 5 == 0 && score != 0)
							healthPackStatus = 1;

						std::stringstream ss;
						ss << "Score: " << score;
						scoreText.setString(ss.str());
					}
				}

				if (healthPackStatus == 1)
				{
					healthPack.setPosition(window.getSize().x, rand() % int(window.getSize().y - healthPack.getGlobalBounds().height));
					healthPackStatus = 2;
				}
				else if (healthPackStatus == 2)
				{
					healthPack.move(-5.f, 0.f);

					if (bird.getGlobalBounds().intersects(healthPack.getGlobalBounds()))
					{
						healthPackStatus = 0;
						if (hp < 10)
						{
							hp++;
							RectangleShape newHpThick(hpBar[hpBar.size() - 1]);
							newHpThick.move(20.f, 0.f);
							hpBar.push_back(newHpThick);

							isHealed = true;
						}
					}
				}

				if (bombSpawnTimer < 40)
					bombSpawnTimer++;

				if (bombSpawnTimer >= 40)
				{
					bomb.setPosition(window.getSize().x, rand() % int(window.getSize().y - bomb.getGlobalBounds().height));
					bombs.push_back(Sprite(bomb));
					bombSpawnTimer = 0;
				}

				//COLLISION
				for (size_t i = 0; i < bombs.size(); i++)
				{
					if (bird.getGlobalBounds().intersects(bombs[i].getGlobalBounds()))
					{
						hp--;
						bird.setTexture(explosionTex);
						bombs.erase(bombs.begin() + i);
						hpBar.pop_back();

						isHit = true;
					}
				}

			}
		}


		//DRAW
		window.clear();

		window.draw(bird);

		for (size_t i = 0; i < bombs.size(); i++)
		{
			window.draw(bombs[i]);
		}

		if (healthPackStatus == 2)
			window.draw(healthPack);

		//UI
		window.draw(hpBarOut);

		for (auto& i : hpBar)
		{
			window.draw(i);
		}

		if (hp == 0)
		{
			window.draw(gameOverText);
		}

		if (pause)
			window.draw(pauseText);
		else
			window.draw(scoreText);


		window.display();
	}

	return 0;
}