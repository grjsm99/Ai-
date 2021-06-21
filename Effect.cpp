#include "Effect.h"

void Effect::Print(HDC& hdc) {
	BITMAP bmp; //--- DDB 포맷의 비트맵 이미지 타입
	int bW, bH;
	GetObject(*myImage, sizeof(BITMAP), &bmp);
	bW = bmp.bmWidth;
	bH = bmp.bmHeight;
	//myImage->Draw(hdc, x - bW/2 * size, y - bH/2 * size, bW * size, bH * size, 0, 0, bW, bH);
	myImage->AlphaBlend(hdc, x - bW / 2 * size, y - bH / 2 * size, bW * size, bH * size, 0, 0, bW, bH, alpha, 0);

	if (imageN == 1) {
		if (alpha > (int)(150 / hp)) alpha -= (int)(150 / hp);
		else alpha = 0;
		size *= 1.01f;
	}
}

bool Effect::DieCheck() {
	hp -= 1;
	if (hp <= 0) {
		return true;
	}
	return false;
}

void Effect::NextFrame() {
	if (myImage + 1 != NULL) myImage++;
}