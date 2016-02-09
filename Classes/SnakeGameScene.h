#ifndef __SNAKEGAME_SCENE_H__
#define __SNAKEGAME_SCENE_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "list"
#include "algorithm"
#include "cstring"
#include "cstdlib"
#include "string"

class SnakeGame : public cocos2d::Layer
{
public:

	static cocos2d::Scene* createScene();
	virtual bool init();

	CREATE_FUNC(SnakeGame);

private:

	enum class Directions { UP, DOWN, LEFT, RIGHT };

	float fps;
	float spd;
	float currTime;
	bool hasStarted, isPlaying, tempHit;
	int gridX[100][100];
	int gridY[100][100];
	int hasSnakePart[100][100];
	int w, h;
	int score;

	int MAX_DIRECTIONS;
	int SNAKE_LENGTH;
	int GRID_SPACE;

	cocos2d::Sprite *foodSprite;
	int foodPosX;
	int foodPosY;

	Directions currDirection;
	bool hasDirection;
	bool isFacingRight;
	
	std::string charactersSpriteFileNames[100];
	int charactersSize;
	int currentCharacterIndex;

	std::string soundEffectsFileNames[100][100];
	std::string soundEffectPressStartFileName;
	CocosDenshion::SimpleAudioEngine *audio;

	std::list<cocos2d::Sprite *> snakeDrawNodes;
	std::list<cocos2d::Vec2> snakePoints;
	std::list<Directions> directionPoll;

	cocos2d::Vec2 bottomRight, bottomLeft, topLeft, topRight;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::DrawNode *gridNode;
	cocos2d::CCTMXTiledMap *_tileMap;
	cocos2d::CCTMXLayer *_background;
	
	cocos2d::Label *snakeGameLabel;
	cocos2d::Label *playGameLabel;
	cocos2d::Label *exitGameLabel;
	cocos2d::Label *gameOverLabel;
	cocos2d::Label *scoreLabel;

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*);
	void quitGame();
	void startGame();
	void restartGame();
	void hideGameOver();
	void showGameOver();
	void showMenu();
	void hideMenu();
	void update(float);
	void updateSnake();
	void generateFood();

};

#endif // __SNAKEGAME_SCENE_H__
