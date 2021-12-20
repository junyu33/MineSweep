/*
changelog
1.0
(source code)
2.0
configuration interface (ok)
mine-generating xp->win7 mode (ok)
basic keyboard support (ok)
2.1
timing (ok)
minecnt (ok)
2.2
winning interface with personal things (ok)
fast-sweeping keyboard support (ok)
highlight numbers (ok)
small changes about lastx,lasty (ok)
3.0
leaderboard (ok)
three modes (ok)
3.1
quit in menu (ok)
add fast-flaggging (keyboard only) (ok)
lowlight blanks (keyboard only) (ok)
3.2
add colors to numbers (ok)
random block to start (ok) 

lastx, lasty 当前光标坐标
g_scrWidth 窗口宽度
g_scrHeight 窗口高度
g_minNum 雷的个数
myrec[20] 临时储存文件读取的纪录
MineArray.minecnt 剩余雷数
MineArray.timestp 时间戳
*/
#include "MineSweep.h"
#include <ege/sys_edit.h>
#define BLOCKSIZE 35
BlockType MARK;
MineArray g_mineArray;
BGManager g_bgm;
HWND g_hwnd;
int g_scrWidth = DEFAULT_MINE_WIDTH * IMG_SIZE, g_scrHeight = DEFAULT_MINE_HEIGHT * IMG_SIZE;
int g_minNum = 50;
int myrec[20];
void swap(int *x,int *y){int t=*x;*x=*y;*y=t;}
int min(int x,int y){return x<y?x:y;}
void quit()//ver 3.1
{
	cleardevice();
	setcolor(RED);
	setfont(20,10,"consolas");//ver 2.0
	outtextxy(g_scrWidth / 2 - 30, g_scrHeight / 2 - 20, "goodbye~");//ver 2.0
	outtextxy(g_scrWidth / 2 - 100, g_scrHeight / 2 + 20, "the program is exiting");//ver 2.0
	Sleep(2000);
	closegraph();
	exit(0);
}
void loadrecord()//ver 3.0
{
	freopen("recorde.in","r",stdin);
	setcolor(GREEN);xyprintf(100,70,"EASY");
	setcolor(0xFFA500);xyprintf(400,70,"NORMAL");//ver 3.2
	setcolor(RED);xyprintf(700,70,"HARD");	
	int nowy=100,a; setcolor(GREEN);
	for(int i=0;i<10;i++){
		scanf("%d",&a);
		xyprintf(100,nowy,"No.%d: %d seconds",i+1,a);
		nowy+=30;
	}
	freopen("recordm.in","r",stdin);
	nowy=100,a; setcolor(0xFFA500);//ver 3.2 orange
	for(int i=0;i<10;i++){
		scanf("%d",&a);
		xyprintf(400,nowy,"No.%d: %d seconds",i+1,a);
		nowy+=30;
	}
	freopen("recordh.in","r",stdin);
	nowy=100,a; setcolor(RED);
	for(int i=0;i<10;i++){
		scanf("%d",&a);
		xyprintf(700,nowy,"No.%d: %d seconds",i+1,a);
		nowy+=30;
	}
	setcolor(YELLOW);
	xyprintf(350,nowy,"press anykey to continue");
	fclose(stdin);
}
void saverecord(int x,int mode)//ver 3.0
{
	if(mode==dirEASY) freopen("recorde.in","r",stdin);
	if(mode==dirNORM) freopen("recordm.in","r",stdin);
	if(mode==dirHARD) freopen("recordh.in","r",stdin);
	int cnt=0,a;
	while(scanf("%d",&a)!=EOF&&cnt<=10)
	{
		myrec[cnt++]=a;
		printf("%d\n",a);
	}
	fclose(stdin);
	myrec[cnt]=x;
	for(int i=cnt;i;i--)
		if(myrec[i]<myrec[i-1]) swap(&myrec[i],&myrec[i-1]); 
	if(mode==dirEASY) freopen("recorde.in","w",stdout);
	if(mode==dirNORM) freopen("recordm.in","w",stdout);
	if(mode==dirHARD) freopen("recordh.in","w",stdout);
	for(int i=0;i<=min(cnt,10);i++)
		printf("%d\n",myrec[i]);
	fclose(stdout);
}
void startFrame()
{
	g_bgm.playStart();
	setcolor(RED);
	setfont(20,10, "consolas");
	for(int i = 0; i < g_scrWidth; i += 35)
	{
		for(int j = 0; j < g_scrHeight; j += 35) // ver 2.1
		{
			putimage(i, j, Block::imgBlock[0]);
		}
	}

	RECT rect;
	rect.left = rect.top = 0;
	rect.right = g_scrWidth;
	rect.bottom = g_scrHeight;
	outtextrect(0, 0, g_scrWidth, g_scrHeight, "ver3.2:\n1. added colors to numbers\n2. start randomly");//ver 3.2
	setcolor(YELLOW);
	outtextrect(0, 100, g_scrWidth, g_scrHeight, "click any block or press any key start a new game!\ntips:\nhold ESC key to exit\nhold e->easy n->normal h->hard space->custom\nhold r to restart.\nhold l to see leaderboard.\nyou can use direction key, enter, and \'1\' \'2\' \'3\' while gaming\n");//ver 3.2
}
void leaderboard()//ver 3.0
{
	//setbkcolor(WHITE);
	initgraph(960, 540);
	PIMAGE img;
	img=newimage();
	getimage(img,"scoreboard.png",0,0);
	putimage(0,0,960,540,img,0,0,1920,1080);
	loadrecord();
	for(;is_run;delay_fps(60))
		if(kbhit()) break;
	delimage(img);
	initgraph(g_scrWidth, g_scrHeight + 20, INIT_RENDERMANUAL);
	startFrame();
}
inline Message translateMsg(MOUSEMSG& msg)
{
	static bool isSearching = false;
	switch(msg.uMsg)
	{
	case WM_LBUTTONDOWN: 
		if(msg.mkRButton)
		{
			isSearching = true;
			return msgSEARCH;
		}
		break;
	case WM_RBUTTONDOWN:
		if(msg.mkLButton)
		{
			isSearching = true;
			return msgSEARCH;
		}
		else return msgGUESS;
	case WM_LBUTTONUP:
		isSearching = false;
		if(msg.mkRButton) return msgENDSEARCH;
 		if(isSearching)
 		{
 			isSearching = false;
 			return msgMOVE;
 		}
		return msgSWEEP;
	case WM_RBUTTONUP:
		if(msg.mkLButton) return msgENDSEARCH;
		isSearching = false;
	default:;
	}
	return msgMOVE;
}

