//ͬ���� 
//������ 2100013124
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
const char chestr[10][5] = { "��","��","  ","��","��" };//�����ַ�
char playername[4][11] = { "","���","����","���" };
char blackwhite[2][5] = { "��","��" };
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } };
bool REC = 0;
int randnum;

int map[7][7] = { 0 };//��ͼ
int arrowX = 3, arrowY = 3;//���λ��
int myround;//�غ���
int turn;//�ֵ�˭
int lastFromX, lastFromY, lastToX, lastToY;//��һ���ƶ�
int historystep[10000][5];//��ʷ����
int backmap[2][7][7] = { 0 };//��������

int blackplayer = 0;
int whiteplayer = 0;//�ڰ�������˭����
int blackdepth = 4;//�������������
int whitedepth = 4;//�������������

char menu0 = '0';//�˵�
int maxdepth = 4;//bot��������
int waittime1 = 750;//�ȴ�ʱ��

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

//���������Ļ������
void cls()
{
	COORD coord = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO binfo; /* to get buffer info */
	DWORD num;

	/* ȡ��ǰ��������Ϣ */
	GetConsoleScreenBufferInfo(hout, &binfo);
	/* ����ַ� */
	FillConsoleOutputCharacter(hout, (TCHAR)' ', binfo.dwSize.X * binfo.dwSize.Y, coord, &num);
	/* ������� */
	FillConsoleOutputAttribute(hout, binfo.wAttributes, binfo.dwSize.X * binfo.dwSize.Y, coord, &num);

	/* ���ص�(0,0) */
	SetConsoleCursorPosition(hout, coord);
	return;
}

//�ж�һ��λ���ǲ����ڵ�ͼ��
inline bool inmap(int x, int y)
{
	return(x >= 0 && x <= 6 && y >= 0 && y <= 6);
}

//������ƶ���ָ��λ��
void gotoxy1(int X, int Y)
{
	COORD coord;
	coord.X = X;
	coord.Y = Y;
	SetConsoleCursorPosition(hout, coord);
}

//������ƶ���ָ���̸�
void gotoxy(int X, int Y)
{
	gotoxy1(4 * X + 6, 2 * Y + 8);
}

//��ָ��λ�ô���ӡһ��ָ�����ַ�
void showch1(int X, int Y, char ch)
{
	gotoxy1(X, Y);
	putchar(ch);
}

//��ָ�����괦��ӡ�����ַ� 0�ǿ�λ 1�Ǻ��壬-1�ǰ��� 2��ѡ�еĺ��� -2��ѡ�еİ���
void showch(int X, int Y, int chess)
{
	gotoxy(X, Y);
	printf(chestr[chess + 2]);
}

//��ʾ����
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
	cout << "��ǰ�غ���  " << myround;
	gotoxy1(1, 3);
	cout << "���壨" << playername[blackplayer] << "���� " << blackchess;

	gotoxy1(1, 4);
	cout << "���壨" << playername[whiteplayer] << "���� " << whitechess;
	if (!(turn == 1 && myround == 1))
	{
		gotoxy1(2, 23);
		char str1[2][5] = { "��","��" };
		printf("��%d�غϣ�%s����(%d,%d)�ߵ�(%d,%d)", round + (color == 1), str1[(color + 1) / 2], lastFromY, lastFromX, lastToY, lastToX);
	}
	gotoxy(arrowX, arrowY);
}

//���Ƹ���
void draw_table()
{
	cls();
	gotoxy1(0, 0);
	cout << "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	if (REC == 1)
		cout << "��    �طż�¼     �� �������ң��ƶ���\n";
	else
		cout << "��   ��Ϸ������    �� �������ң��ƶ���\n";
	cout << "��                 �� �ո�:ѡ��/ȡ����\n";
	cout << "��                 �� B:����  S:���橦\n";
	cout << "��                 �� Q:�˳���Ϸ    ��\n";
	cout << "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	cout << "��     ��  ��  ��  ��  ��  ��  ��  ��\n";
	cout << "��   �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	cout << "�� ����   ��   ��   ��   ��   ��   ��   �� ��\n";
	cout << "��   �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	cout << "�� ����   ��   ��   ��   ��   ��   ��   �� ��\n";
	cout << "��   �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	cout << "�� ����   ��   ��   ��   ��   ��   ��   �� ��\n";
	cout << "��   �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	cout << "�� ����   ��   ��   ��   ��   ��   ��   �� ��\n";
	cout << "��   �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	cout << "�� ����   ��   ��   ��   ��   ��   ��   �� ��\n";
	cout << "��   �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	cout << "�� ����   ��   ��   ��   ��   ��   ��   �� ��\n";
	cout << "��   �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	cout << "�� ����   ��   ��   ��   ��   ��   ��   �� ��\n";
	cout << "��   �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	cout << "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
	cout << "��                                 ��\n";
	cout << "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��\n";
}

//���Ƶ�ǰ����
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

//��ʼ������ֺ���
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

//�ƶ�����
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

//��ʵ�ƶ�����
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

//������ֺ���
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

