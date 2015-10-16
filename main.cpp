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
 * ���ܴ�˵��ħ��
 */

int n,m;
const int maxn=25;
const int dx[4]={-1,0,1,0};
const int dy[4]={0,1,0,-1};
int invalid[maxn][maxn];

const int INF = 1e9;	//�����

const int LEFT = 0;
const int DOWN = 1;
const int RIGHT = 2;
const int UP = 3;

struct point
{
	int x,y;
	int createRound;	//�ڵڼ�������
	point(int _x,int _y,int _round=0){
		x=_x;
		y=_y;
		createRound = _round;
	}
};

struct Game{
	int round;	//��ǰ��Ϸ�غ�
	int role;	//1��ʾ�����Ͻǣ�0��ʾ�����½ǳ���
	int nextX;
	int nextY;
}game;

list<point> snake[2]; // 0��ʾ�Լ����ߣ�1��ʾ�Է�����
int possibleDire[10];
int posCount;


//��ȡ˫��ͷβ
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

//˫����ͷ�������پ���
int distance(){
	point me = myHead();
	point op = opponentHead();
	return abs(me.x-op.x)+abs(me.y-op.y);
}


bool whetherGrow(int num)  //���غ��Ƿ�����
{	//ע������غϴ�0��ʼ������
	if (num<=24) return true;
	if ((num-24)%3==0) return true;
	return false;
}

//createRound �غ����ɵ������ڵڼ��غ���ʧ
int whenDisappear(int createRound){
	if(createRound==0)return 0;
	return 26+createRound+createRound/3;
}

void deleteEnd(int id)     //ɾ����β
{
	snake[id].pop_back();
}

void move(int id,int dire,int num)  //���Ϊid���߳���dire�����ƶ�һ��
{
	//num+2=��ǰ�غ�.
	point p=*(snake[id].begin());
	int x=p.x+dx[dire];
	int y=p.y+dy[dire];
	snake[id].push_front(point(x,y,num+2));
	if (!whetherGrow(num))
		deleteEnd(id);
}
void outputSnakeBody(int id)    //�������
{
	cout<<"Snake No."<<id<<endl;
	for (list<point>::iterator iter=snake[id].begin();iter!=snake[id].end();++iter)
		cout<<iter->x<<" "<<iter->y<<endl;
	cout<<endl;
}

/*
bool isInBody(int x,int y)   //�ж�(x,y)λ���Ƿ�����
{
	for (int id=0;id<=1;id++)
		for (list<point>::iterator iter=snake[id].begin();iter!=snake[id].end();++iter)
			if (x==iter->x && y==iter->y)
				return true;
	return false;
}*/

void body2Obstacle()   //������ת��Ϊ�ϰ���
{
	for (int id=0;id<=1;id++)
		for (list<point>::iterator iter=snake[id].begin();iter!=snake[id].end();++iter)
			invalid[iter->x][iter->y]=iter->createRound;
}

bool validDirection(int id,int k)  //�жϵ�ǰ�ƶ��������һ���Ƿ�Ϸ�
{
	point p=*(snake[id].begin());
	int x=p.x+dx[k];
	int y=p.y+dy[k];
	if (x>n || y>m || x<1 || y<1) return false;
	if (invalid[x][y]) return false;
	return true;
}

int Rand(int p)   //�������һ��0��p������
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

//������ͨ���С
int CB(int x,int y){
	memset(CBvis,0,sizeof(CBvis));
	int res = 0;
	dfs(x,y,res);
	return res;
}

