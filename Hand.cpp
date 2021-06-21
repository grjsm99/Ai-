#include <atlimage.h>
#include "Hand.h"
#include "board.h"
#include "Effect.h"
extern CImage imgCardSkin;
extern CImage imgBombCard;
extern CImage cardLabel;
extern Board board;
extern bool isAni;
extern POINTFLOAT start, dest, now;
extern Card aniCard;
extern HWND* hWndptr;
extern list<Effect> effects;
extern CImage expd[];
extern int cymode;

void Hand::Print(HDC& hdc) {
	
	if (ai && !board.viewCard) {
		for (int i = 0; i < cardNum; ++i) {
			imgCardSkin.Draw(hdc, x + (i % 5) * (CARDW + 1), y + (i / 5) * (CARDH + 1), CARDW, CARDH, 0, 0, CARDW, CARDH);
		}
	}
	else 
	 {
		for (int i = 0; i < cardNum; ++i) {
			if (card[i].id != -2) {
				card[i].Print(hdc, x + (i % 5) * (CARDW + 1), y + (i / 5) * (CARDH + 1), CARDW, CARDH);
				if (board.Layer[card[i].id / 4].size() > 0) {
					// 폭탄인지 확인
					int sameMonthNum = 0;
					for (int j = 0; j < cardNum; ++j) {
						if (card[i].id / 4 == card[j].id / 4 && card[j].id >= 0)
							++sameMonthNum;
					}

					if (cymode == 0) {
						if (sameMonthNum == 3) {	//폭탄일때
							cardLabel.TransparentBlt(hdc, x + (i % 5) * (CARDW + 1), y + (i / 5) * (CARDH + 1), 24, 22, 24, 0, 12, 11, RGB(255, 0, 255));
						}
						else {
							if (board.graySign[card[i].id / 4]) {
								cardLabel.TransparentBlt(hdc, x + (i % 5) * (CARDW + 1), y + (i / 5) * (CARDH + 1), 24, 22, 0, 0, 12, 11, RGB(255, 0, 255));//회색
							}
							else {
								cardLabel.TransparentBlt(hdc, x + (i % 5) * (CARDW + 1), y + (i / 5) * (CARDH + 1), 24, 22, 12, 0, 12, 11, RGB(255, 0, 255));//파란색
							}

						}
					}
				}
			}
			else {
				imgBombCard.Draw(hdc, x + (i % 5) * (CARDW + 1), y + (i / 5) * (CARDH + 1), CARDW, CARDH);
			}
		}
	}
}

int Hand::ClickedCard(int _x, int _y) {
	_x -= x;
	_y -= y;

	for (int i = 0; i < cardNum; ++i) {
		if ((i % 5) * (CARDW + 1) <= _x && _x <= (i % 5 + 1) * (CARDW + 1) && (i / 5) * (CARDH + 1) <= _y && _y <= (i / 5 + 1) * (CARDH + 1)) {
			return i;
		}
	}

	return -1;
}

int Hand::Pick(int index) {

	if (index == -1) return -1;
	int selectID = card[index].id;
	//  폭탄뒤집기 카드인지 확인
	if (selectID == -2) {
		ThrowCard(index);
		return -2;
	}
	if (selectID / 4 == 12) { // 조커일때
		myField->skin.push_back(card[index]);
		for (int i = index; i < cardNum - 1; ++i) {
			card[i] = card[i + 1];
		}
		cardNum--;
		isAni = true;
		aniCard.id = selectID;
		start = { (FLOAT)(x + (index % 5) * (CARDW + 1)) , (FLOAT)(y + (index / 5) * (CARDH + 1)) };
		dest = { (FLOAT)(myField->x + 420 + ((myField->skin.size()) % 5) * 25), (FLOAT)(myField->y + 210 - ((myField->skin.size()) / 5) * 30) };
		now = start;
		MSG Message;
		while (GetMessage(&Message, 0, 0, 0))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
			if (Message.message == WM_CLOSE) { SendMessage(*hWndptr, WM_DESTROY, 0, 0); break; }
			if (isAni == false) {	
				break;
			}
		}
		if(!ai) card[cardNum] = board.DrawDeck(1);
		else card[cardNum] = board.DrawDeck(2);
		cardNum++;
		return selectID;

	}

	// 폭탄인지 확인
	int sameMonthNum = 0;
	for (int i = 0; i < cardNum; ++i) {
		if (card[i].id / 4 == selectID / 4 && card[i].id >= 0) {
			++sameMonthNum;
		}
	}
	if (sameMonthNum == 3) {
		myField->mulcnt++;
		if (board.Layer[selectID / 4].size() == 1) {
			//폭탄
			board.expdN = 3;
			for (int i = 0; i < cardNum; ++i) {
				if (card[i].id / 4 == selectID / 4) {
					ThrowCard(i--);
				}
			}
			card[cardNum].id = -2;
			card[cardNum+1].id = -2;
			cardNum += 2;
			
		}
		else {
			//흔들기
			ThrowCard(index);
		}
		return selectID;
	}
	// 일반
	ThrowCard(index);
	return selectID;
}

