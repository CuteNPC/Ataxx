//同化棋 
//马文晗 2100013124
//2022.1.6 
#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<windows.h>
#include<cstring>
#include<conio.h>
#include<fstream>
#include<cmath>
#include<climits>
#include <stdio.h>  
#include <windows.h>
#include <MMSystem.h>
#pragma comment(lib,"winmm.lib")

using namespace std;

const HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
const char chestr[10][5] = { "◆","●","  ","○","◇" };//棋子字符
char playername[4][11] = { "","玩家","机器","随机" };
char blackwhite[2][5] = { "黑","白" };
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } };
bool REC = 0;
int randnum;

int map[7][7] = { 0 };//地图
int arrowX = 3, arrowY = 3;//光标位置
int myround;//回合数
int turn;//轮到谁
int lastFromX, lastFromY, lastToX, lastToY;//上一次移动
int historystep[10000][5];//历史步数
int backmap[2][7][7] = { 0 };//悔棋棋盘

int blackplayer = 0;
int whiteplayer = 0;//黑白棋子有谁控制
int blackdepth = 4;//黑棋机器人智商
int whitedepth = 4;//白棋机器人智商

char menu0 = '0';//菜单
int maxdepth = 4;//bot搜索层数
int waittime1 = 750;//等待时间

void cls();
bool inmap(int x, int y);
void gotoxy1(int X, int Y);
void gotoxy(int X, int Y);
void showch1(int X, int Y, char ch);
void showch(int X, int Y, int chess);
void showstep(int color, int round);
void draw_table();
void draw_border();
void init_map();
void move(int map[7][7], int fromX, int fromY, int toX, int toY, int color);
void mapmove(int fromX, int fromY, int toX, int toY, int color);
int evaluate(int map[7][7], int color, int color2);
int search(int lastmap[7][7], int depth, int alpha, int beta, int color);
void menu_save();
int player(int color);
int randBot(int color);
int Bot(int color);
int win(int color);
int win(int color);
void chessback(int color);
void menu_savereplay();
void menu_readreplay();
void menu_main();
void ingame();
void menu_new();
void menu_load();
void menu_help();

int (*f[4]) (int player) = { NULL, player,Bot,randBot };

//清除整个屏幕缓冲区
void cls()
{
	COORD coord = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO binfo; /* to get buffer info */
	DWORD num;

	/* 取当前缓冲区信息 */
	GetConsoleScreenBufferInfo(hout, &binfo);
	/* 填充字符 */
	FillConsoleOutputCharacter(hout, (TCHAR)' ', binfo.dwSize.X * binfo.dwSize.Y, coord, &num);
	/* 填充属性 */
	FillConsoleOutputAttribute(hout, binfo.wAttributes, binfo.dwSize.X * binfo.dwSize.Y, coord, &num);

	/* 光标回到(0,0) */
	SetConsoleCursorPosition(hout, coord);
	return;
}

//判断一个位置是不是在地图中
inline bool inmap(int x, int y)
{
	return(x >= 0 && x <= 6 && y >= 0 && y <= 6);
}

//将光标移动到指定位置
void gotoxy1(int X, int Y)
{
	COORD coord;
	coord.X = X;
	coord.Y = Y;
	SetConsoleCursorPosition(hout, coord);
}

//将光标移动到指定盘格
void gotoxy(int X, int Y)
{
	gotoxy1(4 * X + 6, 2 * Y + 8);
}

//在指定位置处打印一个指定的字符
void showch1(int X, int Y, char ch)
{
	gotoxy1(X, Y);
	putchar(ch);
}

//在指定坐标处打印两个字符 0是空位 1是黑棋，-1是白棋 2是选中的黑棋 -2是选中的白棋
void showch(int X, int Y, int chess)
{
	gotoxy(X, Y);
	printf(chestr[chess + 2]);
}

//显示步骤
void showstep(int color, int round)
{
	int blackchess = 0, whitechess = 0;
	for (int i = 0; i <= 6; i++)
	{
		for (int j = 0; j <= 6; j++)
		{
			if (map[i][j] == 1)
				blackchess++;
			if (map[i][j] == -1)
				whitechess++;
		}
	}
	gotoxy1(1, 2);
	cout << "当前回合数  " << myround;
	gotoxy1(1, 3);
	cout << "黑棋（" << playername[blackplayer] << "）○ " << blackchess;

	gotoxy1(1, 4);
	cout << "白棋（" << playername[whiteplayer] << "）● " << whitechess;
	if (!(turn == 1 && myround == 1))
	{
		gotoxy1(2, 23);
		char str1[2][5] = { "白","黑" };
		printf("第%d回合，%s方从(%d,%d)走到(%d,%d)", round + (color == 1), str1[(color + 1) / 2], lastFromY, lastFromX, lastToY, lastToX);
	}
	gotoxy(arrowX, arrowY);
}

