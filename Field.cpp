#include "Field.h"
#include "Board.h"
#include <cmath>
#include "Hand.h"
#include "resource.h"
#pragma comment(lib,"winmm.lib")

extern int selmode;
extern bool turn;
extern Board board;
extern HWND* hWndptr;
extern bool isAni;
extern POINTFLOAT start, dest, now;
extern Card aniCard;
extern Hand hand;
extern HINSTANCE g_hInst;
extern Hand handai;
void Field::ScoreCalcul() {
	score = 0;

	//�� ���� ADD
	int skinScore = 0;
	for (auto it = skin.begin(); it != skin.end(); ++it) {
		skinScore += 1 + it->multiple;
	}
	if (skinScore >= 10) {
		score += skinScore - 9;
	}

	//�� ���� ADD
	if (light.size() == 3) {
		score += 3;
		//���� ������ ���� -1
		for (auto it = light.begin(); it != light.end(); ++it) {
			if (it->id == 44) {
				score -= 1;
				break;
			}
		}
	}
	else if (light.size() > 3) {
		score += light.size();
		if (light.size() == 5) score += 10;
	}

	//���� ����
	if (ten.size() >= 5) {
		score += ten.size() - 4;
	}
	//���� Ȯ��
	bool have4 = false, have12 = false, have29 = false;
	for (auto it = ten.begin(); it != ten.end(); ++it) {
		if (it->id == 4)
			have4 = true;
		else if (it->id == 12)
			have12 = true;
		else if (it->id == 29)
			have29 = true;
	}
	if (have4 && have12 && have29) {
		score += 5;
	}

	//�� ����
	if (band.size() >= 5) {
		score += band.size() - 4;
	}

	bool have1 = false, have5 = false, have9 = false;	  //ȫ��
	bool have21 = false, have33 = false, have37 = false;  //û��
	bool have13 = false, have17 = false, have25 = false;  //�ʴ�
	for (auto it = band.begin(); it != band.end(); ++it) {
		//ȫ�� ������ �ִ���
		if (it->id == 1)
			have1 = true;
		else if (it->id == 5)
			have5 = true;
		else if (it->id == 9)
			have9 = true;
		//û�� ������ �ִ���
		if (it->id == 21)
			have21 = true;
		else if (it->id == 33)
			have33 = true;
		else if (it->id == 37)
			have37 = true;
		//�ʴ� ������ �ִ���
		if (it->id == 13)
			have13 = true;
		else if (it->id == 17)
			have17 = true;
		else if (it->id == 25)
			have25 = true;
	}

	if (have1 && have5 && have9) {score += 3;} //ȫ�� �޼���
	
	if (have21 && have33 && have37) {score += 3;} //û�� �޼���
	
	if (have13 && have17 && have25) {score += 3;} //�ʴ� �޼���


	if (go == 1) score++;
	if (go > 1) score += 2;;
}

