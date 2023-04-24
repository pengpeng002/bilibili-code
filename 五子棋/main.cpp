/*
	Name: ������ 
	Copyright: pengpeng 
	Author: ���� 
	Date: 2021/12/01 15:53
	Description: ��������Ϸ 
*/

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <vector>
#include "res.h"//��Դ�ļ� 
using namespace std;

#define SPACE 0 // δ���� 
#define HUMAN (nowBlack ? 1 : -1) // ��������� 
#define AI (nowBlack ? -1 : 1) // ��AI���� 
const char* WINDOW_TITLE = "�������塿"; // ���� 
const char* WINDOW_CLASS_NAME = WINDOW_TITLE; // ���� 
const int BLOCK_SIZE = 30; // ����������֮��ļ�� 
const int LINES = 15; // �����ߵ����� 
const int MARGIN = 15; // ���̱�Ե��Ի����Ե�Ŀ�϶ 
const int RADIUS = 10; // ���ӻ��ư뾶
const int BORDER = (LINES - 1) * BLOCK_SIZE; // ���̵Ĵ�С
const int MARK_SIZE = 6; // ���ӱ����Χ 
const int WINDOW_HEIGHT = BORDER + MARGIN * 2 + 50; // �Ի��򴰿ڸ߶� 
const int WINDOW_WIDTH = BORDER + MARGIN * 2 + 10; // �Ի��򴰿ڿ�� 
int maps[LINES+2][LINES+2]; // �������飬ȡֵ-1(white) 0(space) 1(black) 
int score[LINES+2][LINES+2]; // �������� 
int px=-1, py=-1; // ����ƶ������������� 
bool nowBlack = 1; // �Ƿ�ǰΪ������ 
bool inGrid=0; // px py ȡֵ�Ƿ���Ч 
bool gameOver = 0; // ��ǰ��Ϸ�Ƿ��Ѿ����� 
bool bWin = 0; // �Ƿ����Ӯ�� 
bool useAI = 1; // �Ƿ��˻���ս 
bool firstAI = 0; // �Ƿ�AI���� 
int ppx[]={3,7,11,11,3}; // �����м��������ǵ��X���� 
int ppy[]={3,7,11,3,11}; // �����м��������ǵ��Y���� 

HWND hwnd;
HDC hdc;
HDC mdc = NULL; // �ڴ�DC���
HBITMAP bmp2; // �ڴ�DC��bmp 
HINSTANCE inst;
HMENU hmenu = NULL;
bool useCache = 0;


void AIAction(int *p, int *q);
void calcScore();


