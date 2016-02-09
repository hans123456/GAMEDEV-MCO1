#include "SnakeGameScene.h"

USING_NS_CC;

Scene* SnakeGame::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = SnakeGame::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool SnakeGame::init()
{
	if (!Layer::init())
	{
		return false;
	}

	MAX_DIRECTIONS = 3;
	SNAKE_LENGTH = 3;
	GRID_SPACE = 30;

	fps = 1.0 / 60;
	spd = fps * 8;
	isPlaying = false;
	hasStarted = false;

	charactersSpriteFileNames[0] = "mario32.png";
	soundEffectsFileNames[0][0] = "audio/mario/mario_here_we_go.wav";
	soundEffectsFileNames[0][1] = "audio/mario/mario_yahoo.wav";
	soundEffectsFileNames[0][2] = "audio/mario/mario_mamma-mia.wav";

	charactersSpriteFileNames[1] = "luigi32.png";
	soundEffectsFileNames[1][0] = "audio/luigi/luigi_here_we_go.wav";
	soundEffectsFileNames[1][1] = "audio/luigi/luigi_yahoo.wav";
	soundEffectsFileNames[1][2] = "audio/luigi/luigi_mamma-mia.wav";

	charactersSpriteFileNames[2] = "wario32.png";
	soundEffectsFileNames[2][0] = "audio/wario/wario_here_i_go.wav";
	soundEffectsFileNames[2][1] = "audio/wario/wario_Eh_heaheaheaheah.wav";
	soundEffectsFileNames[2][2] = "audio/wario/wario_wahhhhhhh!.wav";

	charactersSize = 3;
	currentCharacterIndex = -1;

	soundEffectPressStartFileName = "audio/mario_thank_you.wav";

	audio = CocosDenshion::SimpleAudioEngine::getInstance();

	for (int i=0; i<charactersSize; i++)
	{
		for (int j=0; j<3; j++)
		{
			audio->preloadEffect(soundEffectsFileNames[i][j].c_str());
		}
	}

	audio->preloadEffect(soundEffectPressStartFileName.c_str());

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(SnakeGame::onKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	CCSprite * bg = CCSprite::create("bg.png");
	bg->setAnchorPoint(Vec2(0, 0));
	this->addChild(bg);

	snakeGameLabel = Label::createWithTTF("Snake Game.", "fonts/Super Plumber Brothers.ttf", 72);
	snakeGameLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2 + 50));
	snakeGameLabel->setColor(Color3B::BLACK);
	this->addChild(snakeGameLabel, 1);

	playGameLabel = Label::createWithTTF("Press enter to play.", "fonts/Super Plumber Brothers.ttf", 36);
	playGameLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		snakeGameLabel->getPositionY() - snakeGameLabel->getContentSize().height - 20));
	playGameLabel->setColor(Color3B::BLACK);
	this->addChild(playGameLabel, 1);

	exitGameLabel = Label::createWithTTF("Press esc to exit.", "fonts/Super Plumber Brothers.ttf", 24);
	exitGameLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		playGameLabel->getPositionY() - playGameLabel->getContentSize().height - 10));
	exitGameLabel->setColor(Color3B::BLACK);
	this->addChild(exitGameLabel, 1);

	gameOverLabel = Label::createWithTTF("Game Over.", "fonts/Super Plumber Brothers.ttf", 72);
	gameOverLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2 + 50));
	gameOverLabel->setColor(Color3B::BLACK);
	gameOverLabel->setVisible(false);
	this->addChild(gameOverLabel, 1);

	scoreLabel =  Label::createWithTTF("", "fonts/Super Plumber Brothers.ttf", 36);
	scoreLabel->setAnchorPoint(Vec2(0, 0.5));
	scoreLabel->setPosition(Vec2(origin.x + GRID_SPACE,
		origin.y + visibleSize.height - 50));
	scoreLabel->setColor(Color3B::BLACK);
	this->addChild(scoreLabel, 1);

	bottomRight = Vec2(visibleSize.width - GRID_SPACE, origin.y + GRID_SPACE);
	bottomLeft = Vec2(bottomRight.x - 480, bottomRight.y);
	topRight = Vec2(bottomRight.x, bottomRight.y + 480);
	topLeft = Vec2(bottomLeft.x, topRight.y);

	foodSprite = cocos2d::Sprite::create("food_sprite.png");
	foodSprite->setScale(0.8);
	foodSprite->retain();
	foodSprite->setVisible(false);
	this->addChild(foodSprite);

	gridNode = DrawNode::create();

	int tspace = GRID_SPACE / 2;

	h = 0;
	for (int i = 0, j = topRight.y - GRID_SPACE; i<=j; i += GRID_SPACE, h++)
	{
		if (i == 0 || i == j) 
			gridNode->drawLine(Vec2(bottomLeft.x, bottomLeft.y + i), Vec2(topRight.x, bottomLeft.y + i), Color4F::GRAY);
	}
	h--;

	w = 0;
	for (int i = 0, j = topRight.x - GRID_SPACE; i<=j; i += GRID_SPACE, w++)
	{
		if (i == 0 || i == j)
			gridNode->drawLine(Vec2(bottomLeft.x + i, bottomLeft.y), Vec2(bottomLeft.x + i, topRight.y), Color4F::GRAY);
	}
	w--;

	gridNode->retain();

	for (int i = 0; i<h; i++)
	{
		for (int j = 0; j<w; j++)
		{
			gridX[i][j] = bottomLeft.x + (j * GRID_SPACE) + tspace;
			gridY[i][j] = bottomLeft.y + (i * GRID_SPACE) + tspace;
		}
	}

	audio->playEffect(soundEffectPressStartFileName.c_str());
	return true;
}