//绘制格子
void draw_table()
{
	cls();
	gotoxy1(0, 0);
	cout << "┌ ─ ─ ─ ─ ─ ─ ─ ─ ┬ ─ ─ ─ ─ ─ ─ ─ ┐\n";
	if (REC == 1)
		cout << "│    回放记录     │ 上下左右：移动│\n";
	else
		cout << "│   游戏进行中    │ 上下左右：移动│\n";
	cout << "│                 │ 空格:选择/取消│\n";
	cout << "│                 │ B:悔棋  S:保存│\n";
	cout << "│                 │ Q:退出游戏    │\n";
	cout << "├ ─ ─ ─ ─ ─ ─ ─ ─ ┴ ─ ─ ─ ─ ─ ─ ─ ┤\n";
	cout << "│     ０  １  ２  ３  ４  ５  ６  │\n";
	cout << "│   ┌ ─ ┬ ─ ┬ ─ ┬ ─ ┬ ─ ┬ ─ ┬ ─ ┐ │\n";
	cout << "│ ０│   │   │   │   │   │   │   │ │\n";
	cout << "│   ├ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┤ │\n";
	cout << "│ １│   │   │   │   │   │   │   │ │\n";
	cout << "│   ├ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┤ │\n";
	cout << "│ ２│   │   │   │   │   │   │   │ │\n";
	cout << "│   ├ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┤ │\n";
	cout << "│ ３│   │   │   │   │   │   │   │ │\n";
	cout << "│   ├ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┤ │\n";
	cout << "│ ４│   │   │   │   │   │   │   │ │\n";
	cout << "│   ├ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┤ │\n";
	cout << "│ ５│   │   │   │   │   │   │   │ │\n";
	cout << "│   ├ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┤ │\n";
	cout << "│ ６│   │   │   │   │   │   │   │ │\n";
	cout << "│   └ ─ ┴ ─ ┴ ─ ┴ ─ ┴ ─ ┴ ─ ┴ ─ ┘ │\n";
	cout << "├ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┤\n";
	cout << "│                                 │\n";
	cout << "└ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┘\n";
}

//绘制当前棋盘
void draw_border()
{
	draw_table();
	for (int i = 0; i <= 6; i++)
	{
		for (int j = 0; j <= 6; j++)
		{
			showch(i, j, map[i][j]);
		}
	}
	gotoxy(arrowX, arrowY);
	showstep(-turn, myround - 1);
}

//初始化新棋局函数
void init_map()
{
	memset(map, 0, sizeof(map));
	memset(backmap, 0, sizeof(backmap));
	memset(historystep, 0, sizeof(historystep));
	map[0][0] = 1;
	map[6][0] = -1;
	map[0][6] = -1;
	map[6][6] = 1;
	arrowX = 3;
	arrowY = 3;
	myround = 1;
	turn = 1;
	backmap[0][0][0] = -2;
	backmap[1][0][0] = -2;
	lastFromX = 0;
	lastFromY = 0;
	lastToX = 0;
	lastToY = 0;
}

//移动操作
void move(int map[7][7], int fromX, int fromY, int toX, int toY, int color)
{
	if (fabs(fromX - toX) == 2 || fabs(fromY - toY) == 2)
	{
		map[fromX][fromY] = 0;
	}
	map[toX][toY] = color;
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			if (inmap(toX + dx, toY + dy) && map[toX + dx][toY + dy] == -color)
			{
				map[toX + dx][toY + dy] = color;
			}
		}
	}
	return;
}

//真实移动操作
void mapmove(int fromX, int fromY, int toX, int toY, int color)
{
	int totstep = (myround - 1) * 2 + (color == 1 ? 1 : 2);
	historystep[totstep][0] = color;
	historystep[totstep][1] = fromX;
	historystep[totstep][2] = fromY;
	historystep[totstep][3] = toX;
	historystep[totstep][4] = toY;
	move(map, fromX, fromY, toX, toY, color);
	lastFromX = fromX;
	lastFromY = fromY;
	lastToX = toX;
	lastToY = toY;
}