void Field::Print(HDC& hdc) {
	const int spaceW = 25;	//ī�� ��ĥ���� ��
	const int spaceH = 30;	//ī�� ��ĥ���� ����
	//�� ���
	int count = 0;
	for (auto it = light.begin(); it != light.end(); ++it, ++count) {
		it->Print(hdc, x + 10 + count * spaceW , y + 110 , CARDW, CARDH);
	}

	//�� ���
	count = 0;
	for (auto it = band.begin(); it != band.end(); ++it, ++count) {
		it->Print(hdc, x + 160 + count * spaceW, y + 210, CARDW, CARDH);
	}

	//���� ���
	count = 0;
	for (auto it = ten.begin(); it != ten.end(); ++it, ++count) {
		it->Print(hdc, x + 160 + count * spaceW, y + 110, CARDW, CARDH);
	}

	//�� ���
	count = 0;
	for (auto it = skin.begin(); it != skin.end(); ++it, ++count) {
		it->Print(hdc, x + 420 + (count % 5) * spaceW, y + 210 - (count / 5) * spaceH, CARDW, CARDH);
	}
}
void Field::Input(Card card, int ani) {
	if (0 <= card.id / 4 && card.id / 4 < 12) {
		board.graySign[card.id / 4] = true;
	}

	if (card.type == "��") {
		light.push_back(card);
	}
	else if (card.type == "����") {
		if (card.id / 4 == 8 && ani == 0) { // ��ȭ �����϶�
			if (turn == true) {
				selmode = 2;
				MSG Message;
				RECT msgbox = { 380, 250 ,623, 393 };
				InvalidateRect(*hWndptr, &msgbox, false);
				while (GetMessage(&Message, 0, 0, 0))
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
					if (Message.message == WM_CLOSE) { SendMessage(*hWndptr, WM_DESTROY, 0, 0); break; }
					if (selmode == 3) {
						skin.push_back(Card(32, "��", 1));
						card.type = "��";
						selmode = 0;
						break;
					}
					if (selmode == 4) {
						ten.push_back(Card(32, "����", 0));
						selmode = 0;
						break;
					}
				}
			}
			else  skin.push_back(card);
		}
		else if(ani==1 && card.id == 32) skin.push_back(card);
		else  ten.push_back(card);
	}
	else if (card.type == "��") {
		band.push_back(card);
	}
	else if (card.type == "��") {
		skin.push_back(card);
	}


	if (ani == 0) {
		isAni = true;
		aniCard = card;
		if (card.id / 4 < 12) start = { (FLOAT)(50 + (card.id / 4) % 6 * 140 + (board.Layer[card.id / 4].size() - 1) * 20) + (card.id / 4 % 6) / 3 * 100, (FLOAT)(230 + card.id / 4 / 6 * 120 + (board.Layer[card.id / 4].size() - 1) * 20) };
		else {
			if(card.id==board.JLayer.front().card.id)	start = { (FLOAT)(50 + board.JLayer.front().mon % 6 * 140 + (board.JLayer.front().num) * 20) + (board.JLayer.front().mon % 6) / 3 * 100,
				(FLOAT)(230 + board.JLayer.front().mon / 6 * 120 + (board.JLayer.front().num) * 20) };
			else start = { (FLOAT)(50 + board.JLayer.back().mon % 6 * 140 + (board.JLayer.back().num) * 20) + (board.JLayer.back().mon % 6) / 3 * 100,
				(FLOAT)(230 + board.JLayer.back().mon / 6 * 120 + (board.JLayer.back().num) * 20) };
		}
		if(card.type == "��") dest = { (FLOAT)(x + 420 + ((skin.size()) - 1) % 5 * 25), (FLOAT)(y + 210 - ((skin.size() - 1) / 5) * 30) };
		else if (card.type == "��") dest = { (FLOAT)(x + 10 + (light.size() - 1) * 25), (FLOAT)(y + 110)};
		else if (card.type == "��") dest = { (FLOAT)(x + 160 + (band.size() - 1) * 25), (FLOAT)(y + 210)};
		else if (card.type == "����") dest = { (FLOAT)(x + 160 + (ten.size() - 1) * 25), (FLOAT)(y + 110)};
		// 50 + i % 6 * 140 + (cnt + jokerCnt) * 20 + (i % 6) / 3 * 100, 230 + i / 6 * 120 + (cnt + jokerCnt) * 20
		now = start;
		MSG Message;
		while (GetMessage(&Message, 0, 0, 0))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
			if (isAni == false) {
				break;
			}
			if (Message.message == WM_CLOSE) { SendMessage(*hWndptr, WM_DESTROY, 0, 0); break;}
		}

	}
}
void Field::PrintScore(HDC& hdc) {

	if (score >= 10) board.numimage.TransparentBlt(hdc, x + 1000, y + 100 + 80, 32, 45, score / 10 * 32, 0, 32, 45, RGB(5, 151, 5));
	board.numimage.TransparentBlt(hdc, x + 1032, y + 100 + 80, 32, 45, score % 10 * 32, 0, 32, 45, RGB(5, 151, 5));
	board.numimage.TransparentBlt(hdc, x + 1032, y + 100 + 30, 32, 45, go % 10 * 32, 0, 32, 45, RGB(5, 151, 5));


	board.moneyimage.TransparentBlt(hdc, x + 1000, y + 170 + 80, 80, 50, 0, 0, 80, 50, RGB(5, 151, 5));
	board.goimage.TransparentBlt(hdc, x + 1070, y + 100 + 33, 38, 32, 0, 0, 19, 16, RGB(5, 151, 1));
	board.scoreimage.TransparentBlt(hdc, x + 1070, y + 100 + 78, 36, 38, 0, 0, 18, 19, RGB(5, 151, 1));


	if(ai) board.aiportimage.TransparentBlt(hdc, x + 1130, y + 100, 200, 152, 0, 0, 200, 152, RGB(5, 151, 1));
	else board.plportimage.TransparentBlt(hdc, x + 1130, y + 130, 200,115, 0, 0, 200, 115, RGB(5,151,1));
	int digit = log10(Money) + 1;
	int n;	

	for (int i = 0; i < digit; i++) {
		n = Money;
		for (int j = 0; j < digit-i-1; j++) {
			n /= 10;
		}
		board.nummonimage.TransparentBlt(hdc, x+1240+i*16 - digit*16, y+185 + 80, 16, 23, (n%10)*31.5, 0, 31.5, 45, RGB(5,151,102));
	}
}
bool Field::GoStop() {
	if (score > goscore) {
		if (!ai && hand.cardNum!=0) {
			selmode = 5;
			RECT msgbox = { 380, 250 ,623, 393 };
			InvalidateRect(*hWndptr, &msgbox, false);
			MSG Message;
			while (GetMessage(&Message, 0, 0, 0))
			{
				TranslateMessage(&Message);
				DispatchMessage(&Message);
				if (Message.message == WM_CLOSE) { SendMessage(*hWndptr, WM_DESTROY, 0, 0); break; }
				if (selmode == 6) {	// ���Ұ��
					goscore = score;
					if (go <= 1) goscore++;
					go++;
					selmode = 0;
					ScoreCalcul();
					InvalidateRect(*hWndptr, &msgbox, false);
					if (go <= 3)
						PlaySound(MAKEINTRESOURCE(IDR_WAVE11 + go - 1), g_hInst, SND_ASYNC | SND_RESOURCE);
					else
						PlaySound(MAKEINTRESOURCE(IDR_WAVE14), g_hInst, SND_ASYNC | SND_RESOURCE);
					board.rePrint(*hWndptr, 500);
					return false;

				}
				if (selmode == 7) {	// ���ӳ�
					board.ShowResult(true);
					board.Reset(0);
					return true;
				}
			}

		}
		 if (hand.cardNum==0) { //�о����� ��
			if (ai) {
				board.ShowResult(false);
				board.Reset(1);
			}
			else {
				board.ShowResult(true);
				board.Reset(0);
			}
			return true;
		}
		else { // ai ���Ǵ�
			 if (board.myField->AiGoThink()) {
				 goscore = score;
				 if (go <= 2)goscore++;
				 go++;
				 selmode = 0;
				 ScoreCalcul();
				 InvalidateRect(*hWndptr, NULL, false);
				 if (go <= 3)
					 PlaySound(MAKEINTRESOURCE(IDR_WAVE11 + go - 1), g_hInst, SND_ASYNC | SND_RESOURCE);
				 else
					 PlaySound(MAKEINTRESOURCE(IDR_WAVE14), g_hInst, SND_ASYNC | SND_RESOURCE);
				 board.rePrint(*hWndptr, 500);
				 return false;
			 }
			 else {
				 board.ShowResult(false);
				 board.Reset(1);
				 return true;
			 }
		}
	}
	else return false;
}
bool Field::AiGoThink() {
	int cutLine = board.enField->score;
	for (int i = 0; i < 12; ++i) {
		if (board.Layer[i].size() == 3) {    //���� ������
			bool canEat = false;
			for (int i = 0; i < handai.cardNum; ++i) {
				if (handai.card[i].id / 4 == i) {
					canEat = true;
					break;
				}
			}
			if (canEat) {    //���� ������ ������ ĿƮ���ΰ���
				cutLine -= 2;
			}
			else {    //���� ������ ������ ĿƮ��������
				cutLine += 2;
			}

		}
	}
	if (cutLine >= 4) {    //ĿƮ������(�⺻�� �������) 4�� �̻��̸� ����
		return false;
	}

	//���п� ������ ������ ����(2�� �����϶�) ����
	if (handai.cardNum <= 1) {
		for (int i = 0; i < handai.cardNum; ++i) {
			if (board.Layer[handai.card[i].id / 4].size() > 0) {
				break;
			}
			else {
				if (i == handai.cardNum - 1) {
					return false;
				}
			}
		}
	}

	return true;
}