void SnakeGame::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	switch (keyCode) {
	case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE:
		quitGame();
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_ENTER:
		if (!hasStarted)
		{
			startGame();
		}
		if (!isPlaying)
		{
			restartGame();
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		if (isPlaying)
		{
			if (directionPoll.size() < MAX_DIRECTIONS)
			{
				if (directionPoll.empty())
				{
					directionPoll.push_back(Directions::UP);
					break;
				}
				Directions last = directionPoll.back();
				if (last != Directions::UP && last != Directions::DOWN)
				{
					directionPoll.push_back(Directions::UP);
				}
			}
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		if (isPlaying)
		{
			if (directionPoll.size() < MAX_DIRECTIONS)
			{
				if (directionPoll.empty())
				{
					directionPoll.push_back(Directions::DOWN);
					break;
				}
				Directions last = directionPoll.back();
				if (last != Directions::UP && last != Directions::DOWN)
				{
					directionPoll.push_back(Directions::DOWN);
				}
			}
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		if (isPlaying)
		{
			if (directionPoll.size() < MAX_DIRECTIONS)
			{
				if (directionPoll.empty())
				{
					directionPoll.push_back(Directions::LEFT);
					break;
				}
				Directions last = directionPoll.back();
				if (last != Directions::LEFT && last != Directions::RIGHT)
				{
					directionPoll.push_back(Directions::LEFT);
				}
			}
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		if (isPlaying)
		{
			if (directionPoll.size() < MAX_DIRECTIONS)
			{
				if (directionPoll.empty())
				{
					directionPoll.push_back(Directions::RIGHT);
					break;
				}
				Directions last = directionPoll.back();
				if (last != Directions::LEFT && last != Directions::RIGHT)
				{
					directionPoll.push_back(Directions::RIGHT);
				}
			}
		}
		break;
	}
}

void SnakeGame::quitGame()
{
	if (isPlaying)
	{
		return;
	}
	cocos2d::Director::getInstance()->end();
}

void SnakeGame::startGame()
{
	hasStarted = true;
	hideMenu();
	this->addChild(gridNode);
}

void SnakeGame::restartGame()
{
	if (isPlaying)
	{
		return;
	}

	score = 0;
	currTime = 0;
	isPlaying = true;
	isFacingRight = true;
	tempHit = false;
	hideGameOver();
	memset(hasSnakePart, 0, sizeof(hasSnakePart));
	scoreLabel->setString("Score : " + std::to_string(score));
	hasDirection = false;
	currentCharacterIndex = (currentCharacterIndex + 1) % charactersSize;

	while (!snakeDrawNodes.empty()) 
	{
		cocos2d::Sprite *drawNode = snakeDrawNodes.back();
		snakeDrawNodes.pop_back();
		drawNode->removeFromParent();
		drawNode->autorelease();
	}

	snakePoints.clear();
	directionPoll.clear();

	int x = w / 2;
	int y = h / 2;

	for (int i = 0; i < SNAKE_LENGTH; i++)
	{
		cocos2d::Vec2 index = Vec2(x, y);
		cocos2d::Sprite *sprite;

		if (i == SNAKE_LENGTH-1)
		{
			sprite = cocos2d::Sprite::create(charactersSpriteFileNames[currentCharacterIndex]);
		}
		else
		{
			sprite = cocos2d::Sprite::create("goomba32.png");
		}

		sprite->setScale(0.9375);
		sprite->retain();
		sprite->setPosition(gridX[y][x], gridY[y][x]);
		snakeDrawNodes.push_back(sprite);
		snakePoints.push_back(index);
		this->addChild(sprite);
		hasSnakePart[y][x] = true;
	}

	generateFood();

	audio->playEffect(soundEffectsFileNames[currentCharacterIndex][0].c_str());

	this->schedule(schedule_selector(SnakeGame::update));

}

void SnakeGame::hideGameOver()
{
	gameOverLabel->setVisible(false);
	playGameLabel->setVisible(false);
	exitGameLabel->setVisible(false);
}

void SnakeGame::showGameOver()
{
	gameOverLabel->setVisible(true);
	playGameLabel->setVisible(true);
	exitGameLabel->setVisible(true);
}

void SnakeGame::hideMenu()
{
	snakeGameLabel->setVisible(false);
	playGameLabel->setVisible(false);
	exitGameLabel->setVisible(false);
}

void SnakeGame::showMenu()
{
	snakeGameLabel->setVisible(true);
	playGameLabel->setVisible(true);
	exitGameLabel->setVisible(true);
}

void SnakeGame::update(float delta)
{
	currTime += delta;
	if (currTime > spd)
	{
		currTime = 0;
		updateSnake();
		scoreLabel->setString("Score : " + std::to_string(score));
	}
}

void SnakeGame::updateSnake()
{
	if (!directionPoll.empty())
	{
		Directions *prevDirection = &currDirection;
		currDirection = directionPoll.front();
		hasDirection = true;

		if (currDirection == Directions::RIGHT)
		{
			isFacingRight = true;
		}
		else if (currDirection == Directions::LEFT)
		{
			isFacingRight = false;
		}

		if (directionPoll.size() > 1)
		{
			directionPoll.pop_front();
		}
	}

	if (!hasDirection)
	{
		return;
	}

	cocos2d::Vec2 prevHeadPoint = snakePoints.front();
	int x = prevHeadPoint.x;
	int y = prevHeadPoint.y;

	switch (currDirection) 
	{
	case Directions::UP :
		y++;
		break;
	case Directions::DOWN :
		y--;
		break;
	case Directions::LEFT :
		x--;
		break;
	case Directions::RIGHT :
		x++;
		break;
	default :
		return;
		break;
	}

	Vec2 nextPoint = Vec2(x, y);
	bool hitThySelf = hasSnakePart[y][x];

	if (x >= w || x < 0 || y >= h || y < 0 || hitThySelf)
	{
		if (tempHit) {
			isPlaying = false;
			audio->playEffect(soundEffectsFileNames[currentCharacterIndex][2].c_str());
			this->unschedule(schedule_selector(SnakeGame::update));
			showGameOver();
		}
		tempHit = true;
		return;
	}
	else
	{
		tempHit = false;
	}

	hasSnakePart[y][x] = true;

	cocos2d::Sprite *prevHeadDrawNode = snakeDrawNodes.front();
	snakeDrawNodes.pop_front();
	prevHeadDrawNode->removeFromParent();
	prevHeadDrawNode->autorelease();

	prevHeadDrawNode = cocos2d::Sprite::create("goomba32.png");
	prevHeadDrawNode->setScale(0.9375);
	prevHeadDrawNode->retain();
	prevHeadDrawNode->setPosition(gridX[(int) prevHeadPoint.y][(int) prevHeadPoint.x], 
		gridY[(int) prevHeadPoint.y][(int) prevHeadPoint.x]);
	snakeDrawNodes.push_front(prevHeadDrawNode);
	this->addChild(prevHeadDrawNode);

	if (x != foodPosX || y != foodPosY)
	{
		cocos2d::Sprite *tailDrawNode = snakeDrawNodes.back();
		snakeDrawNodes.pop_back();
		tailDrawNode->removeFromParent();
		tailDrawNode->autorelease();

		cocos2d::Vec2 tailPoint = snakePoints.back();
		snakePoints.pop_back();

		hasSnakePart[(int) tailPoint.y][(int) tailPoint.x] = false;
	}
	else
	{
		audio->playEffect(soundEffectsFileNames[currentCharacterIndex][1].c_str());
		generateFood();
		score++;
	}

	cocos2d::Vec2 index = Vec2(x, y);
	cocos2d::Sprite *newHead = cocos2d::Sprite::create(charactersSpriteFileNames[currentCharacterIndex]);
	newHead->setScale(0.9375);
	newHead->retain();
	newHead->setPosition(gridX[y][x], gridY[y][x]);

	if (!isFacingRight) 
	{
		newHead->setScaleX(-1);
	}

	snakeDrawNodes.push_front(newHead);
	snakePoints.push_front(index);
	this->addChild(newHead);
	hasSnakePart[y][x] = true;

}

void SnakeGame::generateFood()
{
	foodPosX = -1;
	foodPosY = -1;
	foodSprite->setVisible(false);
	do
	{
		foodPosX = rand() % w;
		foodPosY = rand() % h;
	} while (hasSnakePart[foodPosY][foodPosX]);
	foodSprite->setPositionX(gridX[foodPosY][foodPosX]);
	foodSprite->setPositionY(gridY[foodPosY][foodPosX]);
	foodSprite->setVisible(true);
}