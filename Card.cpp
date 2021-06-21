#include <atlimage.h>
#include "Card.h"

extern CImage imageCard;
extern bool isAni;
extern Card aniCard;
extern CImage imgCardSkin;
void Card::Print(HDC& hdc, int _x, int _y, int _w, int _h) {
	if(aniCard.id != id)
		imageCard.Draw(hdc, _x, _y, _w, _h, (id % 8) * (CARDW + 4), (id / 8) * (CARDH + 4), CARDW, CARDH);
}
void Card::PrintAni(HDC& hdc, int _x, int _y, int _w, int _h, int a) {
	if (a == 0) imageCard.Draw(hdc, _x, _y, _w, _h, (id % 8) * (CARDW + 4), (id / 8) * (CARDH + 4), CARDW, CARDH);
	else imgCardSkin.Draw(hdc, _x, _y, _w, _h, 0, 0, CARDW, CARDH);
} 