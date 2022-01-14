#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <windows.h>
#include <iostream>
#include <sstream>

#define GRIDSIZE 35
#define STAGE_WIDTH 10
#define STAGE_HEIGHT 20
#define P1_STAGE_CORNER_X 156
#define P1_STAGE_CORNER_Y 174
#define P2_STAGE_CORNER_X 844
#define P2_STAGE_CORNER_Y 174
#define P1_NXET_CORNER_X 587
#define P1_NXET_CORNER_Y 125
#define P2_NXET_CORNER_X 702
#define P2_NXET_CORNER_Y 125
#define DALAYVALUE 0.3
#define HOLD_CORNER_X 660
#define HOLD_CORNER_Y 275



typedef enum PLAYROLE {
	roleNONE, //空
	rolePLAYER1, //玩家1
	rolePLAYER2, //玩家2
};


using namespace sf;

class Tetris
{
public:
	Tetris();

	int gridSize;//块大小（18）
	int imgBGno, imgSkinNo;
	Texture* tTiles;
	Texture tBackground, tButtons, tNum, tTimer, tCounter, tGameOver;//创建纹理对象
	Sprite sBackground, sTiles, sButtons, sNum, sTimer, sCounter, sGameOver;//创建精灵对象
	int Field[STAGE_HEIGHT][STAGE_WIDTH] = { 0 };
	Vector2i currentSquare[4], nextSquare[4], tempSquare[4], shadowSquare[4];
	int role;
	Vector2i mCornPoint, nextSquareCornPoint, holdSquareCornPoint;//游戏区域位置
	int colorNum, nextcolorNum, tempcolorNum;
	int currentShapeNum, nextShapeNum, tempShapeNum;
	bool rotate, hold, hardDrop, newShapeFlag, animationFlag;
	bool gameOver;
	int animationRow[4];
	float animationCtrlValue;
	
	int score;
	int Figures[7][4] =
	{
	3,5,1,7, // I
	4,5,2,7, // S
	4,5,3,6, // Z
	5,3,4,7, // T
	5,3,2,7, // L
	5,7,3,6, // J
	2,3,4,5  // O
	};

	typedef enum gridShape {
		shapeI, // I
		shapeS, // S
		shapeZ, // Z
		shapeT, // T
		shapeL, // L
		shapeJ, // J
		shapeO, // O
	};

	int b7array[7] = { 0 }, b7Int;
	int Bag7();
	int dx;
	float timer, delay;

	static int holdcolorNum, holdShapeNum;
	static Vector2i holdSquare[4];

	void traditionLogic();
	void clearLine();
	void newShapeFunc();
	void animationFunc(int i, sf::RenderWindow* window);
	void slowLoading();
	void hardDropFunc();
	void shadow();
	void Logic();
	void xMove();
	void rotateFunc();
	void checkLine();
	void yMove();
	bool hitTest();
	void holdFunc();


	void isWin();
	void Initial(Texture* tex);
	void Input(sf::Event event);
	void Draw(sf::RenderWindow* window);

};