void Hand::ThrowCard(int index) {
	int selectID = card[index].id;

	//

	if (selectID >= 0 && selectID / 4 < 12) {
		if (board.Layer[selectID / 4].size() > 0 && board.Layer[selectID / 4].size() < 3) board.aniN = 1;
		else if (board.Layer[selectID / 4].size() == 3) board.aniN = 4;
		else  board.aniN = 2;
		board.Layer[selectID / 4].push_back(card[index]);
		isAni = true;
		aniCard = card[index];
		start = { (FLOAT)(x + (index % 5) * (CARDW + 1)), (FLOAT)(y + (index / 5) * (CARDH + 1)) };
		dest = { (FLOAT)(50 + (selectID / 4) % 6 * 140 + (board.Layer[selectID / 4].size() - 1) * 20) + (selectID / 4 % 6) / 3 * 100, (FLOAT)(230 + selectID / 4 / 6 * 120 + (board.Layer[selectID / 4].size() - 1) * 20) };
		now = start;
		MSG Message;

		while (GetMessage(&Message, 0, 0, 0))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
			if (Message.message == WM_CLOSE) { SendMessage(*hWndptr, WM_DESTROY, 0, 0); break; }
			if (isAni == false) {
				break;
			}
		}
		
	}
	for (int i = index; i < cardNum - 1; ++i) {
		card[i] = card[i + 1];
	}
	--cardNum;
}

