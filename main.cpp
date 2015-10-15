#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<list>
#include<string>
#include<ctime>
#include"jsoncpp/json.h"
using namespace std;

/*
 * 不愧传说大魔王 v3 
 */

int n,m;
const int maxn=25;
const int dx[4]={-1,0,1,0};
const int dy[4]={0,1,0,-1};
int invalid[maxn][maxn];

const int INF = 1e9;	//无穷大 

const int LEFT = 0;
const int DOWN = 1;
const int RIGHT = 2;
const int UP = 3;

struct point
{
	int x,y;
	int createRound;	//在第几轮生成 
	point(int _x,int _y,int _round=0){
		x=_x;
		y=_y;
		createRound = _round;
	}
};

struct Game{
	int round;	//当前游戏回合 
	int role;	//1表示在左上角，0表示在右下角出生 
}game;

list<point> snake[2]; // 0表示自己的蛇，1表示对方的蛇
int possibleDire[10];
int posCount;


//获取双方头尾 
point myHead(){
	point p=*(snake[0].begin());
	return p;
}
point opponentHead(){
	point p=*(snake[1].begin());
	return p;
}

point myTail(){
	point p=*(snake[0].rbegin());
	return p;
}
point opponentTail(){
	point p=*(snake[1].rbegin());
	return p;
}

//双方蛇头的曼哈顿距离 
int distance(){
	point me = myHead();
	point op = opponentHead();
	return abs(me.x-op.x)+abs(me.y-op.y);
} 


bool whetherGrow(int num)  //本回合是否生长
{	//注意这里回合从0开始。。。 
	if (num<=24) return true;
	if ((num-24)%3==0) return true;
	return false;
}

//createRound 回合生成的身体在第几回合消失 
int whenDisappear(int createRound){
	if(createRound==0)return 0;
	return 26+createRound+createRound/3;
}

void deleteEnd(int id)     //删除蛇尾
{
	snake[id].pop_back();
}
 
void move(int id,int dire,int num)  //编号为id的蛇朝向dire方向移动一步
{
	//num+2=当前回合. 
	point p=*(snake[id].begin());
	int x=p.x+dx[dire];
	int y=p.y+dy[dire];
	snake[id].push_front(point(x,y,num+2));
	if (!whetherGrow(num))
		deleteEnd(id);
}
void outputSnakeBody(int id)    //调试语句
{
	cout<<"Snake No."<<id<<endl;
	for (list<point>::iterator iter=snake[id].begin();iter!=snake[id].end();++iter)
		cout<<iter->x<<" "<<iter->y<<endl;
	cout<<endl;
}

/*
bool isInBody(int x,int y)   //判断(x,y)位置是否有蛇
{
	for (int id=0;id<=1;id++)
		for (list<point>::iterator iter=snake[id].begin();iter!=snake[id].end();++iter)
			if (x==iter->x && y==iter->y)
				return true;
	return false;
}*/

void body2Obstacle()   //把身体转化为障碍物 
{
	for (int id=0;id<=1;id++)
		for (list<point>::iterator iter=snake[id].begin();iter!=snake[id].end();++iter)
			invalid[iter->x][iter->y]=iter->createRound;
}
 
bool validDirection(int id,int k)  //判断当前移动方向的下一格是否合法
{
	point p=*(snake[id].begin());
	int x=p.x+dx[k];
	int y=p.y+dy[k];
	if (x>n || y>m || x<1 || y<1) return false;
	if (invalid[x][y]) return false;
	return true;
}
 
int Rand(int p)   //随机生成一个0到p的数字
{
	return rand()*rand()*rand()%p;
}


bool CBvis[maxn][maxn];

void dfs(int x,int y,int &res){
	res++;
	CBvis[x][y]=1;
	for(int i=0;i<4;i++){	//4 directions
		int nextX = x+dx[i];
		int nextY = y+dy[i];
		if (nextX>n || nextY>m || nextX<1 || nextY<1) continue;
		if(!invalid[nextX][nextY]&&!CBvis[nextX][nextY]){
			dfs(nextX,nextY,res);
		}
	}
}

//计算连通块大小 
int CB(int x,int y){
	memset(CBvis,0,sizeof(CBvis));
	int res = 0;
	dfs(x,y,res);
	return res;
}

/*
//计算对方移动后连通块大小 
int CBaftermove(int x,int y){
	memset(CBvis,0,sizeof(CBvis));
	point p=*(snake[1].begin());
	for(int i=0;i<4;i++){
		int nextX = p.x+dx[i];
		int nextY = p.y+dy[i];
		CBvis[nextX][nextY]=1;
	}
	
	int res = 0;
	dfs(x,y,res);
	return res;
}
*/