//�����ӽڵ㺯��
int search(int lastmap[7][7], int depth, int alpha, int beta, int color)
{
	bool minlayer = depth % 2;//��һ����0�㣬��������min�㣬ż������max��
	int color2 = minlayer ? -color : color;//min�ֵ��Է��£�max�ֵ��Լ��£�
	if (depth == maxdepth)
	{
		return evaluate(lastmap, color, color2);
	}
	bool finalnode = 1;
	int val = minlayer ? INT_MAX : INT_MIN;//��ʼ��������ֵ��ΪMAX��MIN��
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
				{	//�ҵ�һ�����µķ�֧
					finalnode = 0;
					int thismap[7][7];//�����µ�ͼ
					memcpy(thismap, lastmap, sizeof(map));//���Ƶ�ͼ
					move(thismap, i, j, i + ii, j + jj, color2);//�����ƶ�
					int theval = search(thismap, depth + 1, alpha, beta, color);//ֵ����һ�㷵��
					if (minlayer)
					{
						val = min(val, theval);//������Сֵ
						beta = min(theval, beta);//���¦�
					}
					else
					{
						val = max(val, theval);//�������ֵ
						alpha = max(theval, alpha);//���¦�
					}
					if (alpha >= beta)
					{
						return minlayer ? INT_MIN : INT_MAX;
						//��֦����������С�㸳minֵ�����㸳maxֵ������һ�����ᱻѡ��
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

//�浵
void menu_save()
{
	char filename[128];
	fstream fileexist;
	cls();
	while (1)
	{
		cout << "�浵����" << endl << "������Ҫ������ļ�����������׺�������򷵻���Ϸ��" << endl;
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
		cout << "�ļ��Ѵ��ڣ�";
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
	cout << "�ļ�����ɹ���\n��Enter������Ϸ" << endl;
	while (_getch() != '\r');
	return;
}

//��һغ�
int player(int color)
{
	char c;
	//����
	int souX = 4;
	int souY = 4;
	//��ѡ�е�����xy����
	bool sel = false;
	//�Ƿ�ѡ��
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
					return 0;//����
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
				cout << "��ǰ���������" << endl << "�����Զ�����";
				Sleep(1500);
			}
			else
			{
				PlaySound(TEXT("./sound/ask.wav"), NULL, SND_FILENAME | SND_ASYNC);
				cout << "��ȷ��Ҫ�����𣨰�Yȷ������Nȡ����" << endl;
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
			cout << "��ȷ��Ҫ�˳���Ϸ��(��Yȷ������Nȡ��)";
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

//��������˻غ�
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

//���������˻غ�
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

//�ж�˭Ӯ��
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

//����
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

//����ط�
void menu_savereplay()
{
	char filename[128];
	fstream fileexist;
	cls();
	while (1)
	{
		cout << "�طű���" << endl << "������Ҫ������ļ�����������׺�������򷵻أ�" << endl;
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
		cout << "�ļ��Ѵ��ڣ�";
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
	cout << "�ط��ļ�����ɹ���\n��Enter����" << endl;
	while (_getch() != '\r');
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	return;
}

//��ȡ�ط�
void menu_readreplay()
{
	char str[1028];
	char filename[128];
	fstream fileexist;
	cls();
	while (1)
	{
		cout << "�طŶ�ȡ" << endl << "������Ҫ��ȡ���ļ�����������׺������ֱ�ӻس��򷵻����˵���" << endl;
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
			cout << "�ļ������ڣ�";
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
				cout << "�ⲻ��ͬ����طż�¼";
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
	cout << "�ļ���ȡ�ɹ���\n��Enter��ʼ�ط�" << endl;
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
	printf("��Ϸ������%s����%s��ʤ����\n", winner == 1 ? "��" : "��", winner == 1 ? playername[blackplayer] : playername[whiteplayer]);
	printf("�ط���ɣ���Esc�������˵���");
	while (_getch() != 27);
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	menu0 = '0';
	return;

}

//���˵�
void menu_main()
{
	cout << "ͬ����" << endl;
	cout << "1.����Ϸ" << endl;
	cout << "2.����" << endl;
	cout << "3.�ط�" << endl;
	cout << "4.����" << endl;
	cout << "Esc.�˳�" << endl;
	while (menu0 != '1' && menu0 != '2' && menu0 != '3' && menu0 != '4' && menu0 != 27)
	{
		menu0 = _getch();
	}
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

//����Ϸ��
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
	printf("��Ϸ������%s����%s��ʤ����\n", winner == 1 ? "��" : "��", winner == 1 ? playername[blackplayer] : playername[whiteplayer]);
	printf("��R����طţ���Esc�������˵���");
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
			printf("��Ϸ������%s����%s��ʤ����\n", winner == 1 ? "��" : "��", winner == 1 ? playername[blackplayer] : playername[whiteplayer]);
			printf("��R����طţ���Esc�������˵���");
		}
	}
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	menu0 = '0';
	return;

}

//����Ϸ
void menu_new()
{
	char a = 0, b = 0, c = 0, a1 = 0, b1 = 0;
	cout << "����Ϸ" << endl;
	cout << "��������κεط���Esc�������˵�" << endl << endl;
	cout << "ѡ���ɫ������˭���ƣ�1.��ң�2.������3.�����" << endl;
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
	cout << "��ɫ���ӽ���" << playername[a - '0'] << "����" << endl << endl;
	if (a == '2')
	{
		cout << "ѡ����������̣�(1-4)" << endl;
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
		cout << "�����������Ϊ��" << a1 << endl << endl;
	}
	cout << "ѡ���ɫ������˭���ƣ�1.��ң�2.������3.�����" << endl;
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
	cout << "��ɫ���ӽ���" << playername[b - '0'] << "����" << endl << endl;
	if (b == '2')
	{
		cout << "ѡ����������̣�(1-4)" << endl;
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
		cout << "�����������Ϊ��" << b1 << endl << endl;
	}
	cout << "ѡ����ɣ���Enter��ʼ��Ϸ����Esc���˳�" << endl;
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

//����
void menu_load()
{
	char str[1028];
	char filename[128];
	fstream fileexist;
	cls();
	while (1)
	{
		cout << "�浵��ȡ" << endl << "������Ҫ��ȡ���ļ�����������׺������ֱ�ӻس��򷵻����˵���" << endl;
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
			cout << "�ļ������ڣ�";
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
				cout << "�ⲻ��ͬ����浵";
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
	cout << "�ļ���ȡ�ɹ���\n��Enter��ʼ��Ϸ" << endl;
	while (_getch() != '\r');
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	menu0 = '5';
	return;
}

//����
void menu_help()
{
	cls();
	cout << "������\n";
	cout << "\n1.����������\n";
	cout << "���ֵ���Ļغ�ʱ�������ʹ�ü��̽��в�����\n";
	cout << "ʹ���������Ҽ��ƶ���꣬Խ����ͼ�߽����ѭ����\n";
	cout << "ʹ�ÿո��ѡ�������ʼ���ӣ�ѡ������ӽ���Ϊ���Ρ�\n";
	cout << "��ʹ�ÿո��ѡ�����Ŀ��λ�á���ȷ���������ӽ��ƶ����ơ�\n";
	cout << "����볷��ѡ�����ӣ�ֻ��Ҫ�ٶ����Ӱ��¿ո񼴿ɡ�\n";
	cout << "��B�����Ի��壬����һ���Խ������λ���\n";
	cout << "��S������ѡ�񱣴���Ϸ���ȣ�����һ���ļ�������Ϸ�����浽�ļ���.thq\n";
	cout << "����Ĵ浵���������˵��ġ���ȡ����ʹ��\n";
	cout << "��Q������ѡ�񷵻ص����˵�\n";
	cout << "��Ϸ����ʱ����R������ѡ�񱣴���Ϸ�طţ�����һ���ļ������طŽ����浽�ļ���.rep\n";
	cout << "\n2.��Ϸ����\n";
	cout << "�����ͬ������Ϸ�����ͨ�е�ͬ����������һ�¡�\n";
	cout << "ͬ������һ�ֶ���������Ϸ���ڰ�˫��������7x7���������ƶ����ӡ�\n";
	cout << "���ÿ�ο����ƶ���λ�������޵ģ����岽��������£�\n";
	cout << "ѡȡ�����������ϼ�����һö���ӣ���ѡ��һ������λ�á�Ҫ�����Ӵ�Ϊ�գ���û�жԷ�Ҳû�м��������ӡ�\n";
	cout << "ѡ�������λ�ÿ��������¶���֮һ��\n";
	cout << "����λ������ѡȡ������Ϊ���ĵ�3x3�ķ�Χ�ڡ���ʱѡȡ�����ӻḴ����������λ�á�һ����8��λ�ÿ���ѡ��\n";
	cout << "����λ������ѡȡ������Ϊ���ĵ�3x3�ķ�Χ�⡢5x5�ķ�Χ�ڣ���ʱѡȡ�����ӻ��ƶ���������λ�ã�һ����16��λ�ÿ���ѡ��\n";
	cout << "�ܵ���˵���ƶ�һ���ǡ�����ճ�������ƶ������ǡ�����ճ������\n";
	cout << "ÿ�����Ӻ���λ�õ����ӻὫ���Ӵ����ڵ�8��λ�õ����ӣ�����У�������Լ������ӡ�\n";
	cout << "��û����������λ������Ϸ���������������ӵĵط��������ӿ���һ�������ӣ�Ȼ������˫���������������ʤ��\n";
	cout << "����ʱ�̶Է�������Ϊ0ʱ������ʤ����\n";
	cout << "������淢��ѭ��������һ����һ��ʹ�þ���ص����������ֹ���һ�����棬��ô�Է��������߳��͵�ʱһ����һ��������ᱻ��Ϊ���Ϸ����롣\n";
	cout << "��Ϸ�غ�����400ʱ����Ϸ��ֱ�ӽ���������˫���������������ʤ��\n";
	cout << endl << "��Esc����";
	while (_getch() != 27);
	PlaySound(TEXT("./sound/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
	menu0 = '0';
}

//������
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