inline void keyDownFrame(int x, int y)
{
	putimage(x / IMG_SIZE * IMG_SIZE, y / IMG_SIZE * IMG_SIZE, Block::imgSpace[0]);
}

DIRECTIONS play()
{
	MOUSEMSG msg;
	Message click;
	int lastx = 10, lasty = 10;//初始坐标
	bool win = false;
	bool isKeyDown = false, isSearching = false;

	g_mineArray.render();

	if(g_mineArray.win())
		return dirWIN;
	for(;is_run(); delay_fps(60))
	{
		bool needRedraw = false;
		int isdirection=0,isenter=0;//重置相关标记
		while(kbhit())// ver 2.0 如果有键盘操作
		{
			printf("now: %d %d\n",lastx,lasty);
			char ch = getch();
			switch(ch)
			{
				case 27: return dirEXIT; break;//esc退出
				case 32: return dirSET; break;//空格设置游戏参数
				case 'r': return dirREPLAY; break;//ver 3.0 按r重开
				case 'e': return dirEASY; break;// 按e简单
				case 'n': return dirNORM; break;// 按n普通
				case 'h': return dirHARD; break;// 按h困难
				case '1':// 按1插旗
					g_bgm.playRightClick();
					g_mineArray.guess(lastx, lasty);//快扫操作
					needRedraw = true;//需要重新画图
					break;	
			
				case '2'://ver 2.2
					needRedraw = true;
					if(!g_mineArray.search(lastx, lasty))//快扫失误
					{
						g_bgm.playBomb();
						return dirLOSE;//游戏失败
					}
					else 
					{
						g_bgm.playSearch();
					}
					if(g_mineArray.win())//如果雷扫完了
						return dirWIN;//就跳转到胜利界面
					break;
				case '3'://ver 3.1
					needRedraw = true;
					g_mineArray.flagall(lastx,lasty);//给周围都标上雷
					g_bgm.playSearch();
					break;
				case '\r'://enter扫雷
					if(lastx<0||lasty<0||lastx>g_scrWidth||lasty>g_scrHeight) continue;//防止光标跑到界外产生bug
						needRedraw = true;
					isenter=1;
					if(!g_mineArray.sweep(lastx, lasty)){//踩到雷
						g_bgm.playBomb();
						return dirLOSE;
					}
					else {
						g_bgm.playClick();
					}
					if(g_mineArray.win())
						return dirWIN;			
					break;
				case 37:
					needRedraw=true;isdirection=1;
					if(lastx>BLOCKSIZE) lastx-=BLOCKSIZE;//ver 2.2
					break;//left
				case 38:
					needRedraw=true;isdirection=1;
					if(lasty>BLOCKSIZE) lasty-=BLOCKSIZE;
					break;//up
				case 39:
					needRedraw=true;isdirection=1;
					if(lastx+BLOCKSIZE <= g_scrWidth) lastx+=BLOCKSIZE;
					break;//right
				case 40:
					needRedraw=true;isdirection=1;
					if(lasty+BLOCKSIZE <= g_scrHeight) lasty+=BLOCKSIZE;
					break;///down
			}
			if(msg.x < 0 || msg.x > g_scrWidth || msg.y < 0 || msg.y > g_scrHeight)
				continue;
		}

		while(mousemsg())//如果是鼠标操作
		{
			msg = GetMouseMsg();
			if(msg.uMsg == WM_MOUSEMOVE && msg.x/IMG_SIZE == lastx/IMG_SIZE && msg.y/IMG_SIZE == lasty/IMG_SIZE)//将鼠标坐标映射到方块坐标
				continue;
			needRedraw = true;
			lastx = msg.x;
			lasty = msg.y;
			click = translateMsg(msg);//将鼠标信息转化为指令
			switch(click)//下面都跟键盘的代码差不多
			{
			case msgSWEEP:
				if(!g_mineArray.sweep(msg.x, msg.y))
				{
					g_bgm.playBomb();
					return dirLOSE;
				}
				else 
				{
					g_bgm.playClick();
				}
				if(g_mineArray.win())
					return dirWIN;
				break;
			case msgGUESS:
				g_bgm.playRightClick();
				g_mineArray.guess(msg.x, msg.y);
				break;
			case msgSEARCH:
				isSearching = true;
				break;
			case msgENDSEARCH:
				isSearching = false;
				if(!g_mineArray.search(msg.x, msg.y))
				{
					g_bgm.playBomb();
					return dirLOSE;
				}
				else 
				{
					g_bgm.playSearch();
				}
				if(g_mineArray.win())
					return dirWIN;
				break;

			default:;
			}
		}
		if(needRedraw)//如果画面需要更新
		{
			//printf("111\n");
			if(lastx<0||lasty<0||lastx>g_scrWidth||lasty>g_scrHeight) continue;//如果光标超出窗口边界就不更新
			cleardevice();//清空显示器
			//Sleep(1000);
			g_mineArray.render();//重新贴图
			setcolor(YELLOW);
			setfont(20, 10, "consolas");
			
			xyprintf(g_scrWidth / 2 - 200, g_scrHeight,"kotomi: %d mines remaining, %d seconds passed", 
			g_mineArray.minecnt,time(0) - g_mineArray.timestp , 10);// ver 2.1 显示游戏时间与剩余雷数
			
			if(isSearching)//如果在快速扫雷
			{
				if(!isenter) g_mineArray.searchFrame(msg.x, msg.y);//如果这个动作是由鼠标完成的，就用鼠标位置作为输入
				else g_mineArray.searchFrame(lastx,lasty);//反之用键盘位置作为输入
			}
			else if(msg.mkLButton||isenter)//如果在扫雷
			{
				if(!isenter) keyDownFrame(msg.x, msg.y);
				else keyDownFrame(lastx,lasty);
			}
			else if(click == msgMOVE||isdirection)//如果在移动光标
			{
				//z123:;
				g_bgm.playMove();
				if(isdirection) g_mineArray.moveFrame(lastx, lasty, 1);// ver 3.1 如果是键盘就显示光标
				else g_mineArray.moveFrame(msg.x, msg.y, 0);//ver 3.1 如果是鼠标就不显示
			}
		}
	}

	return dirWIN;
}