void redraw()
{
	RedrawWindow(::hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

void DrawLine(HDC &hdc, int x, int y, int x1, int y1)
{
	MoveToEx(hdc, x, y, NULL);
	LineTo(hdc, x1, y1);
}

void DrawRectangle(HDC &hdc, int x, int y, int x1, int y1)
{
	DrawLine(hdc, x, y, x1, y);
	DrawLine(hdc, x1, y, x1, y1);
	DrawLine(hdc, x1, y1, x, y1);
	DrawLine(hdc, x, y1, x, y);
}

void checkInGrid(int mx, int my)
{
	for (int i=-MARK_SIZE; i<=MARK_SIZE; i++)
	{
		for (int j=-MARK_SIZE; j<=MARK_SIZE; j++)
		{
			int xx = mx+i-MARGIN, yy = my+j-MARGIN;
			if (xx % BLOCK_SIZE == 0 && yy % BLOCK_SIZE == 0)
			{
				px = xx / BLOCK_SIZE;
				py = yy / BLOCK_SIZE;
				inGrid = 1;
				return ;
			}
		}
	}
	inGrid = 0;
}

bool isWin(int x, int y)
{
	for (int i=0; i<5; i++)
		if (x-i >= 0 && x-i+4 < LINES)
		{
			if (maps[x-i][y] == maps[x-i+1][y] && 
				maps[x-i][y] == maps[x-i+2][y] &&
				maps[x-i][y] == maps[x-i+3][y] &&
				maps[x-i][y] == maps[x-i+4][y])
				{
					return true;
				}
		}
		
	for (int i=0; i<5; i++)
		if (y-i >= 0 && y-i+4 < LINES)
		{
			if (maps[x][y-i] == maps[x][y-i+1] && 
				maps[x][y-i] == maps[x][y-i+2] &&
				maps[x][y-i] == maps[x][y-i+3] &&
				maps[x][y-i] == maps[x][y-i+4])
				{
					return true;
				}
		}
		
	for (int i=0; i<5; i++)
		if (x+i < LINES && x+i-4 >= 0 && y-i >= 0 && y-i+4 < LINES)
		{
			if (maps[x+i][y-i] == maps[x+i-1][y-i+1] && 
				maps[x+i][y-i] == maps[x+i-2][y-i+2] &&
				maps[x+i][y-i] == maps[x+i-3][y-i+3] &&
				maps[x+i][y-i] == maps[x+i-4][y-i+4])
				{
					return true;
				}
		}
		
	for (int i=0; i<5; i++)
		if (x-i < LINES && x-i+4 >= 0 && y-i >= 0 && y-i+4 < LINES)
		{
			if (maps[x-i][y-i] == maps[x-i+1][y-i+1] && 
				maps[x-i][y-i] == maps[x-i+2][y-i+2] &&
				maps[x-i][y-i] == maps[x-i+3][y-i+3] &&
				maps[x-i][y-i] == maps[x-i+4][y-i+4])
				{
					return true;
				}
		}
		
	return false;
}

void checkWin()
{
	bool first = 1;
	if (gameOver)
	{
		memset(maps, 0, sizeof(maps));
		gameOver = 0;
		return ;
	}
	if (isWin(px, py) == true)
	{
		bWin = 1;
		gameOver = 1;
	}
}

void drawWindow(HDC &hdc)
{
	HPEN pen = CreatePen(PS_SOLID, 2, RGB(255,0,0));
	SelectObject(hdc, pen);
	DrawRectangle(hdc, 5, 5, WINDOW_WIDTH-15, WINDOW_HEIGHT-58);
	DrawRectangle(hdc, MARGIN, MARGIN, BORDER+MARGIN, BORDER+MARGIN);
	const int kBoardMargin = 15;
	for (int i = 0; i < LINES; i++)
    {
        DrawLine(hdc, MARGIN + BLOCK_SIZE * i, MARGIN, MARGIN + BLOCK_SIZE * i, BORDER + MARGIN);
        DrawLine(hdc, MARGIN, MARGIN + BLOCK_SIZE * i, BORDER + MARGIN, MARGIN + BLOCK_SIZE * i);
    }
    HBRUSH black = CreateSolidBrush(RGB(0,0,0));
    HBRUSH white = CreateSolidBrush(RGB(255,255,255));
    HBRUSH gray = CreateSolidBrush(RGB(240,240,240));
    for (int i=0; i<LINES; i++)
    {
    	for (int j=0; j<LINES; j++)
    	{
    		if (maps[i][j] != 0)
    		{
    			SelectObject(hdc, (maps[i][j] == 1) ? black : white);
    			Ellipse(hdc, (MARGIN + BLOCK_SIZE * i)-RADIUS, (MARGIN + BLOCK_SIZE * j)-RADIUS, (MARGIN + BLOCK_SIZE * i)+RADIUS, (MARGIN + BLOCK_SIZE * j)+RADIUS); 
			}
		}
	}
	for (int i=0; i<5; i++)
	{
		int x=ppx[i], y=ppy[i];
		if (maps[x][y]==0)
		{
			SelectObject(hdc, gray);
			int radius = 3;
			Ellipse(hdc, (MARGIN + BLOCK_SIZE * x)-radius, (MARGIN + BLOCK_SIZE * y)-radius, (MARGIN + BLOCK_SIZE * x)+radius, (MARGIN + BLOCK_SIZE * y)+radius); 
		}
	}
    if (inGrid)
    {
    	SelectObject(hdc, nowBlack ? black : white);
    	Ellipse(hdc, (MARGIN + BLOCK_SIZE * px)-RADIUS, (MARGIN + BLOCK_SIZE * py)-RADIUS, (MARGIN + BLOCK_SIZE * px)+RADIUS, (MARGIN + BLOCK_SIZE * py)+RADIUS); 
	}
}

void tipsAndInit(int ID)
{
	int ret = MessageBox(::hwnd, "ȷ����ʼ����Ϸ��?", "tips", MB_OKCANCEL);
	if (ret == IDOK)
	{
		memset(maps, 0, sizeof(maps));
		gameOver = 0;
		if (ID != ID_NEW_GAME)
		{
			CheckMenuItem(hmenu, ID_HUMAN, MF_UNCHECKED);
			HMENU menu = GetSubMenu(GetSubMenu(hmenu, 0), 3);
			CheckMenuItem(menu, ID_FIRST_HUMAN, MF_UNCHECKED);
			CheckMenuItem(menu, ID_FIRST_AI, MF_UNCHECKED);
			CheckMenuItem(ID == ID_HUMAN ? hmenu : menu, ID, MF_CHECKED);
			
			useAI = (ID!=ID_HUMAN);
			nowBlack = (ID!=ID_FIRST_AI);
			firstAI = (ID==ID_FIRST_AI);
			maps[7][7] = firstAI;
		}
		else
		{
			nowBlack = !(useAI && firstAI);
			maps[7][7] = (useAI && firstAI);
		}
		redraw();
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		case WM_PAINT: {
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);//�ز����٣���ȻMessageBox�������̵�����ԭ��֪�� 
			if (useCache)
			{
				mdc = CreateCompatibleDC(hdc);//����һ����hdc���ݵ��ڴ�DC
				bmp2 = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
				SelectObject(mdc, bmp2);
			}
			drawWindow(useCache ? mdc : hdc);
			if (useCache)
			{
				BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mdc, 0, 0, SRCCOPY);
			}
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		case WM_SYSCOMMAND: {//ϵͳ��Ϣ������������� 
			if (wParam == SC_MAXIMIZE) ;//������󻯰�ť 
			else DefWindowProc(hwnd, Message, wParam, lParam);
			break;
		}
		case WM_NCRBUTTONDOWN://�ڷǹ����������Ҽ�
		case WM_NCLBUTTONDBLCLK: {//�ڷǹ�����˫���һ�
			break;
		}
		case WM_MOUSEMOVE: {
			if (gameOver) break;
			int mx = LOWORD(lParam), my = HIWORD(lParam);
			bool pre = inGrid;
			checkInGrid(mx, my);
			if (inGrid != pre && maps[px][py] == 0)
			{
				redraw();
			}
			break;
		}
		case WM_LBUTTONDOWN: {
			if (gameOver)
			{
				memset(maps, 0, sizeof(maps));
				gameOver = 0;
				if (useAI && firstAI)
				{
					nowBlack = 0;
					maps[7][7]=1;
				}
				redraw();
				break ;
			}
			int mx = LOWORD(lParam), my = HIWORD(lParam);
			checkInGrid(mx, my);
			if (maps[px][py]==0)
			{
				maps[px][py] = nowBlack ? 1 : -1;
				inGrid = 0;
				checkWin();
				if (gameOver && bWin)
				{
					redraw();
					MessageBox(hwnd, "you are win", "tips", 0);
					break;
				}
				if (useAI)
				{
					Sleep(700);
					int p, q;
					AIAction(&p, &q);
					maps[p][q]=nowBlack ? -1 : 1;
					px = p, py = q;
					checkWin();
					redraw();
					if (gameOver && bWin)
					{
						MessageBox(hwnd, "you are lose!", "tips", 0);
					}
				}
				else
				{
					nowBlack = !nowBlack;
				}
			}
			break;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam))
			{
				case ID_HUMAN:
				case ID_NEW_GAME:
				case ID_FIRST_HUMAN:
				case ID_FIRST_AI: {
					tipsAndInit(LOWORD(wParam));
					break;
				}
				case ID_EXIT://�˳� 
				{
					PostQuitMessage(0);
					break;
				}
				case ID_SHOW_HELP://���� 
				{
					MessageBox(hwnd, "�����ͬ��ɫ����������һ��ֱ�߼����ʤ��", "Help", 0);
					break;
				}
				case ID_ABOUT://���� 
				{
					MessageBox(hwnd, "Name: ������\nCopyright: pengpeng\nAuthor: ����\nDate: 2021/12/01 15:53\nDescription: ��������Ϸ", "About", 0);
					break;
				}
			}
			break;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	MSG Msg;
	inst = hInstance;

	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* insert window procedure function here */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* use "A" as icon name when you want to use the project icon */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* as above */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	hmenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU));
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,WINDOW_CLASS_NAME,WINDOW_TITLE,WS_VISIBLE|WS_OVERLAPPEDWINDOW^WS_THICKFRAME,250,200,WINDOW_WIDTH,WINDOW_HEIGHT,NULL,hmenu,hInstance,NULL);
	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	HMENU menu = GetSubMenu(GetSubMenu(hmenu, 0), 3);
	CheckMenuItem(menu, ID_FIRST_HUMAN, MF_CHECKED);
	
	hdc = GetDC(::hwnd);
	

	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}


