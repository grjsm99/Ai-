#include "board.h"
#include <Windows.h>
#include "Hand.h"
#include <random>
#include <cstdlib>
#include <ctime>
#include "Effect.h"
#include "resource.h"
#include <fstream>
extern Hand hand;
extern Hand handai;
extern int selmode;
extern int extmpMon;
extern bool turn;
extern bool isAni;
extern bool isback;
extern POINTFLOAT start, dest, now;
extern Card aniCard;
extern HWND* hWndptr;
extern Field field;
extern Field fieldai;
extern int cymode;
extern HINSTANCE g_hInst;
extern int money;
extern list<Effect> effects;

void Board::printLayer(HDC& hdc) {

	for (int i = 0; i < 12; i++) {
		int cnt = 0;
		int jokerCnt = 0;
		for (auto it = Layer[i].begin(); it != Layer[i].end(); it++)
		{
			it->Print(hdc, 50 + i % 6 * 140 + (cnt + jokerCnt) * 20 + (i % 6) / 3 * 100, 230 + i / 6 * 120 + (cnt + jokerCnt) * 20, CARDW, CARDH);

			for (auto it2 = JLayer.begin(); it2 != JLayer.end(); it2++) {
				if (it2->mon == i && it2->num == cnt + 2) {
					jokerCnt++;
					it2->card.Print(hdc, 50 + i % 6 * 140 + (cnt + jokerCnt) * 20 + (i % 6) / 3 * 100, 230 + i / 6 * 120 + (cnt + jokerCnt) * 20, CARDW, CARDH);

				}
			}
			cnt++;
		}
	}
}

void Board::printBoard(HDC& hdc) {
	boardimage.Draw(hdc, 0, 0, WXSIZE, WYSIZE, 0, 0, WXSIZE, WYSIZE);
}
void Board::makeDeck() {
	
	Card tempDeck[50];
	for (int i = 0; i < 50; i++) {
		tempDeck[i].id = i;
		tempDeck[i].multiple = 0;
		if (i >= 48) {
			tempDeck[i].type = "��";
			if (i == 48) tempDeck[i].multiple = 2;
			else tempDeck[i].multiple = 1;
		}
		else if (i % 4 == 0 && (i / 4 == 0 || i / 4 == 2 || i / 4 == 7 || i / 4 == 10 || i / 4 == 11)) {
			tempDeck[i].type = "��";
		}
		else if (((i % 4 == 1) && !(i / 4 == 10 || i / 4 == 7 || i / 4 == 11)) || i == 46) {
			tempDeck[i].type = "��";
		}
		else if (i % 4 == 0 || i==45 || i==29) {
			tempDeck[i].type = "����";
		}
		else if (i == 47 || i == 41 || i == 32) {
			tempDeck[i].type = "��";
			tempDeck[i].multiple = 1;
		}
		else tempDeck[i].type = "��";
	}
	
	int cnt = 50;

	int val=0;
	while (cnt >= 1) {
		val = rand() % cnt;
		decklist.push(tempDeck[val]);
		tempDeck[val] = tempDeck[cnt-1];
		deckCnt++;
		cnt--;
		
	}
}

void Board::printdeck(HDC& hdc) {
	if (!decklist.empty())
	{
		for (int i = 0; i < deckCnt / 5 + 1; i++) {
			imgCardSkin.Draw(hdc, 478 - i * 2, 322 - i*2, CARDW, CARDH);
		}
	}
}
void Board::SetFirst() {

    Card card;
    for (int i = 0; i < 8; i++) {
        card = DrawDeck(-1);
        if (card.id <= 47)    Layer[card.id / 4].push_back(card);
        else { 
            i--; 
			myField->skin.push_back(card);
        }
    }

    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 5; i++) {
            card = DrawDeck(1);
            hand.Insert(card);
        }
        for (int i = 0; i < 5; i++) {
            card = DrawDeck(2);
            handai.Insert(card);
        }
    } // ���̾� ����Ȯ��
	for (int i = 0; i < 12; ++i) {
		if (Layer[i].size() == 4) {
			//����
			if (enField->Money < 100000) {
				myField->Money += enField->Money;
				enField->Money = 5000000;    //���� 500��
			}
			else {
				myField->Money += 100000;
				enField->Money -= 100000;
			}
			ShowResult(turn);
			if (turn) {
				Reset(0);
			}
			else {
				Reset(1);
			}
			return;
		}
	}
	//���� ���� Ȯ��
	int fullCard[12] = { 0, };
	for (int i = 0; i < hand.cardNum; ++i) {
		if (0 <= hand.card[i].id / 4 && hand.card[i].id / 4 <= 11 && ++fullCard[hand.card[i].id / 4] == 4) {
			if (fieldai.Money < 100000) {
				field.Money += fieldai.Money;
				fieldai.Money = 5000000;    //���� 500��
			}
			else {
				field.Money += 100000;
				fieldai.Money -= 100000;
			}
			ShowResult(true);
			Reset(0);
			return;
			//����
		}
	}
	//����� ���� Ȯ��
	fill(fullCard, fullCard + 12, 0);
	for (int i = 0; i < hand.cardNum; ++i) {
		if (0 <= handai.card[i].id / 4 && handai.card[i].id / 4 <= 11 && ++fullCard[handai.card[i].id / 4] == 4) {
			//����
			if (field.Money < 100000) {
				fieldai.Money += field.Money;
				field.Money = 5000000;    //���� 500��
			}
			else {
				fieldai.Money += 100000;
				field.Money -= 100000;
			}
			ShowResult(false);
			Reset(1);
			return;
		}
	}
}