//评估棋局函数
int evaluate(int map[7][7], int color, int color2)
{
	bool flag = 1;
	int a[3] = { 0,0,0 };
	for (int i = 0; i <= 6; i++)
	{
		for (int j = 0; j <= 6; j++)
		{
			a[map[i][j] + 1]++;
			if (map[i][j] == color2 && flag)
			{
				for (int ii = -2; ii <= 2; ii++)
				{
					for (int jj = -2; jj <= 2; jj++)
					{
						if (inmap(i + ii, j + jj) && map[i + ii][j + jj] == 0)
						{
							flag = 0;
							break;
						}
					}
				}
			}
		}
	}
	if (flag)
		return 1000 * (a[color + 1] + a[1] - a[-color + 1]);
	else
		return a[color + 1] - a[-color + 1];
}

//搜索子节点函数
int search(int lastmap[7][7], int depth, int alpha, int beta, int color)
{
	bool minlayer = depth % 2;//第一层是0层，奇数层是min层，偶数层是max层
	int color2 = minlayer ? -color : color;//min轮到对方下，max轮到自己下；
	if (depth == maxdepth)
	{
		return evaluate(lastmap, color, color2);
	}
	bool finalnode = 1;
	int val = minlayer ? INT_MAX : INT_MIN;//初始化这个点的值，为MAX和MIN，
	for (int i = 0; i <= 6; i++)
	{
		for (int j = 0; j <= 6; j++)
		{
			if (lastmap[i][j] != color2)
				continue;
			for (int k = 0; k <= 23; k++)
			{
				int ii = delta[(randnum + k) % 24][0], jj = delta[(randnum + k) % 24][1];
				if (!inmap(i + ii, j + jj))
				{
					continue;
				}
				else if (lastmap[i + ii][j + jj] != 0)
				{
					continue;
				}
				else
				{	//找到一个向下的分支
					finalnode = 0;
					int thismap[7][7];//创建新地图
					memcpy(thismap, lastmap, sizeof(map));//复制地图
					move(thismap, i, j, i + ii, j + jj, color2);//进行移动
					int theval = search(thismap, depth + 1, alpha, beta, color);//值从下一点返回
					if (minlayer)
					{
						val = min(val, theval);//更新最小值
						beta = min(theval, beta);//更新β
					}
					else
					{
						val = max(val, theval);//更新最大值
						alpha = max(theval, alpha);//更新α
					}
					if (alpha >= beta)
					{
						return minlayer ? INT_MIN : INT_MAX;
						//剪枝操作，给最小层赋min值，最大层赋max值，让它一定不会被选中
					}
				}
			}
		}
	}
	if (finalnode)
		return evaluate(lastmap, color, color2);
	else
		return val;
}

//存档
void menu_save()
{
	char filename[128];
	fstream fileexist;
	cls();
	while (1)
	{
		cout << "存档保存" << endl << "请输入要保存的文件名（不含后缀，留空则返回游戏）" << endl;
		cin.clear();
		cin.sync();
		cin.getline(filename, 120);
		if (filename[0] == '\0')
			return;
		strcat(filename, ".thq");
		fileexist.open(filename, ios::in);
		if (!fileexist)
		{
			fileexist.close();
			break;
		}
		PlaySound(TEXT("./sound/error.wav"), NULL, SND_FILENAME | SND_ASYNC);
		cls();
		gotoxy1(0, 3);
		cout << "文件已存在！";
		gotoxy1(0, 0);
		fileexist.close();
	}
	ofstream fileout(filename);
	fileout << "ataxxsavefile\n";
	fileout << myround << ' ' << turn << ' ';
	fileout << arrowX << ' ' << arrowY << ' ';
	fileout << blackplayer << ' ' << whiteplayer << ' ';
	fileout << blackdepth << ' ' << whitedepth << ' ';
	fileout << lastFromX << ' ' << lastFromY << ' ' << lastToX << ' ' << lastToY << ' ';
	for (int i = 0; i <= 6; i++)
	{
		for (int j = 0; j <= 6; j++)
		{
			fileout << map[i][j] << ' ';
		}
	}
	int totstep = (myround - 1) * 2 + (turn == 1 ? 1 : 2) - 1;
	fileout << totstep << ' ';
	for (int i = 1; i <= totstep; i++)
	{
		for (int j = 0; j <= 4; j++)
		{
			fileout << historystep[i][j] << ' ';
		}
	}
	fileout.close();
	PlaySound(TEXT("./sound/loadsuc.wav"), NULL, SND_FILENAME | SND_ASYNC);
	cout << "文件保存成功！\n按Enter返回游戏" << endl;
	while (_getch() != '\r');
	return;
}