void AIAction(int *p, int *q)
{
	memset(score, 0, sizeof(score));
	calcScore();
	int maxScore=0, x=0, y=0;
	for (int i=0; i<LINES; i++)
	{
		for (int j=0; j<LINES; j++)
		{
			if (maps[i][j] == 0 && score[i][j] > maxScore)
			{
				maxScore = score[i][j];
				*p=i, *q=j; 
			}
		}
	}
}

void calcScore()
{
    for (int i=0; i<LINES; i++)
	{
		for (int j=0; j<LINES; j++)
		{
			if (maps[i][j] == 0)
			{
			    for (int x=-1; x<=1; x++)
			    {
			    	for (int y=-1; y<=1; y++)
			    	{
			    		if (x!=0 || y!=0)
			    		{
			    			int humanNum = 0; // ��������ӵĸ���
						    int robotNum = 0; // AI�����ӵĸ���
						    int emptyNum = 0; // ������հ�λ�ĸ���
			    
			    			for (int d=1; d<=4; d++)//��AI���ӵ�ʱ�򣬿�����Ϊһ��û������5���ӵ� 
			    			{
			    				int dx=i+d*x, dy = j+d*y;
			    				if (dx>=0 && dx<LINES && dy>=0 && dy<LINES)
			    				{
			    					if (maps[dx][dy]==HUMAN) humanNum++;
			    					else if (maps[dx][dy]==SPACE)
			    					{
			    						emptyNum++;
			    						break;
									}
			    					else break;
								}
							}
							
							for (int d=1; d<=4; d++)//��AI���ӵ�ʱ�򣬿�����Ϊһ��û������5���ӵ� 
			    			{
			    				int dx=i-d*x, dy = j-d*y;
			    				if (dx>=0 && dx<LINES && dy>=0 && dy<LINES)
			    				{
			    					if (maps[dx][dy]==HUMAN) humanNum++;
			    					else if (maps[dx][dy]==SPACE)
			    					{
			    						emptyNum++;
			    						break;
									}
			    					else break;
								}
							}
							
							if (humanNum == 1) score[i][j]+=10;
							else if (humanNum == 2)
							{
								if (emptyNum == 1) score[i][j]+=30;
								else if (emptyNum == 2) score[i][j]+=40; 
							}
							else if (humanNum == 3)
							{
								if (emptyNum == 1) score[i][j]+=60;
								else if (emptyNum == 2) score[i][j]+=110;
							}
							else if (humanNum == 4) score[i][j]+=1000;
							
							emptyNum = 0;
							
							for (int d=1; d<=4; d++)//��AI���ӵ�ʱ�򣬿�����Ϊһ��û������5���ӵ� 
			    			{
			    				int dx=i+d*x, dy = j+d*y;
			    				if (dx>=0 && dx<LINES && dy>=0 && dy<LINES)
			    				{
			    					if (maps[dx][dy]==AI) robotNum++;
			    					else if (maps[dx][dy]==SPACE)
			    					{
			    						emptyNum++;
			    						break;
									}
			    					else break;
								}
							}
							
							for (int d=1; d<=4; d++)//��AI���ӵ�ʱ�򣬿�����Ϊһ��û������5���ӵ� 
			    			{
			    				int dx=i-d*x, dy = j-d*y;
			    				if (dx>=0 && dx<LINES && dy>=0 && dy<LINES)
			    				{
			    					if (maps[dx][dy]==AI) robotNum++;
			    					else if (maps[dx][dy]==SPACE)
			    					{
			    						emptyNum++;
			    						break;
									}
			    					else break;
								}
							}
							
							if (robotNum==0) score[i][j]+=5;
							else if (robotNum==1) score[i][j]+=10;
							else if (robotNum==2)
							{
								if (emptyNum==1) score[i][j]+=25;
								else if (emptyNum==2) score[i][j]+=50;
							} 
							else if (robotNum==3)
							{
								if (emptyNum==1) score[i][j]+=55;
								else if (emptyNum==2) score[i][j]+=100;
							} 
							else if (robotNum==4) score[i][j]+=1000;
						}
					}
				}
			}
		}
	}
}