Card Board::DrawDeck(int a) {
	Card card = decklist.top();
	decklist.pop();
	deckCnt--;

	if (card.id>=0) {
		isAni = true;	
		aniCard = card;
		start = { (FLOAT)(478 - (deckCnt / 5) * 2), (FLOAT)(322 - (deckCnt / 5) * 2) };
		if (a <= 0) {
			if (card.id / 4 != 12) {
				dest = { (FLOAT)(50 + (card.id / 4) % 6 * 140 + (Layer[card.id / 4].size()) * 20) + (card.id / 4 % 6) / 3 * 100, (FLOAT)(230 + card.id / 4 / 6 * 120 + (Layer[card.id / 4].size()) * 20) };
				if (Layer[card.id / 4].size() == 1 && a == 0 && card.id/4 != tempID/4) aniN = 1; // �Ϲ� ������
				else if (a == 0 && Layer[card.id / 4].size() == 2 && card.id/4 == tempID/4) aniN = 3; // ������
				else if (a == 0 && Layer[card.id / 4].size() == 2 && card.id/4 != tempID/4) aniN = 1; // �Ϲ� ������
				else if (a == 0 && Layer[card.id / 4].size() == 1 && card.id/4 == tempID/4 && tempID>=0) aniN = 5; // ��
				else if (a == 0 && Layer[card.id / 4].size() == 1 && card.id/4 == tempID/4) aniN = 1; // ��ź���� ������
				else if (a == 0 && Layer[card.id / 4].size() == 3) aniN = 4; // 3���ִ°� ������
				else if (a == 0) aniN = 2; // ������
			}
			else {
				if (tempID < 0) 	dest = { (FLOAT)(myField->x + 420 + ((myField->skin.size()) % 5) * 25), (FLOAT)(myField->y + 210 - ((myField->skin.size()) / 5) * 30) };
				else 	dest = { (FLOAT)(50 + (tempID / 4) % 6 * 140 + (Layer[tempID / 4].size()) * 20) + (tempID / 4 % 6) / 3 * 100, (FLOAT)(230 + tempID / 4 / 6 * 120 + (Layer[tempID / 4].size()) * 20) };
				if(a==0) aniN = 1;
			}
		}
		else if (a == 1) {    //���� ��
			dest = { (FLOAT)(hand.x + (hand.cardNum % 5) * (CARDW + 1)) , (FLOAT)(hand.y + (hand.cardNum / 5) * (CARDH + 1)) };
		}
		else if (a == 2) {    //���� ��
			isback = true;
			dest = { (FLOAT)(handai.x + (handai.cardNum % 5) * (CARDW + 1)) , (FLOAT)(handai.y + (handai.cardNum / 5) * (CARDH + 1)) };
		}

		now = start;
		MSG Message;
		while (GetMessage(&Message, 0, 0, 0))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
			if (isAni == false) {
				break;
			}
		}
	}

	return card;
}
void Board::Judge(HDC& hdc, HWND hWnd) {
	int tmpMon;
	Card card = DrawDeck();
	if (tempID != -2) {
		tmpMon = tempID / 4;
	}
	else tmpMon = -1;
	while (card.id / 4 == 12) {
		if(tempID!=-2) JLayer.push_back(JokerCard(card, tmpMon, Layer[tmpMon].size() + 1)); // ����, ��Ŀ ī�� ���� Ǫ��
		else {
			myField->Input(card, 1);
			StealCard();
		}
		rePrint(hWnd, 500);
		card = DrawDeck();	// ����ο�
	}

	int selMon = card.id / 4;
	int stealcnt = 0;

	Layer[selMon].push_back(card);
	rePrint(hWnd, 500);
	


	if (tmpMon == selMon && Layer[selMon].size() == 3) {
		
	} // ��
	else {
		for (auto it = JLayer.begin(); it != JLayer.end();) {
			if (tmpMon == it->mon || selMon == it->mon) {
				myField->Input(it->card);
				JLayer.erase(it++);
				stealcnt++;
			}
			else ++it;
		} // �ش� ���� ��Ŀī�� ������

		if (tmpMon == selMon && Layer[selMon].size() == 2) {
			for (int i = 0; i < 2; i++) {
				myField->Input(Layer[selMon].back());
				Layer[selMon].pop_back();
			}
			stealcnt++;
			aniN = 5;
		}	// ��
		else if (tmpMon == selMon && Layer[selMon].size() == 4) {
			for (int i = 0; i < 4; i++) {
				myField->Input(Layer[selMon].back());
				Layer[selMon].pop_back();
			}
			stealcnt++;
		}	// ����
		else if (tmpMon!=-1 && Layer[tmpMon].size() == 4) {
			for (int i = 0; i < 4; i++) {
				myField->Input(Layer[tmpMon].back());
				Layer[tmpMon].pop_back();
			}
			stealcnt++;
		}	// ��ź or 3���ִµ� 1����

		if (Layer[selMon].size() == 4) {
			for (int i = 0; i < 4; i++) {
				myField->Input(Layer[selMon].back());
				Layer[selMon].pop_back();
			}
			stealcnt++;
		}	// ������ �� ī��� 3��������

		if (Layer[selMon].size() == 3) {
			extmpMon = selMon;

			if (turn == false) {
				PickChoose(Layer[selMon], handai.AiLR(Layer[selMon]));
			}
			else {
				InvalidateRect(hWnd, NULL, false);
				selmode = 1;
				MSG Message;
				while (GetMessage(&Message, 0, 0, 0))
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
					if (Message.message == WM_CLOSE) { SendMessage(*hWndptr, WM_DESTROY, 0, 0); break; }
					if (selmode == 0) {
						break;
					}
				}
			}
		}    // � ���°� ����

		if (tmpMon != -1 && Layer[tmpMon].size() == 3) {
			extmpMon = tmpMon;


			if (turn == false) {
				PickChoose(Layer[tmpMon], handai.AiLR(Layer[tmpMon]));
			}
			else {
				InvalidateRect(hWnd, NULL, false);
				selmode = 1;
				MSG Message;
				while (GetMessage(&Message, 0, 0, 0))
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
					if (Message.message == WM_CLOSE) { SendMessage(*hWndptr, WM_DESTROY, 0, 0); break; }
					if (selmode == 0) {
						break;
					}
				}
			}
		} // ���� ������ ����
		if (tmpMon != selMon && Layer[selMon].size() == 2) {
			
			for (int i = 0; i < 2; i++) {
				myField->Input(Layer[selMon].back());
				Layer[selMon].pop_back();
			}

		} // ��� ����(�Ϲ�)

		if (tmpMon != -1 && tmpMon != selMon && Layer[tmpMon].size() == 2) {
			
			for (int i = 0; i < 2; i++) {
				myField->Input(Layer[tmpMon].back());
				Layer[tmpMon].pop_back();
			}

		} // ���� ����(�Ϲ�)
	

		
		layercnt = 0;
		for (int i = 0; i < 12; i++) layercnt += Layer[i].size();
		if (layercnt == 0 && deckCnt != 0) {
			stealcnt++;	// ��
			effects.push_back(Effect(500, 350, &clearimage, 50, sizeof(clearimage) / sizeof(CImage)));
			PlaySound(MAKEINTRESOURCE(IDR_WAVE10), g_hInst, SND_ASYNC | SND_RESOURCE);
		}
		for (int i = 0; i < stealcnt; i++) {
			StealCard();
		} // ī�廯��
	}
}

