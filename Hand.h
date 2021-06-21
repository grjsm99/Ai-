#pragma once
#include <Windows.h>
#include "Card.h"
#include "Field.h"

class Hand {
public:
	int x, y;
	Card card[10];
	int cardNum = 0;
	bool ai;
	Field* myField;
	Field* enField;

	Hand(bool _i) : x(680), y(_i ? 10 : 535), ai(_i) {};
	void Print(HDC& hdc);
	int ClickedCard(int _x, int _y);	//Ŭ���� ī���� �ε��� ��ȯ( �ƹ��͵� Ŭ���ȵ��� �� -1 ��ȯ )
	int Pick(int index);
	void ThrowCard(int index);
	void Insert(Card cardins);
	int AiPick();
	bool AiLR(list<Card>);

};