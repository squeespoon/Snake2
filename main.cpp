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
 * 智能传说大魔王
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
	int nextX;
	int nextY;
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

bool canMove(int x,int y,int round){
    //不能出界
    //cout<<"in canmove "<<x<<" "<<y<<" "<<round<<endl;
    if(x>n || y>m || x<1 || y<1)return false;
    if(invalid[x][y]==INF)return false;
    if( round>whenDisappear(invalid[x][y]) ){
        //cout<<"?inv is"<<invalid[x][y]<<" "<<whenDisappear(invalid[x][y]) <<" -- round is"<<round<<" --";
        return true;
    }
    if(invalid[x][y]>game.round && invalid[x][y]>round){
        //cout<<" inv is"<<invalid[x][y]<<"-- round is"<<round<<" --";
        return true;
    }
    return false;
}

/*
int dfsPath(int x,int y,int round,int maxDep=4){
//	cout<<"dfsing "<<x<<" "<<y<<endl;
	if(round>=game.round+maxDep-1){
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
*/


int dfsOpPath(int x,int y,int round,int maxDep,bool flag);
int dfsMyPath(int x,int y,int round,int maxDep,bool flag);


/*
 * 搜索己方的路径
 * x:横坐标, y:纵坐标 round:当前回合 maxDep:最大深度 flag:是否内嵌dfs
 */
int dfsMyPath(int x,int y,int round,int maxDep,bool flag){
	//cout<<"dfsing my path "<<x<<","<<y<<" round"<<round<<endl;
	int tmp=invalid[x][y];
	invalid[x][y]=round;
	if(round>=game.round+maxDep-1){
		if(flag){               //搜对方
            point p = opponentHead();
            invalid[x][y]=tmp;
            return dfsOpPath(p.x,p.y,game.round,maxDep,false);
        }else{
            //cout<<"dfs my path end. now score is "<<CB(x,y)+1<<endl;
            invalid[x][y]=tmp;
            return CB(x,y)+1;   //返回评分
        }
	}

	int res;
	if(flag){
        res=INF;
	}else{
	    res=0;
	}
	for(int i=0;i<4;i++){	//4 directions
		int nextX = x+dx[i];
		int nextY = y+dy[i];
		if(canMove(nextX,nextY,round)){
			if(flag){
                res=min(res,dfsMyPath(nextX,nextY,round+1,maxDep,flag) );
			}else{
			    res=max(res,dfsMyPath(nextX,nextY,round+1,maxDep,flag) );
			}
		}

	}
	invalid[x][y]=tmp;
    return res;
}

/*
 * 搜索对方的路径
 * x:横坐标, y:纵坐标 round:当前回合 maxDep:最大深度 flag:是否内嵌dfs
 */
int dfsOpPath(int x,int y,int round,int maxDep,bool flag){
	//cout<<"dfsing op path "<<x<<","<<y<<" round"<<round<<endl;
    int tmp=invalid[x][y];
	invalid[x][y]=round;
    if(round>=game.round+maxDep-1){
        if(flag){               //搜己方
            //注意这里起点
            invalid[x][y]=tmp;
            return dfsMyPath(game.nextX,game.nextY,game.round+1,maxDep,false);
        }else{
            invalid[x][y]=tmp;
            return CB(x,y)+1;   //返回评分
        }
	}

	int res;
	if(flag){
        res=INF;
	}else{
	    res=0;
	}
	for(int i=0;i<4;i++){	//4 directions
		int nextX = x+dx[i];
		int nextY = y+dy[i];
		if(canMove(nextX,nextY,round)){
			if(flag){
                res=min(res,dfsOpPath(nextX,nextY,round+1,maxDep,flag) );
			}else{
			    res=max(res,dfsOpPath(nextX,nextY,round+1,maxDep,flag) );
			}
		}
	}

	if(flag&&res==INF){ //无路可走
	    //cout<<"no way"<<endl;
        return 2*dfsMyPath(game.nextX,game.nextY,game.round+1,maxDep,false);
	}
	invalid[x][y]=tmp;
	return res;
}

int evaluate(int dir){

	//枚举对方路径，对于对方每条路径，枚举自己路径

	//对给定的对方路径，自己能有许多路径，在自己的路径走完后，评分，取得分最高的记为《对方这条路的评分》

	//把对方所有路径中，得分最低的，记为《自己这个方向的评分》，用A表示
	point my = myHead();
	point op = opponentHead();
	game.nextX = my.x+dx[dir];
	game.nextY = my.y+dy[dir];

    int A = 0;
    A += dfsOpPath(op.x,op.y,game.round,6,true)*32;
    /*A += dfsOpPath(op.x,op.y,game.round,4,true)*16;
    A += dfsOpPath(op.x,op.y,game.round,3,true)*8;
    A += dfsOpPath(op.x,op.y,game.round,2,true)*4;
    A += dfsOpPath(op.x,op.y,game.round,1,true)*2;*/
    return A;
	//于是，我们得到的这个方向的原始评分A，考虑了防坑。。



	//反过来。。先枚举自己再枚举对方

	//对于给定的自己的路径，枚举对方路径，对方有多条路，评分，取最好的，记为《自己该条路径的评分》

	//自己所有路径中，对方得分最低分，用B表示



	//考察B，如果B低到一定程度，就增加A的值，否则无视。


}

int getDir(){
	int MAXdire=0,MAXval=-1;
	point me = myHead();
	point op = opponentHead();

	for(int i=0;i<posCount;i++){

		int score = evaluate(possibleDire[i]);



		/*
		 *当两蛇头距离远的时候，改为倾向于靠近对方
		 *当两蛇头过近的时候，改为倾向于远离 (暂缓实现)
		 */

		if(me.x<op.x && possibleDire[i]==RIGHT){
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
		}
	//	cout<<"    dir: "<<possibleDire[i]<<" score: "<<score<<endl;
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

int main(){
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

 	/*cout<<"I'm now at "<<myHead().x<<","<<myHead().y<<endl;
    cout<<"He's now at "<<opponentHead().x<<","<<opponentHead().y<<endl;
*/
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