//玩家回合
int player(int color)
{
	char c;
	//输入
	int souX = 4;
	int souY = 4;
	//被选中的棋子xy坐标
	bool sel = false;
	//是否被选中
	while (1)
	{
		c = _getch();
		switch (c)
		{
		case -32:
			switch (c = _getch())
			{
			case 72:arrowY = (arrowY - 1 >= 0 ? arrowY - 1 : 6); break;
			case 80:arrowY = (arrowY + 1 <= 6 ? arrowY + 1 : 0); break;
			case 75:arrowX = (arrowX - 1 >= 0 ? arrowX - 1 : 6); break;
			case 77:arrowX = (arrowX + 1 <= 6 ? arrowX + 1 : 0); break;
			}
			gotoxy(arrowX, arrowY);
			break;
		case ' ':
			if (sel == 0)
			{
				if (map[arrowX][arrowY] == color)
				{
					sel = 1;
					souX = arrowX;
					souY = arrowY;
					showch(souX, souY, 2 * map[souX][souY]);
					gotoxy(arrowX, arrowY);
				}
			}
			else
			{
				if (map[arrowX][arrowY] == 0 && fabs(souX - arrowX) <= 2 && fabs(souY - arrowY) <= 2)
				{
					memcpy(backmap[color == -1], map, sizeof(map));
					mapmove(souX, souY, arrowX, arrowY, color);
					lastFromX = souX;
					lastFromY = souY;
					lastToX = arrowX;
					lastToY = arrowY;
					sel = 0;
					return 0;//出口
				}
				else if (arrowX == souX && arrowY == souY)
				{
					sel = 0;
					showch(souX, souY, map[souX][souY]);
					gotoxy(arrowX, arrowY);
				}
			}
			break;
		case 's':
		case 'S':
			PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
			menu_save();
			draw_border();
			PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'b':
		case 'B':
			cls();
			if (backmap[color == -1][0][0] == -2)
			{
				PlaySound(TEXT("./sound/error.wav"), NULL, SND_FILENAME | SND_ASYNC);
				cout << "当前不允许悔棋" << endl << "即将自动返回";
				Sleep(1500);
			}
			else
			{
				PlaySound(TEXT("./sound/ask.wav"), NULL, SND_FILENAME | SND_ASYNC);
				cout << "你确定要悔棋吗（按Y确定，按N取消）" << endl;
				char e;
				e = 0;
				while (e != 'y' && e != 'Y' && e != 'N' && e != 'n')
				{
					e = _getch();
				}
				if (e == 'y' || e == 'Y')
				{
					return -2;
				}
			}
			PlaySound(TEXT("./sound/well.wav"), NULL, SND_FILENAME | SND_ASYNC);
			draw_border();
			break;
		case 'q':
		case 'Q':
			PlaySound(TEXT("./sound/ask.wav"), NULL, SND_FILENAME | SND_ASYNC);
			cls();
			cout << "你确定要退出游戏吗？(按Y确定，按N取消)";
			char b;
			b = 0;
			while (b != 'y' && b != 'Y' && b != 'N' && b != 'n')
			{
				b = _getch();
			}
			if (b == 'y' || b == 'Y')
			{
				PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
				return -1;
			}
			draw_border();
			PlaySound(TEXT("./sound/well.wav"), NULL, SND_FILENAME | SND_ASYNC);
			break;
		}
	}
}

//随机机器人回合
int randBot(int color)
{
	clock_t time1, time2;
	time1 = clock();
	int beginPos[1000][2], possiblePos[1000][2], posCount = 0, choice;

	for (int i = 0; i <= 6; i++)
	{
		for (int j = 0; j <= 6; j++)
		{
			if (map[i][j] != color)
				continue;
			for (int ii = -2; ii <= 2; ii++)
			{
				for (int jj = -2; jj <= 2; jj++)
				{
					if (!inmap(i + ii, j + jj))
						continue;
					if (map[i + ii][j + jj] != 0)
						continue;
					beginPos[posCount][0] = i;
					beginPos[posCount][1] = j;
					possiblePos[posCount][0] = i + ii;
					possiblePos[posCount][1] = j + jj;
					posCount++;
				}
			}
		}
	}
	int startX, startY, resultX, resultY;
	if (posCount > 0)
	{
		choice = rand() % posCount;
		startX = beginPos[choice][0];
		startY = beginPos[choice][1];
		resultX = possiblePos[choice][0];
		resultY = possiblePos[choice][1];
	}
	else
	{
		startX = -1;
		startY = -1;
		resultX = -1;
		resultY = -1;
	}
	time2 = clock();
	if ((time2 - time1) <= waittime1)
		Sleep(waittime1 - (time2 - time1));
	mapmove(startX, startY, resultX, resultY, color);
	return 0;
}

