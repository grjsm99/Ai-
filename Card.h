#pragma once
#include <Windows.h>
#include <string>
#define CARDW 59
#define CARDH 90

using namespace std;

class Card {
public:
	int id;	//카드의 고유 번호
	string type;	//카드의 종류(광, 피, 열끗, 띠)
	int multiple;
	
	Card() {};
	Card(int _id, string _type, int _multiple) {
		id = _id;
		type = _type;
		multiple = _multiple;
	}

	void Print(HDC& hdc, int _x, int _y, int _w, int _h);
	void PrintAni(HDC& hdc, int _x, int _y, int _w, int _h, int a = 0);
};