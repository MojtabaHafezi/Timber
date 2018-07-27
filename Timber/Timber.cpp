
// Include important libraries here
#include "stdafx.h"
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;


//FUNCTION DECLARATIONS
void updateBranches(int seed);
const int NUMBERBRANCHES = 6;
Sprite branches[NUMBERBRANCHES];
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUMBERBRANCHES];


int main()
{
	Clock clock;
	//TIME BAR
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarStartHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarStartHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);
	Time gameTimeTotal;
	float timeRemaining = 8.0f;
	float timeBarWidthPerSec = timeBarStartWidth / timeRemaining;


	bool isBeeActive = false;
	bool isCloudActive1 = false;
	bool isCloudActive2 = false;
	bool isCloudActive3 = false;
	bool paused = true;

	float beeSpeed = 0.0f;
	float cloudSpeed1 = 0.0f;
	float cloudSpeed2 = 0.0f;
	float cloudSpeed3 = 0.0f;
	int score = 0;
	Text messageText;
	Text scoreText;


	//FONTS & TEXT
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");
	messageText.setFont(font);
	scoreText.setFont(font);

	messageText.setString("Please press ENTER to continue.");
	scoreText.setString("Score: " + score);

	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);

	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height / 2);
	messageText.setPosition(1920 / 2, 1080 / 2);

	scoreText.setPosition(20, 20);

	//BRANCHES
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	for (int i = 0; i < NUMBERBRANCHES; i++)
	{
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		branches[i].setOrigin(220, 20); //dead center
	}



	//TEXTURES AND SPRITES
	//Create a videomode object
	VideoMode vm(1920, 1080);
	//Create and open a window for the game RenderWindow
	RenderWindow window(vm, "Timber!", Style::Fullscreen);
	//Create textures which are saved on the GPU
	Texture textureBackground;
	Texture textureTree;
	Texture textureBee;
	Texture textureCloud;
	//Create a sprite
	Sprite spriteBackground;
	Sprite spriteTree;
	Sprite spriteBee;
	Sprite spriteCloud1, spriteCloud2, spriteCloud3;
	//Load graphics into the textures
	textureBackground.loadFromFile("graphics/background.png");
	textureTree.loadFromFile("graphics/tree.png");
	textureBee.loadFromFile("graphics/bee.png");
	textureCloud.loadFromFile("graphics/cloud.png");
	//Attach the textures to the sprites and set their position
	spriteBackground.setTexture(textureBackground);
	spriteBackground.setPosition(0, 0);

	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);

	// Clouds
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);

	//Player
	Texture texturePlayer;
	Sprite spritePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);
	side playerSide = side::LEFT;

	//Gravestone
	Texture textureRip;
	Sprite spriteRip;
	textureRip.loadFromFile("graphics/rip.png");
	spriteRip.setTexture(textureRip);
	spriteRip.setPosition(600, 860);

	//Axe
	Texture textureAxe;
	Sprite spriteAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	//Line the axe with the tree
	const float AXE_POS_LEFT = 700;
	const float AXE_POS_RIGHT = 1075;

	//Log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	bool acceptInput = false;

	//SOUND
	SoundBuffer chopBuffer;
	SoundBuffer deathBuffer;
	SoundBuffer timeBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	deathBuffer.loadFromFile("sound/death.wav");
	timeBuffer.loadFromFile("sound/out_of_time.wav");
	Sound chop;
	Sound death;
	Sound oot;
	chop.setBuffer(chopBuffer);
	death.setBuffer(deathBuffer);
	oot.setBuffer(timeBuffer);



	while (window.isOpen())
	{
		//HANDLE PLAYER INPUT

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				//accept input again and hide axe
				acceptInput = true;
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);

			}
		}


		//QUIT
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		//PLAY
		if (Keyboard::isKeyPressed(Keyboard::Return))
		{
			paused = false;
			score = 0;
			timeRemaining = 6;

			for (int i = 1; i < NUMBERBRANCHES; i++)
			{
				branchPositions[i] = side::NONE;
			}

			spriteRip.setPosition(675, 2000);
			spritePlayer.setPosition(580, 720);
			spriteAxe.setPosition(700, 830);

			acceptInput = true;

		}

		if (acceptInput)
		{
			//Handle right arrow input
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				playerSide = side::RIGHT;
				score++;

				timeRemaining += (2 / score) + 0.15f;
				spriteAxe.setPosition(AXE_POS_RIGHT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 700);
				chop.play();
				updateBranches(score);
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;
				acceptInput = false;
			
			}

			//Handle left arrow input
			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				playerSide = side::LEFT;
				score++;
				timeRemaining += (2 / score) + 0.15f;
				chop.play();
				spriteAxe.setPosition(AXE_POS_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);

				updateBranches(score);
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;
				

			}
		}

		//UPDATE SCENE

		if (!paused)
		{


			Time dt = clock.restart();
			//Change timebar accordingly
			timeRemaining -= dt.asSeconds();
			timeBar.setSize(Vector2f(timeBarStartWidth*timeRemaining, timeBarStartHeight));

			if (timeRemaining <= 0.0f)
			{
				//Pause game
				paused = true;
				messageText.setString("Out of time.");
				oot.play();
				//Reposition the text
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height / 2);
				messageText.setPosition(1920 / 2, 1080 / 2);

				
			}

			//set up bee
			if (!isBeeActive)
			{
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 100;

				srand((int)time(0) * 10);
				float height = (rand() % 800) + 500;
				spriteBee.setPosition(2000, height);
				isBeeActive = true;
			}
			else
			{
				//move bee
				spriteBee.setPosition(spriteBee.getPosition().x - (beeSpeed*dt.asSeconds()), spriteBee.getPosition().y);
				if (spriteBee.getPosition().x < -100)
				{
					isBeeActive = false;
				}
			}
			// Manage the clouds
			// Cloud 1
			if (!isCloudActive1)
			{
				// How fast is the cloud
				srand((int)time(0) * 10);
				cloudSpeed1 = (rand() % 200);
				// How high is the cloud
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);
				isCloudActive1 = true;
			}
			else
			{
				spriteCloud1.setPosition(spriteCloud1.getPosition().x + (cloudSpeed1* dt.asSeconds()), spriteCloud1.getPosition().y);
				if (spriteCloud1.getPosition().x > 1920)
				{
					isCloudActive1 = false;
				}
			}
			// Cloud 2
			if (!isCloudActive2)
			{
				// How fast is the cloud
				srand((int)time(0) * 20);
				cloudSpeed2 = (rand() % 200);
				// How high is the cloud
				srand((int)time(0) * 20);
				float height = (rand() % 300) - 150;
				spriteCloud2.setPosition(-200, height);
				isCloudActive2 = true;
			}
			else
			{
				spriteCloud2.setPosition(spriteCloud2.getPosition().x + (cloudSpeed2* dt.asSeconds()), spriteCloud2.getPosition().y);
				if (spriteCloud2.getPosition().x > 1920)
				{
					isCloudActive2 = false;
				}
			}
			// Cloud 3
			if (!isCloudActive3)
			{
				// How fast is the cloud
				srand((int)time(0) * 30);
				cloudSpeed3 = (rand() % 200);
				// How high is the cloud
				srand((int)time(0) * 30);
				float height = (rand() % 450) - 150;
				spriteCloud3.setPosition(-200, height);
				isCloudActive3 = true;
			}
			else
			{
				spriteCloud3.setPosition(spriteCloud3.getPosition().x + (cloudSpeed3* dt.asSeconds()), spriteCloud3.getPosition().y);
				if (spriteCloud3.getPosition().x > 1920)
				{
					isCloudActive3 = false;
				}
			}

			//Manage the text
			std::stringstream ss;
			ss << "Score: " << score;
			scoreText.setString(ss.str());

			//Update the branches
			for (int i = 0; i < NUMBERBRANCHES; i++)
			{
				float height = i * 150;
				if (branchPositions[i] == side::LEFT)
				{
					//Move the branch to the left and rotate
					branches[i].setPosition(610, height);
					branches[i].setRotation(180);
				}
				else
				{
					if (branchPositions[i] == side::RIGHT)
					{
						branches[i].setPosition(1330, height);
						branches[i].setRotation(0);
					}
					else
					{
						//Hide branches
						branches[i].setPosition(3000, height);
					}
				}
			}

			if (logActive)
			{
				spriteLog.setPosition(spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));
				//if the log reaches edge -> make it respawn
				if (spriteLog.getPosition().x > 2000 || spriteLog.getPosition().x <= -100)
				{
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			if (branchPositions[5] == playerSide)
			{
				//death
				paused = true;
				acceptInput = false;

				spriteRip.setPosition(spritePlayer.getPosition().x, spritePlayer.getPosition().y + 60);
				spritePlayer.setPosition(2000, 660);
				spriteAxe.setPosition(spriteAxe.getPosition().x, spriteRip.getPosition().y + 100);
				death.play();
				messageText.setString("YOU LOST!");
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2, textRect.top + textRect.height / 2);
				messageText.setPosition(1920 / 2, 1080 / 2);

				
			}

		}  //  end if (!paused)


		//DRAW GRAPHICS
		//clear last screen 
		window.clear();
		//Redraw
		window.draw(spriteBackground);
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);
		for (int i = 0; i < NUMBERBRANCHES; i++)
		{
			window.draw(branches[i]);
		}
		window.draw(spriteTree);
		window.draw(spritePlayer);
		window.draw(spriteAxe);
		window.draw(spriteLog);
		window.draw(spriteRip);
		window.draw(spriteBee);
		window.draw(scoreText);
		window.draw(timeBar);
		if (paused)
		{
			window.draw(messageText);
		}

		window.display();
	}


	return 0;
}

// Function definition
void updateBranches(int seed)
{
	// Move all the branches down one place
	for (int j = NUMBERBRANCHES - 1; j > 0; j--)
	{
		branchPositions[j] = branchPositions[j - 1];
	}
	//Spawn a new branch
	srand((int)time(0) + seed);
	int r = rand() % 5;
	switch (r)
	{
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;
	}
}