bool canMove(int x,int y,int round){
    //���ܳ���
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
 * ����������·��
 * x:������, y:������ round:��ǰ�غ� maxDep:������ flag:�Ƿ���Ƕdfs
 */
int dfsMyPath(int x,int y,int round,int maxDep,bool flag){
	//cout<<"dfsing my path "<<x<<","<<y<<" round"<<round<<endl;
	int tmp=invalid[x][y];
	invalid[x][y]=round;
	if(round>=game.round+maxDep-1){
		if(flag){               //�ѶԷ�
            point p = opponentHead();
            invalid[x][y]=tmp;
            return dfsOpPath(p.x,p.y,game.round,maxDep,false);
        }else{
            //cout<<"dfs my path end. now score is "<<CB(x,y)+1<<endl;
            invalid[x][y]=tmp;
            return CB(x,y)+1;   //��������
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
 * �����Է���·��
 * x:������, y:������ round:��ǰ�غ� maxDep:������ flag:�Ƿ���Ƕdfs
 */
int dfsOpPath(int x,int y,int round,int maxDep,bool flag){
	//cout<<"dfsing op path "<<x<<","<<y<<" round"<<round<<endl;
    int tmp=invalid[x][y];
	invalid[x][y]=round;
    if(round>=game.round+maxDep-1){
        if(flag){               //�Ѽ���
            //ע���������
            invalid[x][y]=tmp;
            return dfsMyPath(game.nextX,game.nextY,game.round+1,maxDep,false);
        }else{
            invalid[x][y]=tmp;
            return CB(x,y)+1;   //��������
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

	if(flag&&res==INF){ //��·����
	    //cout<<"no way"<<endl;
        return 2*dfsMyPath(game.nextX,game.nextY,game.round+1,maxDep,false);
	}
	invalid[x][y]=tmp;
	return res;
}

int evaluate(int dir){

	//ö�ٶԷ�·�������ڶԷ�ÿ��·����ö���Լ�·��

	//�Ը����ĶԷ�·�����Լ��������·�������Լ���·����������֣�ȡ�÷���ߵļ�Ϊ���Է�����·�����֡�

	//�ѶԷ�����·���У��÷���͵ģ���Ϊ���Լ������������֡�����A��ʾ
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
	//���ǣ����ǵõ�����������ԭʼ����A�������˷��ӡ���



	//������������ö���Լ���ö�ٶԷ�

	//���ڸ������Լ���·����ö�ٶԷ�·�����Է��ж���·�����֣�ȡ��õģ���Ϊ���Լ�����·�������֡�

	//�Լ�����·���У��Է��÷���ͷ֣���B��ʾ



	//����B�����B�͵�һ���̶ȣ�������A��ֵ���������ӡ�


}

int getDir(){
	int MAXdire=0,MAXval=-1;
	point me = myHead();
	point op = opponentHead();

	for(int i=0;i<posCount;i++){

		int score = evaluate(possibleDire[i]);



		/*
		 *������ͷ����Զ��ʱ�򣬸�Ϊ�����ڿ����Է�
		 *������ͷ������ʱ�򣬸�Ϊ������Զ�� (�ݻ�ʵ��)
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
		}else if(score == MAXval){	//��ȵ�ʱ�����
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

	n=input["requests"][(Json::Value::UInt) 0]["height"].asInt();  //���̸߶�
	m=input["requests"][(Json::Value::UInt) 0]["width"].asInt();   //���̿��

	int x=input["requests"][(Json::Value::UInt) 0]["x"].asInt();  //���߳�ʼ������Ϣ
	game.role = x;
	if (x==1){
		snake[0].push_front(point(1,1,1));
		snake[1].push_front(point(n,m,1));
	}
	else{
		snake[1].push_front(point(1,1,1));
		snake[0].push_front(point(n,m,1));
	}
	//�����ͼ�е��ϰ���
	int obsCount=input["requests"][(Json::Value::UInt) 0]["obstacle"].size();

	for (int i=0;i<obsCount;i++)
	{
		int ox=input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["x"].asInt();
		int oy=input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["y"].asInt();
		invalid[ox][oy]=INF;
	}

	//������ʷ��Ϣ�ָ��ֳ�
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

	if (!whetherGrow(total)) // ���غ�����������
	{
		deleteEnd(0);
		deleteEnd(1);
	}

 	//����ת��Ϊ�ϰ���
 	body2Obstacle();

 	/*cout<<"I'm now at "<<myHead().x<<","<<myHead().y<<endl;
    cout<<"He's now at "<<opponentHead().x<<","<<opponentHead().y<<endl;
*/
	for (int k=0;k<4;k++)
		if (validDirection(0,k))
			possibleDire[posCount++]=k;

	srand((unsigned)time(0)+total);

	//����һ������
	Json::Value ret;

	ret["response"]["direction"]=getDir();

	Json::FastWriter writer;
	cout<<writer.write(ret)<<endl;

	return 0;
}

