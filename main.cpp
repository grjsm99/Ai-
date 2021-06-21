#include <windows.h>
#include <tchar.h>
#include <time.h>
#include "resource.h"
#include <atlimage.h>
#include "Hand.h"
#include "Card.h"
#include "board.h"
#include "Field.h"
#include "Effect.h"
#include <fstream>
#include<MMsystem.h>
#pragma comment (lib, "msimg32.lib")
#pragma comment(lib,"winmm.lib")

#define SPEED 10.f
#define CARD_SKIN_N 4
using namespace std;
using namespace Gdiplus;
//전역변수
 
#pragma region 전역변수
CImage imageCard;
CImage imgCardSkin;
CImage imgBombCard;
CImage expd[6];
CImage hit[4];
CImage lips;
CImage cardLabel;
CImage dungimg;
CImage winr;
CImage winl;
CImage drawimg;
CImage imgCardSkins[CARD_SKIN_N];



list<Effect> effects;
Hand hand(false);
Hand handai(true);
Board board;
Field fieldai(true);
Field field(false);
MSG* msgPointer;
HWND* hWndptr;
bool turn = true; // t = 사람

int selmode = 0;
int extmpMon = 0;
int cymode = 0;
int money = 0;
bool isback = false;
bool isAni = false;

POINTFLOAT start, dest, now;
Card aniCard;