void replay()
{
	void set(int);
	startFrame();
	MOUSEMSG msg;
	msg.x = g_scrWidth / 2 + 10, msg.y = g_scrHeight / 2 + 10;
	bool keystart = 0;//ver 3.2
	for(; is_run(); delay_fps(60))
	{
		if(mousemsg() && (msg = GetMouseMsg()).uMsg == WM_LBUTTONUP)
			break;
		if(kbhit())//ver 3.0
		{
			char t=getch();
			if(t==27) quit();//ver 3.1
			if(t==' '){set(0);return;}
			if(t=='l'){leaderboard();return replay();}
			if(t=='e') return set(dirEASY);
			if(t=='n') return set(dirNORM);
			if(t=='h') return set(dirHARD);
			else {keystart=1;break;}//ver 3.2
		}
	}
	int x = g_scrWidth / IMG_SIZE, y = g_scrHeight / IMG_SIZE;
	if(keystart) //ver 3.2
	{
		msg.x=rand()%g_scrWidth;
		msg.y=rand()%g_scrHeight;
	}
	g_mineArray.reInit(x, y, msg.x, msg.y, g_minNum);
	g_mineArray.sweep(msg.x, msg.y);
	//else g_mineArray.moveFrame(10,10); // default position
	flushmouse();
	while(kbhit()) getch();
}

