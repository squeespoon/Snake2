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

const int SHORT = 0;
const int LONG = 1;

struct point{
	int x,y;
	int createRound;	//�ڵڼ�������
	point(){
		father=NULL;
	}
	point(int _x,int _y,int _round=0){
		x=_x;
		y=_y;
		createRound = _round;
		father=NULL;
	}
	
	bool equals(const point &other){
		if(x==other.x&&y==other.y)return 1;
		return 0;
	}
};

struct Game{
	int round;	//��ǰ��Ϸ�غ�
	int role;	//1��ʾ�����Ͻǣ�0��ʾ�����½ǳ���
	int n;
	int m;
	int maxDep;
}game;

list<point> snake[2]; // 0��ʾ�Լ����ߣ�1��ʾ�Է�����
int possibleDire[2][10];
int posCount[2];
int score[2][10][10];





//

/*
 * ��ȡ˫��ͷβ
 * 0-���� 1-�Է� 
 */
point getHead(bool who){
	point p=*(snake[who].begin());
	return p;
}

point getTail(bool who){
	point p=*(snake[who].rbegin());
	return p;
}


//˫����ͷ�������پ���
int distance(){
	point me = getHead(0);
	point op = getHead(1);
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
	return 26+createRound+(createRound-1)/2;
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

	//cout<<"?inv is"<<invalid[x][y]<<" "<<whenDisappear(invalid[x][y]) <<" -- round is"<<round<<" --"<<endl;
    if(invalid[x][y]==0){
    	return true;
	}else if( round+1>=whenDisappear(invalid[x][y]) ){
        
        return true;
    }
    if(invalid[x][y]>game.round && invalid[x][y]>round){
        //cout<<" inv is"<<invalid[x][y]<<"-- round is"<<round<<" --";
        return true;
    }
    return false;
}


/*
 * ����·��
 * x:������, y:������ round:��ǰ�غ� maxDep:������ flag:�Ƿ���Ƕdfs who:�ķ� 
 */
int dfsPath(int x,int y,int x2,int y2,int round,int maxDep,bool flag,bool who){
//	cout<<"dfsing path "<<x<<","<<y<<" round "<<round<<" who="<<who<<" flag="<<flag<<endl;
	int tmp=invalid[x][y];
	invalid[x][y]=round;
	if(round>=game.round+maxDep){
		invalid[x][y]=tmp;
		if(flag){               //�ѶԷ�
            return dfsPath(x2,y2,x,y,game.round+1,game.maxDep,false,!who);
        }else{
            //cout<<"dfs my path end. now score is "<<(1<<maxDep)*(CB(x,y)+maxDep)<<endl;
            return min(64,(1<<maxDep))*(CB(x,y)+maxDep);   //��������
        }
	}

	int res;
	if(flag){
        res=INF;
	}else{
	    res=0;
	}
	int dircnt=0;
	for(int i=0;i<4;i++){	//4 directions
		int nextX = x+dx[i];
		int nextY = y+dy[i];
		if(canMove(nextX,nextY,round)){
			dircnt++;
		}
	}
	for(int i=0;i<4;i++){	//4 directions
		int nextX = x+dx[i];
		int nextY = y+dy[i];
		if(canMove(nextX,nextY,round)){
			if(flag){
                res=min(res,dfsPath(nextX,nextY,x2,y2,round+1,(dircnt==1?maxDep+1:maxDep),flag,who) );
			}else{
			    res=max(res,dfsPath(nextX,nextY,x2,y2,round+1,(dircnt==1?maxDep+1:maxDep),flag,who) );
			}
		}
	}
	invalid[x][y]=tmp;
	//��·���� 
	if(flag){
		if(res==INF)return dfsPath(x2,y2,x,y,game.round+1,game.maxDep,false,!who);
	}else{
		if(res==0)	return min(64,(1<<(round-game.round)))*(round-game.round) ;
	}
	//��Ҫ̫�󡣡� 
	if(res>1500)return 1500;
    return res;
}