//搜索机器人回合
int Bot(int color)
{
	randnum = rand() % 24;
	clock_t time1, time2;
	time1 = clock();
	maxdepth = (color == 1 ? blackdepth : whitedepth);
	int bestFromX = -1, bestFromY = -1, bestToX = -1, bestToY = -1;
	int val = INT_MIN;
	int alpha = INT_MIN, beta = INT_MAX;
	for (int i = 0; i <= 6; i++)
	{
		for (int j = 0; j <= 6; j++)
		{
			if (map[i][j] != color)
				continue;
			for (int k = 0; k <= 23; k++)
			{
				int ii = delta[(randnum + k) % 24][0], jj = delta[(randnum + k) % 24][1];
				if (!inmap(i + ii, j + jj))
					continue;
				else if (map[i + ii][j + jj] != 0)
					continue;
				{
					int thismap[7][7];
					memcpy(thismap, map, sizeof(map));
					move(thismap, i, j, i + ii, j + jj, color);
					int thisval = search(thismap, 1, alpha, beta, color);
					if (thisval > val)
					{
						val = thisval;
						bestFromX = i;
						bestFromY = j;
						bestToX = i + ii;
						bestToY = j + jj;
					}
				}
			}
		}
	}
	time2 = clock();
	if ((time2 - time1) <= waittime1)
		Sleep(waittime1 - (time2 - time1));
	if (bestFromX != -1)
	{
		mapmove(bestFromX, bestFromY, bestToX, bestToY, color);
		return 0;
	}
	else
	{
		randBot(color);
		return 0;
	}
}

//判断谁赢了
int win(int color)
{
	for (int i = 0; i <= 6; i++)
	{
		for (int j = 0; j <= 6; j++)
		{
			if (map[i][j] == color)
			{
				for (int ii = -2; ii <= 2; ii++)
				{
					for (int jj = -2; jj <= 2; jj++)
					{
						if (inmap(i + ii, j + jj) && (map[i + ii][j + jj] == 0))
						{
							return 0;
						}
					}
				}
			}
		}
	}
	int black = 0;
	int white = 0;
	for (int i = 0; i <= 6; i++)
	{
		for (int j = 0; j <= 6; j++)
		{
			if (map[i][j] == 1)
				black++;
			else if (map[i][j] == -1)
				white++;
			else if (map[i][j] == 0 && color == 1)
				white++;
			else
				black++;
		}
	}
	if (black > white)
	{
		PlaySound(TEXT("./sound/cat.wav"), NULL, SND_FILENAME | SND_ASYNC);
		return 1;
	}
	else if (black < white)
	{
		PlaySound(TEXT("./sound/dog.wav"), NULL, SND_FILENAME | SND_ASYNC);
		return -1;
	}
	else return 0;
}