char* read(char *buff,int* s){//ver 2.0
	*s=0;
	while(!isdigit(*buff)) buff++;
	while(isdigit(*buff)) {*s=*s*10+*buff-'0';buff++;}
	return buff;
}

void getnum(char* buff,int *a,int *b,int *c)//ver 2.0
{
	buff=read(buff,a);
	buff=read(buff,b);
	buff=read(buff,c);
}

void set(int mode)//ver 2.0
{
	setfont(20,10, "consolas");//3.0 solve font display bug
	int w, h, n;
	int LEN=480,WID=320;
	initgraph(LEN, WID);
	PIMAGE img;
	img=newimage();
	getimage(img,"392528.png",0,0);
	putimage(0,0,480,320,img,0,0,1920,1080);

	setcolor(RED);
	char strBuff[100];
	const int bufsize=100;
	int buflen=0;
	sys_edit editbox;
	editbox.create(true);
		
	xyprintf(0,0,"hello Nagisa Furukawa!",50);
	xyprintf(0,20,"please input length, width and number of mines",50);
	xyprintf(0,40,"eg: 20 15 50",50);
	editbox.move(20,200);
	editbox.size(440,100);
	editbox.setbgcolor(WHITE);
	editbox.visible(true);
	editbox.gettext(bufsize,strBuff);
	if(mode==dirEASY) w=10,h=10,n=10;//ver3.0
	else if(mode==dirNORM) w=20,h=15,n=50;//ver 3.0
	else if(mode==dirHARD) w=30,h=16,n=99; //ver 3.0
	else for(;is_run();delay_fps(60))
	{
		int output=0;
		while (kbmsg()) {
			key_msg msg = getkey();
			if ((msg.key == key_enter) && (msg.msg == key_msg_up)) {
				output = true;
			}
		}
		int a=0,b=0,c=0;
		if (output) {
			editbox.gettext(bufsize, strBuff);
			buflen = strlen(strBuff);
			editbox.settext("");
		}
		if(strBuff[0]){
			getnum(strBuff,&a,&b,&c);
			w=a,h=b,n=c;
			if(w < 10 || w > GetSystemMetrics(SM_CXSCREEN)/IMG_SIZE)
			{
				cleardevice();
				putimage(0,0,480,320,img,0,0,1920,1080);
				outtextxy(0,0,"invalid width, please input again!");
				continue;
			}
			else if(h < 10 || h > GetSystemMetrics(SM_CYSCREEN)/IMG_SIZE)
			{
				cleardevice();
				putimage(0,0,480,320,img,0,0,1920,1080);
				outtextxy(0,0,"invalid height, please input again!");
				continue;
			}
			else if(n < 10 || n > w * h - 11)//ver 3.1
			{
				cleardevice();
				putimage(0,0,480,320,img,0,0,1920,1080);
				outtextxy(0,0,"invalid number of mines, please input again!");
				continue;
			}
			break;
		}
	}
	flushkey();
	cleardevice();
	editbox.destroy();
	delimage(img);
	g_scrWidth = w * IMG_SIZE;
	g_scrHeight = h * IMG_SIZE;
	g_minNum = n;

	g_mineArray.minecnt = n;//ver 2.1
	initgraph(g_scrWidth, g_scrHeight + 20);// ver 2.1

	setbkmode(TRANSPARENT);
	g_hwnd = getHWnd();
	SetWindowText(g_hwnd, "MineSweeper by junyu33");
	flushmouse();
	replay();
}

