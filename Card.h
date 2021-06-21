#pragma once
#include <Windows.h>
#include <string>
#define CARDW 59
#define CARDH 90

using namespace std;

class Card {
public:
	int id;	//ī���� ���� ��ȣ
	string type;	//ī���� ����(��, ��, ����, ��)
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