void Board::ShowChoose(list<Card> layer, HDC& hdc) {
	selectimage.TransparentBlt(hdc, 380, 250, 243, 143, RGB(255, 255, 255));
	auto it = layer.begin();
	it->Print(hdc, 400, 300, CARDW, CARDH);
	it++;
	it->Print(hdc, 550, 300, CARDW, CARDH);
}

void Board::PickChoose(list<Card>& layer, bool isleft) {
	selmode = 0;
	auto it = layer.begin();
	if (!isleft) it++;

	myField->Input(layer.back());
	layer.erase(--layer.end());

	myField->Input(*it);
	layer.erase(it);


}

void Board::rePrint(HWND hWnd, int time) {
	clock_t start, end;
	start = clock();
	while (clock() - start < time)
	{
		if (clock()  % 30 == 0)
		{
			InvalidateRect(hWnd, NULL, false);
			UpdateWindow(hWnd);
		}
	}
} 

void Board::StealCard() {
	aniN = 0;
	if (!enField->skin.empty()) {
		isAni = true;
		aniCard = enField->skin.back();
		start = { (FLOAT)(enField->x + 420 + ((enField->skin.size() - 1) % 5) * 25) , (FLOAT)(enField->y + 210 - ((enField->skin.size() - 1) / 5) * 30) };
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

		myField->Input(enField->skin.back(), 1);
		enField->skin.pop_back();

	}
	enField->ScoreCalcul();
	myField->ScoreCalcul();
}
void Board::Reset(int a) {
	PlaySound(MAKEINTRESOURCE(IDR_WAVE9), g_hInst, SND_ASYNC | SND_RESOURCE);
	for (int i = 0; i < 12; ++i) {
		graySign[i] = false;
	}

	if (a == 0) {
		myField = &field;
		enField = &fieldai;
	}
	if (a == 1) {
		myField = &fieldai;
		enField = &field;
	}
	aniCard.id = -1;
	myField->light.clear();
	myField->ten.clear();
	myField->band.clear();
	myField->skin.clear();
	myField->score = 0;
	myField->goscore = 6;
	myField->go = 0;
	myField->mulcnt = 0;

	enField->light.clear();
	enField->ten.clear();
	enField->band.clear();
	enField->skin.clear();
	enField->score = 0;
	enField->goscore = 6;
	enField->go = 0;
	enField->mulcnt = 0;
	
	hand.cardNum = 0;
	handai.cardNum = 0;
	for (int i = 0; i < 12; i++) {
		Layer[i].clear();
	}
	while (decklist.size() != 0) decklist.pop();
	JLayer.clear();
	deckCnt = 0;
	tempID = -4;
	makeDeck();
	if (a == 0) { // ��� ��
		turn = true;
	}
	if (a == 1) {
		turn = false;
	}
	SetFirst();
	cymode = a;
}
void Board::makeDeckTest() {
	Card tempDeck[50];
	bool isPut[50] = { false, };
	for (int i = 0; i < 50; i++) {
		tempDeck[i].id = i;
		tempDeck[i].multiple = 0;
		if (i >= 48) {
			tempDeck[i].type = "��";
			if (i == 48) tempDeck[i].multiple = 2;
			else tempDeck[i].multiple = 1;
		}
		else if (i % 4 == 0 && (i / 4 == 0 || i / 4 == 2 || i / 4 == 7 || i / 4 == 10 || i / 4 == 11)) {
			tempDeck[i].type = "��";
		}
		else if (((i % 4 == 1) && !(i / 4 == 10 || i / 4 == 7 || i / 4 == 11)) || i == 46) {
			tempDeck[i].type = "��";
		}
		else if (i % 4 == 0 || i == 45 || i == 29) {
			tempDeck[i].type = "����";
		}
		else if (i == 47 || i == 41 || i == 32) {
			tempDeck[i].type = "��";
			tempDeck[i].multiple = 1;
		}
		else tempDeck[i].type = "��";
	}

/*
	//�׿��ִ� ī��
	isPut[3] = true;
	isPut[48] = true;
	//������ �ִ� ī��
	isPut[37] = true;
	isPut[38] = true;
	isPut[39] = true;
	isPut[40] = true;
	isPut[41] = true;
	//������ �ִ� ī��
	isPut[32] = true;
	isPut[33] = true;
	isPut[34] = true;
	isPut[35] = true;
	isPut[36] = true;
	//������ �ִ� ī��
	isPut[2] = true;
	isPut[6] = true;
	isPut[10] = true;
	isPut[14] = true;
	isPut[18] = true;
	//������ �ִ� ī��
	isPut[1] = true;
	isPut[2] = true;
	isPut[3] = true;
	isPut[13] = true;
	isPut[17] = true;
		//�ǿ� �򸮴� �͵�
		isPut[0] = true;
	isPut[4] = true;
	isPut[8] = true;
	isPut[12] = true;
	isPut[16] = true;
	isPut[20] = true;
	isPut[24] = true;
	isPut[28] = true;

	for (int i = 0; i < 50; ++i) {
		if (!isPut[i]) {
			decklist.push(tempDeck[i]);
		}
	}
	*/

	//�׿��ִ� ī��
	
	



	decklist.push(tempDeck[43]);
	decklist.push(tempDeck[47]);
	decklist.push(tempDeck[11]);

	decklist.push(tempDeck[23]);
	decklist.push(tempDeck[21]);

	decklist.push(tempDeck[39]);
	decklist.push(tempDeck[37]);
	
	decklist.push(tempDeck[5]);
	decklist.push(tempDeck[4]);
	
	decklist.push(tempDeck[10]);
	decklist.push(tempDeck[8]);

	decklist.push(tempDeck[20]);
	decklist.push(tempDeck[33]);
		
	decklist.push(tempDeck[35]);
	decklist.push(tempDeck[27]);
	decklist.push(tempDeck[49]);

	decklist.push(tempDeck[25]);
	decklist.push(tempDeck[38]);

	decklist.push(tempDeck[41]);
	decklist.push(tempDeck[31]);

	decklist.push(tempDeck[36]);
	decklist.push(tempDeck[45]);

	//������ �ִ� ī��
	decklist.push(tempDeck[9]);
	decklist.push(tempDeck[42]);
	decklist.push(tempDeck[26]);
	decklist.push(tempDeck[24]);
	decklist.push(tempDeck[7]);
	//������ �ִ� ī��
	decklist.push(tempDeck[32]);
	decklist.push(tempDeck[34]);
	decklist.push(tempDeck[44]);
	decklist.push(tempDeck[40]);
	decklist.push(tempDeck[12]);
	//������ �ִ� ī��
	decklist.push(tempDeck[6]);
	decklist.push(tempDeck[28]);
	decklist.push(tempDeck[18]);
	decklist.push(tempDeck[17]);
	decklist.push(tempDeck[16]);
	//������ �ִ� ī��
	decklist.push(tempDeck[29]);
	decklist.push(tempDeck[48]);
	decklist.push(tempDeck[22]);
	decklist.push(tempDeck[3]);
	decklist.push(tempDeck[15]);
	//�ǿ� �򸮴� ī��
	decklist.push(tempDeck[46]);
	decklist.push(tempDeck[30]);
	decklist.push(tempDeck[14]);
	decklist.push(tempDeck[13]);
	decklist.push(tempDeck[19]);
	decklist.push(tempDeck[2]);
	decklist.push(tempDeck[1]);
	decklist.push(tempDeck[0]);

	deckCnt = 50;

}

