#pragma once
#include <Windows.h>
#include <list>
#include "Card.h"
#include <cstdlib>
class Field {
public:
	bool ai;
	int x, y;
	int score = 0;
	int go = 0;
	int goscore = 6;
	int mulcnt;
	int Money;
	list<Card> light;
	list<Card> ten;
	list<Card> band;
	list<Card> skin;
	Field(bool _ai) : ai(_ai), x(0), y(ai ? -100 : 420), mulcnt(0), Money(5400000) 
	{

	}
	
	void Print(HDC& hdc);
	void PrintScore(HDC& hdc);
	void ScoreCalcul();	//점수 계산하는 함수
	void Input(Card card, int ani = 0);
	bool GoStop();
	bool AiGoThink();
};