void Hand::Insert(Card cardins) {
	card[cardNum] = cardins;
	cardNum++;
}
int Hand::AiPick() {
	//없음 < 뒤집기카드 < 조커(상대 피x) < 폭탄(상대 피x) < 회색 < 파란색 < 조커(상대 피0) < 폭탄(상대피 0)
	//   0     100              200			  300	        400     500			700				800

	float priority[10] = { 0, };
	float mul = 1.0f;
	Card* canEatCard[2] = { 0, 0 };
	int canEatPriority[2] = { 0, 0 };

	for (int i = 0; i < cardNum; ++i) {
		if (card[i].id == -2) {	//뒤집기 카드
			priority[i] = 100;
		}
		else if (card[i].id / 4 == 12) {	//조커카드
			if (board.enField->skin.size() > 0) {
				priority[i] = 700;
			}
			else {
				priority[i] = 200;
			}
		}
		else {	//일반카드
			if (board.Layer[card[i].id / 4].size() > 0) {	//먹을수 있는 카드가 있는경우
				canEatCard[0] = &(*board.Layer[card[i].id / 4].begin());
				if (board.Layer[card[i].id / 4].size() == 2) { 
					canEatCard[1] = &(*(++board.Layer[card[i].id / 4].begin()));
				}

				// 폭탄인지 확인
				int sameMonthNum = 0;
				for (int j = 0; j < cardNum; ++j) { 
					if (card[i].id / 4 == card[j].id / 4 && card[j].id >= 0)
						++sameMonthNum;
				}

				
					if (sameMonthNum == 3) {	//폭탄일때
						if (board.enField->skin.size() > 0) {
							priority[i] = 800;
						}
						else {
							priority[i] = 300;
						}

					}
					else {
						if (board.graySign[card[i].id / 4]) {
							priority[i] = 400;//회색
							
						}
						else {
							priority[i] = 500;//파란색
						}

					}
				

				mul = 1.0f;
			}
			else {
				mul = -1.0f;
			}

			//타입에 따라 점수주기
			if (card[i].type == "피") {
				if (card[i].multiple == 0)
					priority[i] += 1 * mul;
				else
					priority[i] += 2 * mul;
			}
			else if (card[i].type == "광")
				priority[i] += 2 * mul;
			else if (card[i].type == "띠")
				priority[i] += 1.2 * mul;
			else if (card[i].type == "열끗")
				priority[i] += 1.1 * mul;

			//먹을수 있는 카드의 타입에 따라 점수주기
			for (int k = 0; k < 2; ++k) {
				if (canEatCard[k] != 0) {
					if (canEatCard[k]->type == "피") {
						if (canEatCard[k]->multiple == 0)
							canEatPriority[k] += 1 * mul;
						else
							canEatPriority[k] += 2 * mul;
					}
					else if (canEatCard[k]->type == "광")
						canEatPriority[k] += 2 * mul;
					else if (canEatCard[k]->type == "띠")
						canEatPriority[k] += 1.2 * mul;
					else if (canEatCard[k]->type == "열끗")
						canEatPriority[k] += 1.1 * mul;
				}
			}
			priority[i] += canEatPriority[0] > canEatPriority[1] ? canEatPriority[0] : canEatPriority[1];
		}
	}

	//우선순위(점수)가 가장 높은 패를 낸다.
	int selectIndex = 0;
	float maxPriority = -9999;
	for (int i = 0; i < cardNum; ++i) {
		if (maxPriority < priority[i]) {
			maxPriority = priority[i];
			selectIndex = i;
		}
	}
	return selectIndex;
}
bool Hand::AiLR(list<Card> Layer) {
	auto it = Layer.begin();
	Card left = *it;
	++it;
	Card right = *it;
	float score[5] = { 0.8, 0.4, 0.6, 0,2 };
	float lsc, rsc = 0;
	if (myField->light.size() >= 2)  score[0] = myField->light.size() + 1;
	if (myField->band.size() >= 4)  score[1] = 1.2;
	if (myField->ten.size() >= 4)  score[2] = 1.4;
	if (myField->skin.size() >= 9) score[3] = 1;
	if (left.type == "광") lsc = score[0];
	else if (left.type == "띠") lsc = score[1];
	else if (left.type == "열끗") lsc = score[2];
	else if (left.type == "피") lsc = score[3];
	
	if (right.type == "광") rsc = score[0];
	else if (right.type == "띠") rsc = score[1];
	else if (right.type == "열끗") rsc = score[2];
	else if (right.type == "피") rsc = score[3];

	//홍단 || 청단  || 초단 상대가 가지고 있는지 확인
	int enRed = 0;
	int enBlue = 0;
	int enGreen = 0;
	int myRed = 0;
	int myBlue = 0;
	int myGreen = 0;
	int myBird = 0;
	int enBird = 0;
	for (auto it = enField->band.begin(); it != enField->band.end(); ++it) {
		if (it->id == 1 || it->id == 5 || it->id == 9)
			enRed++;
		if (it->id == 21 || it->id == 33 || it->id == 37)
			enBlue++;
		if (it->id == 13 || it->id == 17 || it->id == 25)
			enGreen++;
	}
	for (auto it = enField->ten.begin(); it != enField->ten.end(); ++it) {
		if (it->id == 4 || it->id == 14 || it->id == 29)
			enBird++;
	}
	for (auto it = myField->band.begin(); it != myField->band.end(); ++it) {
		if (it->id == 1 || it->id == 5 || it->id == 9)
			myRed++;
		if (it->id == 21 || it->id == 33 || it->id == 37)
			myBlue++;
		if (it->id == 13 || it->id == 17 || it->id == 25)
			myGreen++;;
	}
	for (auto it = myField->ten.begin(); it != myField->ten.end(); ++it) {
		if (it->id == 4 || it->id == 14 || it->id == 29)
			myBird++;
	}

	if (((myRed == 2 && enRed == 0) || (enRed == 2 && myRed == 0)) && left.id == 1 || left.id == 5 || left.id == 9) lsc += 2;
	else if (((myRed == 2 && enRed == 0) || (enRed == 2 && myRed == 0)) && right.id == 1 || right.id == 5 || right.id == 9) rsc += 2;
	else if (((myBlue == 2 && enBlue == 0) || (enBlue == 2 && myBlue == 0)) && left.id == 21 || left.id == 33 || left.id == 37) lsc += 2;
	else if (((myBlue == 2 && enBlue == 0) || (enBlue == 2 && myBlue == 0)) && right.id == 21 || right.id == 33 || right.id == 37) rsc += 2;
	else if (((myBlue == 2 && enGreen == 0) || (enGreen == 2 && myGreen == 0)) && left.id == 13 || left.id == 17 || left.id == 25) lsc += 2;
	else if (((myBlue == 2 && enGreen == 0) || (enGreen == 2 && myGreen == 0)) && right.id == 13 || right.id == 17 || right.id == 25) rsc += 2;

	else if (((myBird == 2 && enBird == 0) || (enBird == 2 && myBird == 0)) && left.id == 13 || left.id == 17 || left.id == 25) lsc += 4;
	else if (((myBird == 2 && enBird == 0) || (enBird == 2 && myBird == 0)) && right.id == 13 || right.id == 17 || right.id == 25) rsc += 4;

	lsc += left.multiple;
	rsc += right.multiple;
	if (lsc >= rsc) return true;
	else return false;	
}

