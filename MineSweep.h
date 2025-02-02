﻿/*
 * @Author: junyu33 
 * @Date: 2021-12-05 12:03:58 
 * @Last Modified by:   junyu33 
 * @Last Modified time: 2021-12-05 12:03:58 
 */

#ifndef _MINESWEEP_H_
#define _MINESWEEP_H_

#ifdef UNICODE
#undef UNICODE
#endif

//_CRT_SECURE_NO_DEPRECATE
#pragma warning(disable: 4996)

// <<<<<<< HEAD
// #ifndef _CRT_SECURE_NO_DEPRECATE
// #define _CRT_SECURE_NO_DEPRECATE
// #endif
// 
// #include <windows.h>
// #ifdef _CRT_DEPRECATE_TEXT
// #undef _CRT_DEPRECATE_TEXT
// #define _CRT_DEPRECATE_TEXT(...) 
// #endif
// 
// =======

#include "graphics.h"

#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <vector>
#include <string>
#include <cmath>

#pragma comment(lib, "Winmm.lib")


#define IMG_SIZE			35
#define DEFAULT_MINE_WIDTH	20
#define DEFAULT_MINE_HEIGHT	15

#define BUFFERSIZE 1024

#define PIXEL2MAP(x,y) x = x / IMG_SIZE * IMG_SIZE; y = y / IMG_SIZE * IMG_SIZE
#define PIXEL2BLOCK(x, y) x /= IMG_SIZE; y /= IMG_SIZE
#define DELAY_FPS			(60/7)

#define RANDF	((rand() % 1000 / 1000.0 + 0.2) * pow(-1.0f,rand()%10))

enum BlockType{NORMAL, CHOSEN, FLAG, FLAG_CHOSEN, FLAG_MINE, WRONGFLAG, UNKNOWN, UNKNOWN_CHOSEN, WRONGQMARK,SPACE, UNKNOWN_MINE, ONFIRE, FOUND, MARKEDMINE, MINE};
enum Message{msgSWEEP, msgGUESS, msgSEARCH, msgENDSEARCH, msgKEYDOWN, msgKEYUP, msgMOVE};
enum DIRECTIONS {dirREPLAY, dirSET, dirEXIT, dirWIN, dirLOSE, dirEASY, dirNORM, dirHARD};//ver3.0

class BGManager
{
public:
	BGManager();
	~BGManager();

	void playStart();
	void playClick();
	void playRightClick();
	void playSearch();
	void playMove();
	void playBomb();
	void playWin();

	void cleanBomb();

private:
	int m_bombCnt;
	char* m_bomb;
};

class Block
{
public:
	Block() : m_x(0), m_y(0), m_type(NORMAL), m_num(0)/*, isSwept(false)*/ {}
	Block(int x, int y) : m_x(x), m_y(y), m_type(NORMAL), m_num(0) {}
	virtual void render();
	virtual bool isMine() { return false; }
	virtual void addMine() { ++m_num; }
	virtual int getNum() { return m_num; }
	virtual void setFire() {}
	void getPos(int &x, int &y) { x = m_x; y = m_y; }
	BlockType getType() { return m_type; }
	void setType(BlockType type) { m_type = type; }
	bool canBeSearched() { return m_type == FOUND || m_type == FLAG ? false : true; }
	static PIMAGE *imgBlock, *imgMine, *imgFlag, *imgUnknown, *imgSpace;
	int m_num;
protected:
	int m_x, m_y;
	BlockType m_type;
};

class Mine : public Block
{
public:
	Mine() : m_frame(0), m_f(0), m_dx(0.0f), m_dy(0.0f), m_mx(0.0f), m_my(0.0f) {}
	Mine(int x, int y);
	void render();
	bool isMine() { return true; }
	void addMine() {}
	void setFire();
	int getNum() { return -1; }
	static PIMAGE *imgBlock, *imgMine, *imgFlag, *imgUnknown, *imgSpace;
	static int bomb;
private:
	void m_frameUpdate();
	void m_go();
	float m_dx, m_dy;
	float m_mx, m_my;
	int m_frame, m_f;
};

class MineArray
{
public:
	MineArray(): m_width(0), m_height(0), m_firedX(0), m_firedY(0), m_keyCtrl(KEYFREE), m_block(NULL), m_imgBlock(NULL)
	, m_imgMine(NULL), m_imgFlag(NULL), m_imgUnknown(NULL), m_imgSpace(NULL), m_blockNum(0) {}


	enum KeyState{ KEYDOWN, KEYFREE, SEARCHING };

	void loadResource();
	int minecnt; // ver 2.1
	int timestp; // ver 2.1
	//int cntmine(int width,int height); // ver 2.1
	bool init(int width, int height, int x, int y, int num);
	bool reInit(int width, int height, int x, int y, int num);
	bool win() { return m_blockNum <= 0; }
	bool sweep(int x, int y);
	void keyDown(int, int);
	void guess(int, int);
	void moveFrame(int, int, bool);
	void searchFrame(int x, int y);
	void flagall(int x,int y);//ver 3.1
	bool search(int, int);
	void render();
	void winFrame();
	void loseFrame();
	~MineArray();
private:
	void randomize(bool** b, int width, int height, int num, int , int);
	void initBlocks(bool**);
	void findMore(int, int);
	void markAll();
	int m_width, m_height;
	int m_firedX, m_firedY;
	int m_blockNum;
	KeyState m_keyCtrl;
	Block*** m_block;
	PIMAGE *m_imgBlock, *m_imgMine, *m_imgFlag, *m_imgUnknown, *m_imgSpace;
};

#endif