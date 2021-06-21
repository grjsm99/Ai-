#pragma once
#include <Windows.h>
#include <stack>
#include <list>
#include "Card.h"
#include "Field.h"
#include <atlimage.h>
#define WXSIZE 1280
#define WYSIZE 720

extern CImage imgCardSkin;

class JokerCard {
public:
	Card card;
	int mon;
	int num;
	JokerCard(Card c, int m, int n) : card(c), mon(m), num(n) {}
};

class Board {
public:
	CImage boardimage;
	CImage selectimage;
	CImage selgookimage;
	CImage gostopimage;
	CImage numimage;
	CImage moneyimage;
	CImage nummonimage;
	CImage clearimage;
	CImage aiportimage;
	CImage plportimage;
	CImage scoreimage;
	CImage goimage;
	stack<Card> decklist;
	list<Card> Layer[12];
	list<JokerCard> JLayer;
	Field* myField;
	Field* enField;
	int deckCnt, tempID, expdN, aniN;
	int layercnt;
	bool graySign[12] = { false, };
	bool viewCard;
	Board() : decklist(), Layer(), deckCnt(0), tempID(-4), expdN(0), aniN(0), layercnt(0), viewCard(false) {
		boardimage.Load(L"img\\board.bmp"); // ���� �̹���
		selectimage.Load(L"img\\select.bmp"); // ����â �̹���
		selgookimage.Load(L"img\\selgook.bmp"); // ��������â �̹���
		gostopimage.Load(L"img\\gostop.bmp"); // ���鼱��â �̹���
		numimage.Load(L"img\\num.bmp"); // ���� �̹���
		moneyimage.Load(L"img\\money.bmp"); // ���� �̹���
		nummonimage.Load(L"img\\nummon.bmp");
		clearimage.Load(L"img\\clear.png");
		aiportimage.Load(L"img\\aiport.bmp");
		plportimage.Load(L"img\\plport.bmp");
		scoreimage.Load(L"img\\score.bmp");
		goimage.Load(L"img\\goimg.bmp");

	};
	void printBoard(HDC& hdc); // �� ���
	void printLayer(HDC& hdc); // �ǿ� �� ī�� ���
	void makeDeck();
	void SetFirst();
	Card DrawDeck(int a = 0);
	void printdeck(HDC& hdc);
	void Judge(HDC& hdc, HWND hWnd);
	void ShowChoose(list<Card>, HDC&);
	void PickChoose(list<Card>&, bool);
	void rePrint(HWND, int);
	void StealCard();
	void Reset(int a);
	void makeDeckTest();
	int SendMoney();
	void ShowResult(bool);
	void Save();
};
