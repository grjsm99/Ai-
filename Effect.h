#pragma once
#include <Windows.h>
#include <atlimage.h>

using namespace std;

class Effect {
public:
	CImage* myImage;
	int x, y, hp, div, imageN;
	float size;
	unsigned char alpha;
	char nowframe;
	Effect(int _x, int _y, CImage* _myImage, int _fps, int _imageN) {
		x = _x; y = _y;
		hp = _fps * _imageN;
		div = hp / _imageN;
		size = 1.0f;
		alpha = 255;
		nowframe = 1;
		myImage = _myImage - 1;
		imageN = _imageN;
	}

	void Print(HDC& hdc);
	bool DieCheck();
	void NextFrame();
};