int Board::SendMoney() {
	myField->ScoreCalcul();
	enField->ScoreCalcul();
	if (myField->score == 0 && enField->score == 0) { // �����϶�
		return 100000;
	}
	//����
	if (myField->light.size() >= 3 && enField->light.size() == 0) {
		++myField->mulcnt;
	}

	//�ǹ�
	int mySkinScore = myField->skin.size(), enSkinScore = enField->skin.size();
	for (auto it = myField->skin.begin(); it != myField->skin.end(); ++it) {
		if (it->multiple > 0) { mySkinScore += it->multiple; }
	}
	for (auto it = enField->skin.begin(); it != enField->skin.end(); ++it) {
		if (it->multiple > 0) { enSkinScore += it->multiple; }
	}
	if (mySkinScore >= 10 && enSkinScore <= 5 && enSkinScore > 0) {
		++myField->mulcnt;
	}

	//�۵�
	if (myField->ten.size() >= 7) {
		++myField->mulcnt;
	}

	//��
	for (int i = 3; i <= myField->go; ++i) {
		++myField->mulcnt;
	}

	int result = myField->score * pow(2, myField->mulcnt) * 10000;

	if (enField->Money < result) {
		myField->Money += result;
		enField->Money = 5000000;
		//���â ����
	}
	else {
		myField->Money += result;
		enField->Money -= result;
		//���â ����
	}
	return result;
}
void Board::ShowResult(bool pwin) {
	money = SendMoney() / 10000;
	selmode = 16 - pwin;
	if (pwin) {
		PlaySound(MAKEINTRESOURCE(IDR_WAVE5), g_hInst, SND_ASYNC | SND_RESOURCE);
	}
	else {
		PlaySound(MAKEINTRESOURCE(IDR_WAVE4), g_hInst, SND_ASYNC | SND_RESOURCE);
	}
	MSG Message;
	RECT msgbox = { 314, 210 , 694, 434 };
	InvalidateRect(*hWndptr, &msgbox, false);
	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
		if (Message.message == WM_CLOSE) { SendMessage(*hWndptr, WM_DESTROY, 0, 0); break; }
		if (selmode == 17) {
			selmode = 0;
			break;
		}
	}
	Save();
}

void Board::Save() {
	ofstream fout;
	fout.open("Matgo_info.txt");
	if (fout.is_open()) {

		fout << field.Money << "\n";
		fout << fieldai.Money << "\n";

		fout.close();
	}
}