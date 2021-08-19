#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>

//��Ϸ���ڳߴ磬��Ԫ��ߴ�
#define GAME_WINDOW_W 640
#define GAME_WINDOW_H 480
#define ELEM_SIZE 20
#define NX 24
#define NY 24

//��������
typedef struct pos {
	int x;
	int y;
}Pos;

//̰���ߣ�̰���ߵĳ���, ����
Pos snake[800];
int n;
int dir; //0 �� 1 �� 2 �� 3 ��

//ʳ��
Pos food;

//����
int grade;

//��ͼ, ��ɵ�ͼ��Ԫ�ظ���
Pos map[400];
int m; 

//��Ϸ��ʼ�������
void gamestart();
void initgame();
void initmap();
void initsnake();
void initfood();
void gameover();

//��Ϸ����
int move(int dir, Pos* newhead);
bool isCrashWall(Pos head);
bool isEatFood(Pos head);

//����ͼ��
void paintgame();
void paintmap();
void paintsnake();
void paintfood();


//�쳣
void error() {
	printf("game error\n");
}

//#########################################################################
//-------------------------------main--------------------------------------
int main(void) {

	while (true) {
		gamestart();
		paintgame();

		//������Ϸ����ѭ��
		int ch, res, curdir;
		Pos newhead;
		while (true) {
			if (_kbhit()) {
				//��������룬���·���, ��Ҫ��ȡ���β��ܵõ�����
				ch = _getch();
				ch = _getch();
				//�ϣ�72 �£�80 ��75 �ң�77
				if (ch == 72) curdir = 0;
				else if (ch == 80) curdir = 1;
				else if (ch == 75) curdir = 2;
				else if (ch == 77) curdir = 3;
				if (curdir + dir != 1 && curdir + dir != 5) {
					dir = curdir;
				}

			}
			//���ݷ����ƶ�������� 0 ײǽ����Լ���ײ 1 �����ƶ� 2 �Ե�ʳ��
			
			res = move(dir, &newhead);
			if (res == 0) {
				//0 ײǽ����Լ���ײ����Ϸʧ��
				RECT r = { 25*20, 1*20, 31*20, 3*20 };
				drawtext(_T("��Ϸʧ��"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				r = { 25 * 20, 5 * 20, 31 * 20, 7 * 20 };
				drawtext(_T("���յ÷֣�100"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				r = { 25 * 20, 9 * 20, 31 * 20, 11 * 20 };
				drawtext(_T("����һ�Σ�(y/n)"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				int ch = _getch();
				if (ch == 'y') {
					break;
				}
				else if(ch == 'n'){
					return 0;
				}
				break;
			}
			//������1 �����ƶ� 2 �Ե�ʳ�� ��Ҫ����snake������ͷ���� �����ǳԵ�ʳ��snake���ȼ�һ���Ҳ��ò���β��
			for (int i = n - 1; i >= 0; i--) {
				snake[i + 1] = snake[i];
			}
			snake[0] = newhead;
			int circlex = snake[0].x * ELEM_SIZE + ELEM_SIZE / 2;
			int circley = snake[0].y * ELEM_SIZE + ELEM_SIZE / 2;
			setfillcolor(GREEN);
			solidcircle(circlex, circley, ELEM_SIZE / 2);


			if (res == 1) {
				//��֮ǰβ�͵Ĳ��ָ�����
				int sx = snake[n].x * ELEM_SIZE + ELEM_SIZE / 2;
				int sy = snake[n].y * ELEM_SIZE + ELEM_SIZE / 2;
				clearcircle(sx, sy, ELEM_SIZE / 2);
			}
			if (res == 2) {
				//���ȼ�һ,�÷ּ�һ
				n++;
				grade++;
				
				initfood();
				int fx = food.x * ELEM_SIZE + ELEM_SIZE / 2;
				int fy = food.y * ELEM_SIZE + ELEM_SIZE / 2;
				setfillcolor(YELLOW);
				solidcircle(fx, fy, ELEM_SIZE / 2);
			}

			TCHAR str[64];
			_stprintf_s(str, _T("�÷֣�%d"), grade);
			RECT r = { 25 * 20, 12 * 20, 31 * 20, 14 * 20 };
			//settextcolor(LIGHTBLUE);
			drawtext(str, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			
			Sleep(300);
		}
	}
	




	_getch();
	gameover();
}
//#########################################################################

void gamestart() {
	initgame();
	initmap();
	initsnake();
	initfood();
}
void initgame() {
	initgraph(GAME_WINDOW_W, GAME_WINDOW_H);
	setbkcolor(DARKGRAY);
	cleardevice();
	srand((unsigned)time(NULL));
	grade = 0;
}
void initmap() {
	//��ͼԪ�أ�ǽ�棩�ĸ���
	m = 0; 

	//�������¹������ǽ��
	for (int i = 0; i < NX; i++) {
		for (int j = 0; j < NY; j++) {
			if (i == 0 || i == NX - 1 || j == 0 || j == NY - 1) {
				map[m].x = i;
				map[m].y = j;
				m++;
			}
		}
	}
}
void initsnake() {
	//���һ������
	dir = rand() % 4;

	//��ʼ����
	n = 0;

	//���һ��ͷ��, �����������β��
	int hx, hy, bx, by, tx, ty;
	switch (dir) {
	case 0://��
		hx = rand() % (NX - 2) + 1;
		hy = rand() % (NY - 10) + 5;
		bx = hx; by = hy + 1;
		tx = hx; ty = hy + 2;
		break;
	case 1://��
		hx = rand() % (NX - 2) + 1;
		hy = rand() % (NY - 10) + 5;
		bx = hx; by = hy - 1;
		tx = hx; ty = hy - 2;
		break;
	case 2://��
		hx = rand() % (NX - 10) + 5;
		hy = rand() % (NY - 2) + 1;
		bx = hx + 1; by = hy;
		tx = hx + 2; ty = hy;
		break;
	case 3://��
		hx = rand() % (NX - 10) + 5;
		hy = rand() % (NY - 2) + 1;
		bx = hx - 1; by = hy;
		tx = hx - 2; ty = hy;
	default:
		error();
	}
	snake[n].x = hx;
	snake[n].y = hy;
	n++;
	snake[n].x = bx;
	snake[n].y = by;
	n++;
	snake[n].x = tx;
	snake[n].y = ty;
	n++;
}
void initfood() {
	//����һ���Ϸ�λ�õ�����tmp,��СΪk
	Pos tmp[800];
	int k = 0;
	int arr[24][24] = {0};
	for (int i = 0; i < n; i++) {
		arr[snake[i].x][snake[i].y] = 2;
	}
	for (int i = 0; i < NX; i++) {
		for (int j = 0; j < NY; j++) {
			if (i == 0 || i == NX - 1 || j == 0 || j == NY - 1) {
				arr[i][j] = 1;
			}
			else if (arr[i][j] != 2) {
				Pos pos = { i, j };
				tmp[k++] = pos;
			}
		}
	}

	//�ӺϷ����������һ��λ����Ϊʳ���λ��
	int p = rand() % k;
	food.x = tmp[p].x;
	food.y = tmp[p].y;
}
void gameover() {
	closegraph();
}
void paintgame() {
	paintmap();
	paintsnake();
	paintfood();
}
void paintmap() {
	for (int i = 0; i < m; i++) {
		setfillcolor(RED);
		setlinecolor(BLUE);
		int rectx = map[i].x * ELEM_SIZE;
		int recty = map[i].y * ELEM_SIZE;
		fillrectangle(rectx, recty, rectx + ELEM_SIZE, recty + ELEM_SIZE);
	}
}
void paintsnake() {
	for (int i = 0; i < n; i++) {
		setfillcolor(GREEN);
		setlinecolor(YELLOW);
		int circlex = snake[i].x * ELEM_SIZE + ELEM_SIZE / 2;
		int circley = snake[i].y * ELEM_SIZE + ELEM_SIZE / 2;
		//if (i == 0) {
		//	setfillcolor(RED);
		//	fillcircle(circlex, circley, ELEM_SIZE / 2);
		//	continue;
		//}
		//if (i == n - 1) {
		//	//�����β�ͣ�Сһ��
		//	fillcircle(circlex, circley, ELEM_SIZE / 2.5);
		//	continue;
		//}
		solidcircle(circlex, circley, ELEM_SIZE / 2);
		
	}
}
void paintfood() {
	setfillcolor(YELLOW);
	setlinecolor(CYAN);
	int circlex = food.x * ELEM_SIZE + ELEM_SIZE / 2;
	int circley = food.y * ELEM_SIZE + ELEM_SIZE / 2;
	solidcircle(circlex, circley, ELEM_SIZE / 2);
}
int move(int dir, Pos* newhead) {
	//���ݷ����ƶ����¸�λ��
	if (dir == 0) {
		newhead->x = snake[0].x;
		newhead->y = snake[0].y - 1;
	}
	else if (dir == 1) {
		newhead->x = snake[0].x;
		newhead->y = snake[0].y + 1;
	}
	else if (dir == 2) {
		newhead->x = snake[0].x - 1;
		newhead->y = snake[0].y;
	}
	else if (dir == 3) {
		newhead->x = snake[0].x + 1;
		newhead->y = snake[0].y;
	}

	//�ж���û��ײǽ
	if (isCrashWall(*newhead)) {
		return 0;
	}

	//�ж���û�гԵ�ʳ��
	if (isEatFood(*newhead)) {
		return 2;
	}
	return 1;
}
bool isCrashWall(Pos head) {
	for (int i = 0; i < m; i++) {
		if (head.x == map[i].x && head.y == map[i].y) {
			return true;
		}
	}
	return false;
}
bool isEatFood(Pos head) {
	return head.x == food.x && head.y == food.y;
}