//함수선언

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
void CALLBACK TimerAni(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
void EffectPrint(HDC bufDC);
void PrintMap(HDC& bufDC);
void enTurn(HDC& bufDC);
void Save();
void Load(HWND hWnd);
void Loadimg();
void ShowScore(bool, HDC&);
bool Nagari();
HINSTANCE g_hInst;
LPCTSTR IpszClass = L"Window Class Name";
LPCTSTR IpszWindowName = L"Window Programming Lab";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;
	msgPointer = &Message;
	hWndptr = &hWnd;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = IpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	srand((unsigned int)time(NULL));
	hWnd = CreateWindow(IpszClass, L"맞고",  WS_POPUP | WS_BORDER | WS_CAPTION, 0, 0, WXSIZE, WYSIZE + 38, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, bufDC;
	PAINTSTRUCT ps;
	static HBITMAP bufbit = NULL;
	static int dx, dy;
	static int nowCardSkinN = 1;
	switch (iMessage) {
	case WM_CREATE:
		Loadimg();
		hand.myField = &field;
		handai.myField = &fieldai;
		hand.enField = &fieldai;
		handai.enField = &field;
		Load(hWnd);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		bufDC = CreateCompatibleDC(hdc);
		bufbit = CreateCompatibleBitmap(hdc, WXSIZE, WYSIZE);
		SelectObject(bufDC, bufbit);
		if (cymode == 1 && !isAni) enTurn(bufDC);	// cymode = 1이될경우 적턴 시작
		PrintMap(bufDC);

		BitBlt(hdc, 0, 0, WXSIZE, WYSIZE, bufDC, 0, 0, SRCCOPY);	// 더블버퍼링
		DeleteDC(bufDC);
		DeleteObject(bufbit);
		EndPaint(hWnd, &ps);
		break;
	case WM_CHAR:
		if (wParam == 'q' || wParam == 'Q') PostQuitMessage(0);	// 종료
		if ((wParam == 's' || wParam == 'S') && !isAni) {	// 재시작
			SetTimer(hWnd, 1, 10, TimerAni);
			board.Reset(0);
		}
		if (wParam == 'p' || wParam == 'P') {	// 상대카드 보기
			board.viewCard = !board.viewCard;
		}
		if (wParam == 'K' || wParam == 'k') {
			imgCardSkin = imgCardSkins[nowCardSkinN++];
			if (nowCardSkinN >= CARD_SKIN_N) {
				nowCardSkinN = 0;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		if (!isAni)
		{
			bufDC = GetDC(hWnd);	
			dx = LOWORD(lParam);
			dy = HIWORD(lParam);
			if (selmode == 0 && cymode == 0) { // 사람턴 카드선택시
				cymode = 3;
				board.tempID = hand.Pick(hand.ClickedCard(dx, dy)); // 몇번째 카드 선택인지 확인
				board.myField = &field;
				board.enField = &fieldai;
				if ((board.tempID <= 47 && board.tempID >= 0) || board.tempID == -2) {
					board.rePrint(hWnd, 500);
					board.Judge(bufDC, hWnd);	// 덱에서 카드를 뽑은후 낸카드와 함께 상호작용 확인
					field.ScoreCalcul();
					fieldai.ScoreCalcul();	// 점수계산
					board.rePrint(hWnd, 500);
					if (board.myField->GoStop()) break;		//고스톱 여부 확인
					if (Nagari()) {		// 나가리인지 확인
						board.Reset(0);
						break;
					}
					board.rePrint(hWnd, 500);
					cymode = 1;	// 적턴으로 넘김
				}
				else if (board.tempID > 47) {
					board.StealCard();	// 조커일때 카드를 한장뺏음
					cymode = 0;
				}
				else cymode = 0;
				while (PeekMessage(msgPointer, hWnd, 0, 0, PM_REMOVE)) {
					if (msgPointer->message == WM_QUIT) break;
				};
				
				InvalidateRect(hWnd, NULL, false);
			}
			if (selmode == 1) {
				if (turn == true) {
					if (dx >= 400 && dx <= 400 + CARDW && dy >= 300 && dy <= 300 + CARDH) {
						board.PickChoose(board.Layer[extmpMon], true);
						InvalidateRect(hWnd, NULL, false);
					}
					if (dx >= 550 && dx <= 550 + CARDW && dy >= 300 && dy <= 300 + CARDH) {
						board.PickChoose(board.Layer[extmpMon], false);
						InvalidateRect(hWnd, NULL, false);
					}
				}
			}
			if (selmode == 2) {
				if (turn == true) {
					if (dx >= 414 && dx <= 469 && dy >= 354 && dy <= 374) {
						selmode = 3;
						InvalidateRect(hWnd, NULL, false);
					}
					if (dx >= 530 && dx <= 585 && dy >= 354 && dy <= 374) {
						selmode = 4;
						InvalidateRect(hWnd, NULL, false);
					}

				}
			}
			if (selmode == 5) {
				if (dx >= 414 && dx <= 469 && dy >= 354 && dy <= 374) {
					selmode = 6;
					InvalidateRect(hWnd, NULL, false);
				}
				if (dx >= 530 && dx <= 585 && dy >= 354 && dy <= 374) {
					selmode = 7;
					InvalidateRect(hWnd, NULL, false);
				}
			}
			if (selmode == 15 || selmode == 16) {
				if (dx >= 460 && dx <= 540 && dy >= 384 && dy <= 414) {
					selmode = 17; 
					InvalidateRect(hWnd, NULL, false);
				}
			}
			if (selmode == 20) {
				if (dx >= 460 && dx <= 540 && dy >= 384 && dy <= 414) {
					selmode = 21;
					InvalidateRect(hWnd, NULL, false);
				}
			}
			ReleaseDC(hWnd, bufDC);
		}
		break;
	case WM_COMMAND:
		
		break;
	case WM_TIMER:

		break;
	case WM_KEYDOWN:

		break;
	case WM_KEYUP:

		break;
	case WM_DESTROY:
		imageCard.Destroy();
		imgCardSkin.Destroy();
		board.boardimage.Destroy();
		PostQuitMessage(0);
		return 0;
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void CALLBACK TimerAni(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {

	InvalidateRect(hWnd, NULL, false);
}
void PrintMap(HDC& bufDC) {
	board.printBoard(bufDC);
	hand.Print(bufDC);
	handai.Print(bufDC);
	board.printLayer(bufDC);
	field.Print(bufDC);
	fieldai.Print(bufDC);
	field.PrintScore(bufDC);
	fieldai.PrintScore(bufDC);
	board.printdeck(bufDC);


	if (selmode == 2)	board.selgookimage.TransparentBlt(bufDC, 380, 250, 243, 143, RGB(255, 255, 255));
	if (selmode == 1 && turn == true)	board.ShowChoose(board.Layer[extmpMon], bufDC);
	if (selmode == 5) board.gostopimage.TransparentBlt(bufDC, 380, 250, 243, 143, RGB(255, 255, 255));
	if (selmode == 15) {
		winr.TransparentBlt(bufDC, 314, 210, 380, 224, RGB(255, 255, 255));
		ShowScore(true, bufDC);
	}
	if (selmode == 16) {
		winl.TransparentBlt(bufDC, 314, 210, 380, 224, RGB(255, 255, 255));
		ShowScore(false, bufDC);
	}
	if (selmode == 20) {
		drawimg.TransparentBlt(bufDC, 314, 210, 380, 224, RGB(255, 255, 255));
	}
	EffectPrint(bufDC);
	if (isAni) {
		if(!isback) aniCard.PrintAni(bufDC, now.x, now.y, CARDW, CARDH);
		else {
			aniCard.PrintAni(bufDC, now.x, now.y, CARDW, CARDH, 1);
		}
		now.x += (dest.x - start.x) / SPEED;
		now.y += (dest.y - start.y) / SPEED;
		if (dest.x - 3 <= now.x && now.x <= dest.x + 3 && dest.y - 3 <= now.y && now.y <= dest.y + 3) {
			isAni = false;
			isback = false;
			aniCard.id = -1;
			if (board.expdN-- > 0) {
				effects.push_back(Effect(dest.x + CARDW / 2, dest.y + CARDH / 2, &expd[0], 5, sizeof(expd) / sizeof(CImage)));
				PlaySound(MAKEINTRESOURCE(IDR_WAVE2), g_hInst, SND_ASYNC | SND_RESOURCE);
				board.rePrint(*hWndptr, 100);
			}
			else {
				if (board.aniN == 1) {	// 카드 먹었을때
					effects.push_back(Effect(dest.x, dest.y, &hit[0], 5, sizeof(hit) / sizeof(CImage)));
					PlaySound(MAKEINTRESOURCE(IDR_WAVE1), g_hInst, SND_ASYNC | SND_RESOURCE);
				}
				if(board.aniN ==2) {	// 빈손
					PlaySound(MAKEINTRESOURCE(IDR_WAVE3), g_hInst, SND_ASYNC | SND_RESOURCE);
				}
				if (board.aniN == 3) {	// 쌈
					effects.push_back(Effect(dest.x, dest.y, &dungimg, 10, sizeof(dungimg) / sizeof(CImage)));
					PlaySound(MAKEINTRESOURCE(IDR_WAVE6), g_hInst, SND_ASYNC | SND_RESOURCE);
				}
				if (board.aniN == 4) {	// 3개있는거 먹을때
					effects.push_back(Effect(dest.x, dest.y, &hit[0], 5, sizeof(hit) / sizeof(CImage)));
					PlaySound(MAKEINTRESOURCE(IDR_WAVE7), g_hInst, SND_ASYNC | SND_RESOURCE);
				}
				if (board.aniN == 5) { // 쪽
					effects.push_back(Effect(dest.x, dest.y, &lips , 10, sizeof(lips) / sizeof(CImage)));
					PlaySound(MAKEINTRESOURCE(IDR_WAVE8), g_hInst, SND_ASYNC | SND_RESOURCE);
				}
				if (board.aniN == 6) { // 쓸
					effects.push_back(Effect(dest.x, dest.y, &hit[0], 5, sizeof(hit) / sizeof(CImage)));
					
				}
				board.aniN = 0;
			}
		}

	}

}
void EffectPrint(HDC bufDC) {
	for (auto it = effects.begin(); it != effects.end();) {
		if (it->hp % it->div == 0) it->NextFrame();
		if (it->DieCheck()) {    //삭제
			effects.erase(it++);
		}
		else {    //프린트
			it->Print(bufDC);
			++it;
		}
	}
}

void Save() {
	ofstream fout;
	fout.open("Matgo_info.txt");
	if (fout.is_open()) {

		fout << field.Money << "\n";
		fout << fieldai.Money << "\n";

		fout.close();
	}
}
void Load(HWND hWnd) {
	int answer = MessageBox(hWnd, L"저장된 데이터를 불러오시겠 습니까?", L"Load", MB_YESNO);
	if (answer == IDYES) {
		ifstream fin;
		fin.open("Matgo_info.txt");
		if (fin.is_open()) {
			fin >> field.Money;
			fin >> fieldai.Money;

			fin.close();
		}
		else {
			MessageBox(hWnd, L"저장된 데이터가 없습니다.", L"Load 실패", MB_OK);
			field.Money = 5000000;
			fieldai.Money = 5000000;
		}
	}
	else {
		field.Money = 5000000;
		fieldai.Money = 5000000;
	}


}

void Loadimg() {
	imgCardSkins[0].Load(L"img\\cardskin.bmp");
	imgCardSkins[1].Load(L"img\\cardskin1.png");
	imgCardSkins[2].Load(L"img\\cardskin2.png");
	imgCardSkins[3].Load(L"img\\cardskin3.png");
	imgCardSkin = imgCardSkins[0];
	imageCard.Load(L"img\\cardlist.bmp");
	imgBombCard.Load(L"img\\bomb.bmp");
	cardLabel.Load(L"img\\cardbands.bmp");
	expd[0].Load(L"img\\exp1.png");
	expd[1].Load(L"img\\exp2.png");
	expd[2].Load(L"img\\exp3.png");
	expd[3].Load(L"img\\exp4.png");
	expd[4].Load(L"img\\exp5.png");
	expd[5].Load(L"img\\exp6.png");
	hit[0].Load(L"img\\hit0.png");
	hit[1].Load(L"img\\hit1.png");
	hit[2].Load(L"img\\hit2.png");
	hit[3].Load(L"img\\hit3.png");
	lips.Load(L"img\\lips.png");
	winr.Load(L"img\\pwin.bmp");
	winl.Load(L"img\\plose.bmp");
	drawimg.Load(L"img\\draw.png");
	dungimg.Load(L"img\\dung.png");
}

void enTurn(HDC& bufDC) {
	turn = false;
	cymode = 2;
	board.myField = &fieldai;
	board.enField = &field;
	board.tempID = handai.Pick(handai.AiPick());
	while (board.tempID > 47) {	// 조커냈을경우 다시 냄
		board.StealCard();
		board.rePrint(*hWndptr, 500);
		board.tempID = handai.Pick(handai.AiPick());

	}
	board.rePrint(*hWndptr, 500);
	if (board.tempID <= 47 && board.tempID >= 0 || board.tempID == -2) {
		board.Judge(bufDC, *hWndptr);
	}
	field.ScoreCalcul();
	fieldai.ScoreCalcul();
	if (board.myField->GoStop()) return;
	if (Nagari()) {
		board.Reset(0);
		return;
	}
	cymode = 0;
	turn = true;

}
void ShowScore(bool pwin, HDC& hdc) {

	int digit = log10(money) + 1;
	int n;
	for (int i = 0; i < digit; i++) {
		n = money;
		for (int j = 0; j < digit - i - 1; j++) {
			n /= 10;
		}
		if (!pwin) {
			board.nummonimage.TransparentBlt(hdc, 500 + i * 25, 283, 25, 35.8, (n % 10) * 31.5, 0, 31.5, 45, RGB(5, 151, 102));
			board.nummonimage.TransparentBlt(hdc, 450 + i * 25 - digit * 25, 334, 25, 35.8, (n % 10) * 31.5, 0, 31.5, 45, RGB(5, 151, 102));
		}
		else {
			board.numimage.TransparentBlt(hdc, 500 + i * 25, 283, 25, 35.8, (n % 10) * 31.5, 0, 31.5, 45, RGB(5, 151, 5));
			board.numimage.TransparentBlt(hdc, 450 + i * 25 - digit * 25, 334, 25, 35.8, (n % 10) * 31.5, 0, 31.5, 45, RGB(5, 151, 5));
		}
	}
	for (int i = 0; i < 4; i++) {
		if (!pwin) {
			board.nummonimage.TransparentBlt(hdc, 450 + i * 25, 334, 25, 35.8, 0, 0, 31.5, 45, RGB(5, 151, 102));
		}
		else {
			board.numimage.TransparentBlt(hdc, 450 + i * 25, 334, 25, 35.8, 0, 0, 31.5, 45, RGB(5, 151, 5));
		}
	}

}
bool Nagari() {
	if (handai.cardNum == 0 && hand.cardNum == 0) {
		selmode = 20;
		MSG Message;
		RECT msgbox = { 314, 210 , 694, 434 };
		InvalidateRect(*hWndptr, &msgbox, false);
		while (GetMessage(&Message, 0, 0, 0))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
			if (Message.message == WM_CLOSE) { SendMessage(*hWndptr, WM_DESTROY, 0, 0); break; }
			if (selmode == 21) {
				selmode = 0;
				break;
			}
		}
		return true;
	}
	return false;
}