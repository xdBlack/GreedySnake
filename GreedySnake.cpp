#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>

//游戏窗口尺寸，单元格尺寸
#define GAME_WINDOW_W 640
#define GAME_WINDOW_H 480
#define ELEM_SIZE 20
#define NX 24
#define NY 24

//坐标类型
typedef struct pos {
	int x;
	int y;
}Pos;

//贪吃蛇，贪吃蛇的长度, 方向
Pos snake[800];
int n;
int dir; //0 上 1 下 2 左 3 右

//食物
Pos food;

//分数
int grade;

//地图, 组成地图的元素个数
Pos map[400];
int m; 

//游戏初始化与结束
void gamestart();
void initgame();
void initmap();
void initsnake();
void initfood();
void gameover();

//游戏过程
int move(int dir, Pos* newhead);
bool isCrashWall(Pos head);
bool isEatFood(Pos head);

//绘制图像
void paintgame();
void paintmap();
void paintsnake();
void paintfood();


//异常
void error() {
	printf("game error\n");
}

//#########################################################################
//-------------------------------main--------------------------------------
int main(void) {

	while (true) {
		gamestart();
		paintgame();

		//进入游戏控制循环
		int ch, res, curdir;
		Pos newhead;
		while (true) {
			if (_kbhit()) {
				//如果有输入，更新方向, 需要获取二次才能得到方向
				ch = _getch();
				ch = _getch();
				//上：72 下：80 左：75 右：77
				if (ch == 72) curdir = 0;
				else if (ch == 80) curdir = 1;
				else if (ch == 75) curdir = 2;
				else if (ch == 77) curdir = 3;
				if (curdir + dir != 1 && curdir + dir != 5) {
					dir = curdir;
				}

			}
			//根据方向移动，结果： 0 撞墙或和自己碰撞 1 正常移动 2 吃到食物
			
			res = move(dir, &newhead);
			if (res == 0) {
				//0 撞墙或和自己碰撞，游戏失败
				RECT r = { 25*20, 1*20, 31*20, 3*20 };
				drawtext(_T("游戏失败"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				r = { 25 * 20, 5 * 20, 31 * 20, 7 * 20 };
				drawtext(_T("最终得分：100"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				r = { 25 * 20, 9 * 20, 31 * 20, 11 * 20 };
				drawtext(_T("再来一次？(y/n)"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				int ch = _getch();
				if (ch == 'y') {
					break;
				}
				else if(ch == 'n'){
					return 0;
				}
				break;
			}
			//不管是1 正常移动 2 吃到食物 都要更新snake，绘制头部； 区别是吃到食物snake长度加一，且不用擦掉尾部
			for (int i = n - 1; i >= 0; i--) {
				snake[i + 1] = snake[i];
			}
			snake[0] = newhead;
			int circlex = snake[0].x * ELEM_SIZE + ELEM_SIZE / 2;
			int circley = snake[0].y * ELEM_SIZE + ELEM_SIZE / 2;
			setfillcolor(GREEN);
			solidcircle(circlex, circley, ELEM_SIZE / 2);


			if (res == 1) {
				//把之前尾巴的部分给擦掉
				int sx = snake[n].x * ELEM_SIZE + ELEM_SIZE / 2;
				int sy = snake[n].y * ELEM_SIZE + ELEM_SIZE / 2;
				clearcircle(sx, sy, ELEM_SIZE / 2);
			}
			if (res == 2) {
				//长度加一,得分加一
				n++;
				grade++;
				
				initfood();
				int fx = food.x * ELEM_SIZE + ELEM_SIZE / 2;
				int fy = food.y * ELEM_SIZE + ELEM_SIZE / 2;
				setfillcolor(YELLOW);
				solidcircle(fx, fy, ELEM_SIZE / 2);
			}

			TCHAR str[64];
			_stprintf_s(str, _T("得分：%d"), grade);
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
	//地图元素（墙面）的个数
	m = 0; 

	//按照如下规则添加墙面
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
	//随机一个方向
	dir = rand() % 4;

	//初始长度
	n = 0;

	//随机一个头部, 并计算身体和尾部
	int hx, hy, bx, by, tx, ty;
	switch (dir) {
	case 0://上
		hx = rand() % (NX - 2) + 1;
		hy = rand() % (NY - 10) + 5;
		bx = hx; by = hy + 1;
		tx = hx; ty = hy + 2;
		break;
	case 1://下
		hx = rand() % (NX - 2) + 1;
		hy = rand() % (NY - 10) + 5;
		bx = hx; by = hy - 1;
		tx = hx; ty = hy - 2;
		break;
	case 2://左
		hx = rand() % (NX - 10) + 5;
		hy = rand() % (NY - 2) + 1;
		bx = hx + 1; by = hy;
		tx = hx + 2; ty = hy;
		break;
	case 3://右
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
	//生成一个合法位置的数组tmp,大小为k
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

	//从合法数组中随机一个位置作为食物的位置
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
		//	//如果是尾巴，小一点
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
	//根据方向移动到下个位置
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

	//判断有没有撞墙
	if (isCrashWall(*newhead)) {
		return 0;
	}

	//判断有没有吃到食物
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