int dfsPath(int x,int y,int round,int maxDep=4){
//	cout<<"dfsing "<<x<<" "<<y<<endl;
	if(round+2>game.round+maxDep){
		return CB(x,y)+1;
	} 
	int tmp=invalid[x][y];
	invalid[x][y]=INF;
	int res = 0;
	for(int i=0;i<4;i++){	//4 directions
		int nextX = x+dx[i];
		int nextY = y+dy[i];
		if (nextX>n || nextY>m || nextX<1 || nextY<1) continue;
		//cout<<" .."<<round<<" "<<whenDisappear(invalid[nextX][nextY])<<"..";
		if(round>=whenDisappear(invalid[nextX][nextY])){
			res=max(res,dfsPath(nextX,nextY,round+1) );
		}
	}
	invalid[x][y]=tmp;
	return res;
}





int getDir(){
	int MAXdire=0,MAXval=-1;
	point me = myHead();
	point op = opponentHead();
	
	for(int i=0;i<posCount;i++){
		
		int x=me.x+dx[possibleDire[i]];
		int y=me.y+dy[possibleDire[i]];
		int score = 0;
		//加权。。搜索有限几步 
		score+=dfsPath(x,y,game.round,5)*32;
		score+=dfsPath(x,y,game.round,4)*16;
		score+=dfsPath(x,y,game.round,3)*8;
		score+=dfsPath(x,y,game.round,2)*4;
		//cout<<"    dir: "<<possibleDire[i]<<" score: "<<score<<endl;
		score+=dfsPath(x,y,game.round,1)*2;
		//cout<<"    dir: "<<possibleDire[i]<<" score: "<<score<<endl;

		
		/*
		 *当两蛇头距离远的时候，改为倾向于靠近对方
		 *当两蛇头过近的时候，改为倾向于远离 (暂缓实现)
		 */
		
		/*if(me.x<op.x && possibleDire[i]==RIGHT){
			score++;
		}
		if(me.x>op.x && possibleDire[i]==LEFT){
			score++;
		}
		if(me.y<op.y && possibleDire[i]==DOWN){
			score++;
		}
		if(me.y>op.y && possibleDire[i]==UP){
			score++;
		}*/
		//cout<<"    dir: "<<possibleDire[i]<<" score: "<<score<<endl;
		if( score>MAXval ){
			MAXval = score;
			MAXdire=possibleDire[i];
		}else if(score == MAXval){	//相等的时候随机 
			int rnd = Rand(1357);

			if(rnd&1){
				MAXdire=possibleDire[i];
			}
		}
	}
	return MAXdire;
}

int main()
{
	//freopen("input.txt","r",stdin);
	memset(invalid,0,sizeof(invalid));
	string str;
	string temp;
	while (getline(cin,temp))
		str+=temp;
 
	Json::Reader reader;
	Json::Value input;
	reader.parse(str,input);
 
	n=input["requests"][(Json::Value::UInt) 0]["height"].asInt();  //棋盘高度
	m=input["requests"][(Json::Value::UInt) 0]["width"].asInt();   //棋盘宽度
 
	int x=input["requests"][(Json::Value::UInt) 0]["x"].asInt();  //读蛇初始化的信息
	game.role = x;
	if (x==1){
		snake[0].push_front(point(1,1,1));
		snake[1].push_front(point(n,m,1));
	}
	else{
		snake[1].push_front(point(1,1,1));
		snake[0].push_front(point(n,m,1));
	}
	//处理地图中的障碍物
	int obsCount=input["requests"][(Json::Value::UInt) 0]["obstacle"].size(); 
 
	for (int i=0;i<obsCount;i++)
	{
		int ox=input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["x"].asInt();
		int oy=input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["y"].asInt();
		invalid[ox][oy]=INF;
	}
 
	//根据历史信息恢复现场
	int total=input["responses"].size();
 	game.round = total + 1;	//
	int dire;
	for (int i=0;i<total;i++)
	{
		dire=input["responses"][i]["direction"].asInt();
		move(0,dire,i);
 
		dire=input["requests"][i+1]["direction"].asInt();
		move(1,dire,i);	
	}
 
	if (!whetherGrow(total)) // 本回合两条蛇生长
	{
		deleteEnd(0);
		deleteEnd(1);
	}
 	
 	//身体转换为障碍物 
 	body2Obstacle();
 	
 	//cout<<"I'm now at "<<myHead().x<<","<<myHead().y<<endl;
 	
	for (int k=0;k<4;k++)
		if (validDirection(0,k))
			possibleDire[posCount++]=k;
 
	srand((unsigned)time(0)+total);
 
	//做出一个决策	
	Json::Value ret;

	ret["response"]["direction"]=getDir();
	
	Json::FastWriter writer;
	cout<<writer.write(ret)<<endl;
 
	return 0;
}