bool isOpening(){
	int x = (n-1)/2 + (m-1)/2;
	if(game.round<=x+1)return 1;
	return 0;
}

//
void startFix(int dir,int &score){
	if(!isOpening())return;
	point me = getHead(0);
	if(game.role==1){	//���Ͻ� 
		if(dir==RIGHT){
			score += abs((n-1)/2-me.x)+1;
		}else if(dir==DOWN){
			score += abs((m-1)/2-me.y)+1;
		}
	}else{
		if(dir==LEFT){
			score += abs(me.x-(n-1)/2)+1;
		}else if(dir==UP){
			score += abs(me.y-(m-1)/2)+1;
		}
	}
	
}


int getDir(){
	int MAXdire=0,MAXval=-INF;
	point me = getHead(0);
	point op = getHead(1);
	
	//������·���ߣ�ֱ�ӷ��� 
	if(posCount[0]==0||posCount[1]==0){
		return possibleDire[0][0];
	}
	
	int minMyScore[10];
	int minOpScore[10];
	
	for(int i=0;i<4;i++){
		minMyScore[i]=INF;
		minOpScore[i]=0;
	}
	
	for(int i=0;i<posCount[0];i++){
		for(int j=0;j<posCount[1];j++){
			int myNextX =  me.x+dx[possibleDire[0][i]];
			int myNextY =  me.y+dy[possibleDire[0][i]];
			int opNextX =  op.x+dx[possibleDire[1][j]];
			int opNextY =  op.y+dy[possibleDire[1][j]];
			//ע������Ҫ����� 
			score[0][i][j] = dfsPath(opNextX,opNextY,myNextX,myNextY,game.round+1,game.maxDep,1,1);
			score[1][i][j] = dfsPath(myNextX,myNextY,opNextX,opNextY,game.round+1,game.maxDep,1,0);
			//
			minMyScore[i]=min(minMyScore[i],score[0][i][j]);
			minOpScore[i]=max(minOpScore[i],score[1][i][j]);
			/*cout<<"�ҵķ���"<<possibleDire[0][i]<<" ���ַ���"<<possibleDire[1][j]<<endl;
			cout<<" ---�ҵĵ÷�"<<score[0][i][j]<<" ���ֵ÷�"<<score[1][i][j]<<endl;
		*/
		}
	}
	
	for(int i=0;i<posCount[0];i++){
		int score = minMyScore[i];
		
		startFix(possibleDire[0][i],score);
		
		//�ܿ������� 
		if(minMyScore[i]>2*minOpScore[i])score+=INF;
		
		//cout<<"    dir: "<<possibleDire[0][i]<<" score: "<<score<<endl;
		if( score>MAXval ){
			MAXval = score;
			MAXdire=possibleDire[0][i];
		}else if(score == MAXval){	//��ȵ�ʱ�����
			int rnd = Rand(1357);
			
			if(rnd&1){
				MAXdire=possibleDire[0][i];
			}
		}
	}
	return MAXdire;
}

int main(){
//	freopen("input.txt","r",stdin);

	memset(invalid,0,sizeof(invalid));
	game.maxDep = 6;
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
	
/*	cout<<"n = "<<n<<" m = "<<m<<endl;
	cout<<"Current round is "<<game.round<<endl;
 	cout<<"I'm now at "<<getHead(0).x<<","<<getHead(0).y<<endl;
    cout<<"He's now at "<<getHead(1).x<<","<<getHead(1).y<<endl;
*/

	for(int who=0;who<2;who++){
		for (int k=0;k<4;k++){
			if (validDirection(who,k)){
				possibleDire[who][posCount[who]++]=k;
			}
		}
	}
	srand((unsigned)time(0)+total);

	//����һ������
	Json::Value ret;
	
	
	ret["response"]["direction"]=getDir();

	Json::FastWriter writer;
	cout<<writer.write(ret)<<endl;

	return 0;
}