void youWin()
{
	int mode;
	if(g_scrWidth/IMG_SIZE==10&&g_scrHeight/IMG_SIZE==10&&g_minNum==10) mode=dirEASY;
	if(g_scrWidth/IMG_SIZE==20&&g_scrHeight/IMG_SIZE==15&&g_minNum==50) mode=dirNORM;
	if(g_scrWidth/IMG_SIZE==30&&g_scrHeight/IMG_SIZE==16&&g_minNum==99) mode=dirHARD;
	saverecord(time(0)-g_mineArray.timestp,mode);//ver 3.0
	g_bgm.playWin();
	g_mineArray.winFrame();
	cleardevice();
	setcolor(YELLOW);
	setfont(18,9,"consolas");
	outtextxy(0,g_scrHeight/2, "the game will restart after 1.5 seconds");

	flushmouse();
	Sleep(1500);
	replay();
}

void youLose()
{
	g_mineArray.loseFrame();
	g_bgm.cleanBomb();
	cleardevice();
	setcolor(YELLOW);
	setfont(18,9,"consolas");
	outtextxy(0,g_scrHeight/2, "the game will restart after 1.5 seconds");

	flushmouse();
	Sleep(1500);
	replay();
}

void init()
{
	initgraph(g_scrWidth, g_scrHeight + 20, INIT_RENDERMANUAL);// ver 2.1
	setbkmode(TRANSPARENT);
	g_hwnd = getHWnd();
	SetWindowText(g_hwnd, "MineSweeper by junyu33");//ver 2.0
	g_mineArray.loadResource();
	startFrame();	
	MOUSEMSG msg;
	msg.x = g_scrWidth / 2 + 10, msg.y = g_scrHeight / 2 + 10;
	bool keystart = 0;
	for(; is_run(); delay_fps(60))
	{
		if(mousemsg() && (msg = GetMouseMsg()).uMsg == WM_LBUTTONUP)
			break;
		if(kbhit())//ver 3.0
		{
			char t=getch();
			if(t==27) quit();//ver 3.1
			if(t==' '){set(0);return;}
			if(t=='l'){leaderboard();return replay();}
			if(t=='e') return set(dirEASY);
			if(t=='n') return set(dirNORM);
			if(t=='h') return set(dirHARD);
			else {keystart=1;break;}//ver 3.2
		}	
	}
	//g_mineArray.minecnt=g_minNum;
	if(keystart) //ver 3.2
	{
		msg.x=rand()%g_scrWidth;
		msg.y=rand()%g_scrHeight;
	}
	g_mineArray.init(DEFAULT_MINE_WIDTH, DEFAULT_MINE_HEIGHT, msg.x, msg.y, g_minNum);
	g_mineArray.sweep(msg.x, msg.y);
	
	g_bgm.playClick();
}

int main()
{
	init();
	while(1)
	{
		switch(play())
		{
		case dirREPLAY: replay(); break;
		// case dirSET: set(0); break;
		// case dirEASY: set(1); break;
		// case dirNORM: set(2); break;
		// case dirHARD: set(3); break;
		case dirEXIT: quit(); break;//ver 3.1
		case dirWIN: youWin(); break;
		case dirLOSE: youLose(); break;
		default:;
		}
	}
	return 0;
}