//悔棋
void chessback(int color)
{
	int f = (color == -1);
	memcpy(map, backmap[f], sizeof(map));
	memset(backmap, 0, sizeof(backmap));
	backmap[0][0][0] = -2;
	backmap[1][0][0] = -2;
	myround--;
	int totstep = (myround - 1) * 2 + (turn == 1 ? 1 : 2);
	lastFromX = historystep[totstep - 1][1];
	lastFromY = historystep[totstep - 1][2];
	lastToX = historystep[totstep - 1][3];
	lastToY = historystep[totstep - 1][4];
	PlaySound(TEXT("./sound/back.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

//保存回放
void menu_savereplay()
{
	char filename[128];
	fstream fileexist;
	cls();
	while (1)
	{
		cout << "回放保存" << endl << "请输入要保存的文件名（不含后缀，留空则返回）" << endl;
		cin.clear();
		cin.sync();
		cin.getline(filename, 120);
		if (filename[0] == '\0')
		{
			PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
			return;
		}
		strcat(filename, ".rep");
		fileexist.open(filename, ios::in);
		if (!fileexist)
		{
			fileexist.close();
			break;
		}
		PlaySound(TEXT("./sound/error.wav"), NULL, SND_FILENAME | SND_ASYNC);
		cls();
		gotoxy1(0, 3);
		cout << "文件已存在！";
		gotoxy1(0, 0);
		fileexist.close();
	}
	ofstream fileout(filename);
	fileout << "ataxxreplayfile\n";
	fileout << blackplayer << ' ' << whiteplayer << ' ';
	fileout << blackdepth << ' ' << whitedepth << ' ';
	int totstep = (myround - 1) * 2 + (turn == 1 ? 1 : 2) - 1;
	fileout << totstep << ' ';
	for (int i = 1; i <= totstep; i++)
	{
		for (int j = 0; j <= 4; j++)
		{
			fileout << historystep[i][j] << ' ';
		}
	}
	fileout.close();
	PlaySound(TEXT("./sound/loadsuc.wav"), NULL, SND_FILENAME | SND_ASYNC);
	cout << "回放文件保存成功！\n按Enter返回" << endl;
	while (_getch() != '\r');
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	return;
}

//读取回放
void menu_readreplay()
{
	char str[1028];
	char filename[128];
	fstream fileexist;
	cls();
	while (1)
	{
		cout << "回放读取" << endl << "请输入要读取的文件名（不含后缀，留空直接回车则返回主菜单）" << endl;
		cin.clear();
		cin.sync();
		cin.getline(filename, 120);
		if (filename[0] == '\0')
		{
			PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME);
			menu0 = '0';
			return;
		}
		strcat(filename, ".rep");
		fileexist.open(filename, ios::in);
		if (!fileexist)
		{
			PlaySound(TEXT("./sound/error.wav"), NULL, SND_FILENAME | SND_ASYNC);
			cls();
			gotoxy1(0, 3);
			cout << "文件不存在！";
			gotoxy1(0, 0);
		}
		else
		{
			fileexist.getline(str, 1024);
			if (strcmp(str, "ataxxreplayfile"))
			{
				PlaySound(TEXT("./sound/error.wav"), NULL, SND_FILENAME | SND_ASYNC);
				cls();
				gotoxy1(0, 3);
				cout << "这不是同化棋回放记录";
				gotoxy1(0, 0);
			}
			else
			{
				fileexist.close();
				break;
			}
		}
		fileexist.close();
	}
	ifstream filein(filename);
	init_map();
	filein.getline(filename, 1024);
	filein >> blackplayer >> whiteplayer;
	filein >> blackdepth >> whitedepth;
	int totstep;
	filein >> totstep;
	for (int i = 1; i <= totstep; i++)
	{
		for (int j = 0; j <= 4; j++)
		{
			filein >> historystep[i][j];
		}
	}
	filein.close();
	PlaySound(TEXT("./sound/loadsuc.wav"), NULL, SND_FILENAME | SND_ASYNC);
	cout << "文件读取成功！\n按Enter开始回放" << endl;
	while (_getch() != '\r');
	REC = 1;
	PlaySound(TEXT("./sound/chest.wav"), NULL, SND_FILENAME | SND_ASYNC);
	for (int i = 1; i <= totstep; i++)
	{
		draw_border();
		Sleep(waittime1);
		if (turn == 1)
			PlaySound(TEXT("./sound/stone.wav"), NULL, SND_FILENAME | SND_ASYNC);
		else if (turn == -1)
			PlaySound(TEXT("./sound/wood.wav"), NULL, SND_FILENAME | SND_ASYNC);
		move(map, historystep[i][1], historystep[i][2], historystep[i][3], historystep[i][4], historystep[i][0]);
		lastFromX = historystep[i][1];
		lastFromY = historystep[i][2];
		lastToX = historystep[i][3];
		lastToY = historystep[i][4];
		if (turn == -1)
			myround++;
		turn = -turn;
	}
	draw_border();
	int winner = win(turn);
	gotoxy1(0, 25);
	printf("游戏结束，%s方（%s）胜利！\n", winner == 1 ? "黑" : "白", winner == 1 ? playername[blackplayer] : playername[whiteplayer]);
	printf("回放完成，按Esc返回主菜单。");
	while (_getch() != 27);
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	menu0 = '0';
	return;

}

//主菜单
void menu_main()
{
	cout << "同化棋" << endl;
	cout << "1.新游戏" << endl;
	cout << "2.读档" << endl;
	cout << "3.回放" << endl;
	cout << "4.帮助" << endl;
	cout << "Esc.退出" << endl;
	while (menu0 != '1' && menu0 != '2' && menu0 != '3' && menu0 != '4' && menu0 != 27)
	{
		menu0 = _getch();
	}
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

//在游戏中
void ingame()
{
	PlaySound(TEXT("./sound/chest.wav"), NULL, SND_FILENAME | SND_ASYNC);
	int freturn = 0;
	int winner = 0;
	REC = 0;
	while (!winner)
	{
		draw_border();
		if (turn == 1)
		{
			freturn = (*f[blackplayer])(1);
		}
		else
		{
			freturn = (*f[whiteplayer])(-1);
		}
		if (freturn == -1)
		{
			menu0 = '0';
			return;
		}
		else if (freturn == -2)
		{
			chessback(turn);
		}
		else
		{
			if (turn == 1)
				PlaySound(TEXT("./sound/stone.wav"), NULL, SND_FILENAME | SND_ASYNC);
			else if (turn == -1)
				PlaySound(TEXT("./sound/wood.wav"), NULL, SND_FILENAME | SND_ASYNC);
			if (turn == -1)
				myround++;
			turn = -turn;
			winner = win(turn);
		}
	}
	draw_border();
	gotoxy1(0, 25);
	printf("游戏结束，%s方（%s）胜利！\n", winner == 1 ? "黑" : "白", winner == 1 ? playername[blackplayer] : playername[whiteplayer]);
	printf("按R保存回放，按Esc返回主菜单。");
	char ch = 0;
	while (ch != 27)
	{
		ch = _getch();
		if (ch == 'R' || ch == 'r')
		{
			PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
			menu_savereplay();
			draw_border();
			gotoxy1(0, 26);
			printf("游戏结束，%s方（%s）胜利！\n", winner == 1 ? "黑" : "白", winner == 1 ? playername[blackplayer] : playername[whiteplayer]);
			printf("按R保存回放，按Esc返回主菜单。");
		}
	}
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	menu0 = '0';
	return;

}

//新游戏
void menu_new()
{
	char a = 0, b = 0, c = 0, a1 = 0, b1 = 0;
	cout << "新游戏" << endl;
	cout << "你可以在任何地方按Esc返回主菜单" << endl << endl;
	cout << "选择黑色棋子由谁控制（1.玩家，2.机器，3.随机）" << endl;
	while (a != '1' && a != '2' && a != '3' && a != 27)
	{
		a = _getch();
	}
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	if (a == 27)
	{
		menu0 = '0';
		return;
	}
	cout << "黑色棋子将由" << playername[a - '0'] << "控制" << endl << endl;
	if (a == '2')
	{
		cout << "选择机器的智商：(1-4)" << endl;
		while (a1 != '1' && a1 != '2' && a1 != '3' && a1 != '4' && a1 != 27)
		{
			a1 = _getch();
		}
		PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
		if (a1 == 27)
		{
			menu0 = '0';
			return;
		}
		cout << "黑棋机器智商为：" << a1 << endl << endl;
	}
	cout << "选择白色棋子由谁控制（1.玩家，2.机器，3.随机）" << endl;
	while (b != '1' && b != '2' && b != '3' && b != 27)
	{
		b = _getch();
	}
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	if (b == 27)
	{
		menu0 = '0';
		return;
	}
	cout << "白色棋子将由" << playername[b - '0'] << "控制" << endl << endl;
	if (b == '2')
	{
		cout << "选择机器的智商：(1-4)" << endl;
		while (b1 != '1' && b1 != '2' && b1 != '3' && b1 != '4' && b1 != 27)
		{
			b1 = _getch();
		}
		PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
		if (b1 == 27)
		{
			menu0 = '0';
			return;
		}
		cout << "白棋机器智商为：" << b1 << endl << endl;
	}
	cout << "选择完成，按Enter开始游戏，按Esc键退出" << endl;
	while (c != 27 && c != '\r')
	{
		c = _getch();
	}
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	if (c == 27)
	{
		menu0 = '0';
		return;
	}
	blackplayer = a - '0';
	whiteplayer = b - '0';
	blackdepth = a1 - '0';
	whitedepth = b1 - '0';
	menu0 = '5';
	init_map();
}

//读档
void menu_load()
{
	char str[1028];
	char filename[128];
	fstream fileexist;
	cls();
	while (1)
	{
		cout << "存档读取" << endl << "请输入要读取的文件名（不含后缀，留空直接回车则返回主菜单）" << endl;
		cin.clear();
		cin.sync();
		cin.getline(filename, 120);
		if (filename[0] == '\0')
		{
			PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
			menu0 = '0';
			return;
		}
		strcat(filename, ".thq");
		fileexist.open(filename, ios::in);
		if (!fileexist)
		{
			PlaySound(TEXT("./sound/error.wav"), NULL, SND_FILENAME | SND_ASYNC);
			cls();
			gotoxy1(0, 3);
			cout << "文件不存在！";
			gotoxy1(0, 0);
		}
		else
		{
			fileexist.getline(str, 1024);
			if (strcmp(str, "ataxxsavefile"))
			{
				PlaySound(TEXT("./sound/error.wav"), NULL, SND_FILENAME | SND_ASYNC);
				cls();
				gotoxy1(0, 3);
				cout << "这不是同化棋存档";
				gotoxy1(0, 0);
			}
			else
			{
				fileexist.close();
				break;
			}
		}
		fileexist.close();
	}
	ifstream filein(filename);
	filein.getline(filename, 1024);
	filein >> myround >> turn;
	filein >> arrowX >> arrowY;
	filein >> blackplayer >> whiteplayer;
	filein >> blackdepth >> whitedepth;
	filein >> lastFromX >> lastFromY >> lastToX >> lastToY;
	for (int i = 0; i <= 6; i++)
	{
		for (int j = 0; j <= 6; j++)
		{
			filein >> map[i][j];
		}
	}
	int totstep;
	filein >> totstep;
	for (int i = 1; i <= totstep; i++)
	{
		for (int j = 0; j <= 4; j++)
		{
			filein >> historystep[i][j];
		}
	}
	backmap[0][0][0] = -2;
	backmap[1][0][0] = -2;
	filein.close();
	PlaySound(TEXT("./sound/loadsuc.wav"), NULL, SND_FILENAME | SND_ASYNC);
	cout << "文件读取成功！\n按Enter开始游戏" << endl;
	while (_getch() != '\r');
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	menu0 = '5';
	return;
}

//帮助
void menu_help()
{
	cls();
	cout << "帮助：\n";
	cout << "\n1.操作方法：\n";
	cout << "当轮到你的回合时，你可以使用键盘进行操作。\n";
	cout << "使用上下左右键移动光标，越过地图边界可以循环。\n";
	cout << "使用空格键选择你的起始棋子，选择后棋子将变为菱形。\n";
	cout << "再使用空格键选择你的目标位置。正确操作后棋子将移动或复制。\n";
	cout << "如果想撤销选择棋子，只需要再对棋子按下空格即可。\n";
	cout << "按B键可以悔棋，不能一次性进行两次悔棋\n";
	cout << "按S键可以选择保存游戏进度，输入一个文件名，游戏将保存到文件名.thq\n";
	cout << "保存的存档可以在主菜单的“读取”中使用\n";
	cout << "按Q键可以选择返回到主菜单\n";
	cout << "游戏结束时，按R键可以选择保存游戏回放，输入一个文件名，回放将保存到文件名.rep\n";
	cout << "\n2.游戏规则\n";
	cout << "这里的同化棋游戏与国际通行的同化棋规则基本一致。\n";
	cout << "同化棋是一种二人棋盘游戏，黑白双方交替在7x7的棋盘上移动棋子。\n";
	cout << "玩家每次可以移动的位置是有限的，具体步骤解释如下：\n";
	cout << "选取现在在棋盘上己方的一枚棋子，再选择一个落子位置。要求落子处为空，既没有对方也没有己方的棋子。\n";
	cout << "选择的落子位置可以是以下二者之一：\n";
	cout << "落子位置在以选取的棋子为中心的3x3的范围内。此时选取的棋子会复制自身到落子位置。一共有8个位置可以选择。\n";
	cout << "落子位置在以选取的棋子为中心的3x3的范围外、5x5的范围内，此时选取的棋子会移动自身到落子位置，一共有16个位置可以选择。\n";
	cout << "总的来说，移动一格是“复制粘贴”，移动两格是“剪切粘贴”。\n";
	cout << "每次落子后，新位置的落子会将落子处紧邻的8个位置的棋子（如果有）都变成自己的棋子。\n";
	cout << "若没有上述这种位置则游戏结束，将所有无子的地方算作有子可下一方的棋子，然后数出双方棋子数，棋多者胜。\n";
	cout << "任意时刻对方棋子数为0时，己方胜利。\n";
	cout << "如果局面发生循环，即有一方的一步使得局面回到了曾经出现过的一个局面，那么对方将不能走出和当时一样的一步。否则会被判为不合法输入。\n";
	cout << "游戏回合数到400时，游戏会直接结束，数出双方棋子数，棋多者胜。\n";
	cout << endl << "按Esc返回";
	while (_getch() != 27);
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	menu0 = '0';
}

//主函数
int main()
{
	srand((unsigned)time(NULL));
	PlaySound(TEXT("./sound/lvlup.wav"), NULL, SND_FILENAME | SND_ASYNC);
	while (menu0 != 27)
	{
		cls();
		switch (menu0)
		{
		case '0':menu_main(); break;
		case '1':menu_new(); break;
		case '2':menu_load(); break;
		case '3':menu_readreplay(); break;
		case '4':menu_help(); break;
		case '5':ingame(); break;
		default:menu_main(); break;
		}
	}
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME);
	cls();
	return 0;
}