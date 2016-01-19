#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<list>
#include<string>
#include<queue>
#include<ctime>
#include<time.h>
#include"jsoncpp/json.h"
using namespace std;

/*
 * ���ܴ�˵��ħ��
 *  ���¼�֦���� 
 *  ������������
 *  �߽���������
 *  �Զ������
 *  �������TLE���� 1.10
 *  ���������� 
 *  ������������ 0.2
 */


//const bool DEBUG = 0;

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

const int ME = 0;
const int OP = 1;


//�ֵ���
struct TrieNode{
	int bestSon;
	TrieNode *pSon[4];			//4�����ܵĺ�̵�ָ��
	TrieNode(){
		for(int i=0;i<4;i++){
			pSon[i]=NULL;
		}
		bestSon = -1;
	}
};


struct Trie{
	TrieNode *root;		//���� 
	Trie(){
		root = new TrieNode();
	}
	
	//�����µ��ֵ����ڵ� 
	TrieNode* insert(TrieNode *pNode, int dir){
		pNode->pSon[dir] = new TrieNode();
		return pNode->pSon[dir];
	}
}trie;


double runTime(){
	return (double)clock()/CLOCKS_PER_SEC;
}



struct point{
	int x,y;
	int createRound;	//�ڵڼ�������
	point(){
	}
	point(int _x,int _y,int _round=0){
		x=_x;
		y=_y;
		createRound = _round;
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
	int maxDep;	//������ 
	int hitTime;
	double timeLimit;
	bool TLE;	// time limit exceeded. 
	double aspirationErr;
}game;

list<point> snake[2]; // 0��ʾ�Լ����ߣ�1��ʾ�Է�����

void outputPath(){
	cout<<"��������˫�������߷�:"<<endl;
	TrieNode *cur = trie.root;
	int dep = game.maxDep;
	while(dep--) {
		
		int dir = cur->bestSon;
		if(-1 == dir)break;
		if(dep&1){
			cout<<"�ҷ��� "<<dir;
		}else{
			cout<<", �Է��� "<<dir<<endl;
		}
		cur = cur->pSon[dir];
	}
}

/*
 * ��ȡ˫��ͷβ
 * 0-���� 1-�Է�
 */
inline point getHead(bool who){
	point p=*(snake[who].begin());
	return p;
}


bool whetherGrow(int num){  //���غ��Ƿ�����
	//ע������غϴ�0��ʼ������
	if (num<=24) return true;
	if ((num-24)%3==0) return true;
	return false;
}

//createRound �غ����ɵ������ڵڼ��غ���ʧ
int whenDisappear(int createRound){
	if(createRound==0)return 0;
	return 26+createRound+(createRound-1)/2;
}

void deleteEnd(int id){     //ɾ����β
	snake[id].pop_back();
}

void move(int id,int dire,int num){  //���Ϊid���߳���dire�����ƶ�һ��
	//num+2=��ǰ�غ�.
	point p=*(snake[id].begin());
	int x=p.x+dx[dire];
	int y=p.y+dy[dire];
	snake[id].push_front(point(x,y,num+2));
	if (!whetherGrow(num))
		deleteEnd(id);
}


int Ucount,Dcount,Lcount,Rcount;

inline double Abs(double x){
	if(x<0)return -x;
	return x;
}

inline int Abs(int x){
	if(x<0)return -x;
	return x;
}

inline bool isNearUD(){
	point me = getHead(ME);
	double mid;
	if(m&1){
		mid = (m+0.0)/2;
	}else{
		mid = (m+1.0)/2;
	}
	if(Abs(me.y-mid)<=2)return 1;
	return 0;
}

inline bool isNearLR(){
	point me = getHead(ME);
	double mid;
	if(n&1){
		mid = (n+0.0)/2;
	}else{
		mid = (n+1.0)/2;
	}
	if(Abs(me.x-mid)<=2)return 1;
	return 0;
}

void body2Obstacle(){   //������ת��Ϊ�ϰ���
	Ucount = Dcount = Lcount = Rcount = 0;
	for (int id=0;id<=1;id++){
		for (list<point>::iterator iter=snake[id].begin();iter!=snake[id].end();++iter){
			invalid[iter->x][iter->y]=iter->createRound;
			int tmpx = iter->x;
			int tmpy = iter->y;
			//
			//if(id)continue;
			int add=1;
			if(id)add=-1;
			if(n&1){
				if(tmpx<((n+1)>>1)){
					Lcount+=add;
				}
				if(tmpx>((n+1)>>1)){
					Rcount+=add;
				}
			}else{
				if(tmpx<=((n)>>1)){
					Lcount+=add;
				}else{
					Rcount+=add;
				}
			}
			//
			if(m&1){
				if(tmpy<((m+1)>>1)){
					Ucount+=add;
				}
				if(tmpy>((m+1)>>1)){
					Dcount+=add;
				}
			}else{
				if(tmpy<=((m)>>1)){
					Ucount++;
				}else{
					Dcount++;
				}
			}
		}
	}
}


int CBvis[2][maxn][maxn];

//�Ƿ�տ��� 
inline bool isOpening(){
	int x = ((n-1)>>1) + ((m-1)>>1);
	if(game.round<=x+2)return 1;
	return 0;
}

//ʹ�ÿ������������м��� 
void startFix(int dir,double &score){
	if(isOpening()){
		point me = getHead(0);
		if(game.role==1){	//���Ͻ�
			if(dir==RIGHT){
				score += 1*(abs((n-1)/2-me.x)+1);
			}else if(dir==DOWN){
				score += 1*(abs((m-1)/2-me.y)+1);
			}
		}else{
			if(dir==LEFT){
				score += 1*(abs(me.x-(n-1)/2)+1);
			}else if(dir==UP){
				score += 1*(abs(me.y-(m-1)/2)+1);
			}
		}
	}else{		//͵���̡��� 
		int UD = Abs(Ucount-Dcount);
		int LR = Abs(Lcount-Rcount);
		
		if(isNearUD()){
			if(Ucount>Dcount && dir==DOWN ){
				//cout<<"D �ӳ�"<<endl; 
				score+=0.001*UD;
			}
			if(Dcount>Ucount && dir==UP ){
				//cout<<"U �ӳ�"<<endl; 
				score+=0.001*UD;
			}
		}
		if(isNearLR()){
			if(Lcount>Rcount && dir==RIGHT ){
				//cout<<"R �ӳ�"<<endl; 
				score+=0.001*LR;
			}
			if(Rcount>Lcount && dir==LEFT ){
				//cout<<"L �ӳ�"<<endl; 
				score+=0.001*LR;
			}
		}
	}
	
}

bool canMove(int x,int y,int round){
    //���ܳ���
    if(x>n || y>m || x<1 || y<1)return false;
    if(invalid[x][y]==INF)return false;
    if(invalid[x][y]==0){
    	return true;
	}else if( round>=whenDisappear(invalid[x][y]) ){
        if(!isOpening())return true;
    }
    return false;
}

point que[400];


void bfsCB(int who,int x,int y,int round){
	int quehead = 0;
	int quetail = 0;
	que[quetail++]=point(x,y);
	CBvis[who][x][y]=0;
	while(quetail!=quehead){
		point cur = que[quehead++];
		int x = cur.x;
		int y = cur.y;
		for(int i=0;i<4;i++){
			int nextX = x+dx[i];
			int nextY = y+dy[i];
			if(CBvis[who][nextX][nextY] !=INF)continue;
			if(canMove(nextX,nextY,round+CBvis[who][x][y] + 1)){
				que[quetail++] = (point(nextX,nextY));
				CBvis[who][nextX][nextY] = CBvis[who][x][y] + 1;
			}
		}
	}
}

double judge(int x0,int y0,int x1,int y1,int round){
	for(int k=0;k<2;k++){
		for(int i=1;i<=n;i++){
			for(int j=1;j<=m;j++){
				CBvis[k][i][j]=INF;
			}
		}
	}
	bfsCB(0,x0,y0,round);
	bfsCB(1,x1,y1,round);
	
	int me=1;
	int op=1;
	for(int i=1;i<=n;i++){
		for(int j=1;j<=m;j++){
			if(CBvis[ME][i][j]==INF&&CBvis[OP][i][j]==INF)continue;
			if(CBvis[ME][i][j]<CBvis[OP][i][j]){
				me++;
			}
			if(CBvis[ME][i][j]>CBvis[OP][i][j]){
				op++;
			}
		}
	}

	return (me+0.0)/(op+0.0)-(op+0.0)/(me+0.0);
}


/*
 * dep Ϊż��ʱ���Ǽ���ڵ㣬��ʱ˫���ƶ��Ļغ������ 
 * ע�������˲���pnode
 * �ò���������ǰ�ڵ����ֵ����е�λ�á� 
 */
double AB(TrieNode *pNode,int &bestDir, point pt0, point pt1, double alapha, double beta, int dep){
	// time control 
	if(game.TLE ){
		return -1;
	}
	if( (game.hitTime&1023) == 0){
		if(runTime()>game.timeLimit){
			game.TLE = 1;
		}
	}
	game.hitTime++;

	int curRound = game.round + ((game.maxDep-dep)>>1);
	if(!(dep&1)){	//�غ������ 
		
		//Ϊ˫����һ���ŷ�����
		int myCount = 0, opCount=0;
		for(int i=0;i<4;i++){
			int nextX = pt0.x+dx[i];
			int nextY = pt0.y+dy[i];
			if(canMove(nextX,nextY,curRound+1)){
                myCount++;
                bestDir=i;
            }
		}
		for(int i=0;i<4;i++){
			int nextX = pt1.x+dx[i];
			int nextY = pt1.y+dy[i];
			if(canMove(nextX,nextY,curRound+1))opCount++;
		}
		
		if(myCount+opCount == 0){	//����ͬ��(����û����������ǰ��ֹ)
			//if(DEBUG)cout<<"tie."<<endl;
			return 0;
		}else if(myCount == 0){	//����·����(����û����������ǰ��ֹ)
			return -INF+curRound;
		}else if(opCount == 0){	//�Է���·����(����û����������ǰ��ֹ)
			return INF-curRound;
		}
		
		
		//�ѵ��������� 
		if(!dep){
			double res =  judge(pt0.x,pt0.y,pt1.x,pt1.y,curRound);
			//if(DEBUG)cout<<"normal end at"<<x0<<","<<y0 <<" score is "<<res<<endl;
			return res;
		} 
	}
	
	double score = -INF;
	//�ҳ�������,����������֮
	
	//������ע�;���û��PVS��ȡ��ע�;���PVS 
	int principal = pNode->bestSon;
	if(~principal){
		int i = principal;
		int nextX = pt0.x+dx[i];
		int nextY = pt0.y+dy[i];
		if(canMove(nextX,nextY,curRound+1)){
			//������һ���ڵ��Ƿ���֮ǰ��������ѹ��������֮ǰû���ѹ���Ҫ�Ƚ�����Ӧ�ڵ㡣������ȼӴ�ʱ����PVS 
			TrieNode *pSon = pNode->pSon[i];
			//	����ڵ㲻���ڣ������ڵ㣬����ʱ�����ʱΪ0 
			
			if( !pSon )pSon = trie.insert(pNode,i);	
			
			// make
			int tmp = invalid[nextX][nextY];
			invalid[nextX][nextY] = curRound+1; 
			// make end
			
			int unused;
			
			score = -AB( pSon ,unused, pt1, point(nextX,nextY), -beta, -alapha, dep-1);
			if(dep == game.maxDep){
				startFix(i,score);
			}
			// undo make
			invalid[nextX][nextY] = tmp;
			// undo end
			
			bestDir=i;
			if(score > alapha){
				alapha = score;
			}
			
			//ά����Ѻ�� 
			pNode->bestSon = i;
			
			//���������� 
			/*if(dep == game.maxDep){
				if(DEBUG){
					cout<<" --- dir = "<<i<<" score is ";
					printf("%.4f\n",score);
				}
			}*/
		}
	}
	
	
	for(int i=0;i<4;i++){
		if(score >= beta)break;			//����pruning. 
		if(i == principal)continue;		//�������Ѿ���������,��Ҫ�ظ����� 
		if(score > alapha)alapha = score;
		int nextX = pt0.x+dx[i];
		int nextY = pt0.y+dy[i];
		if(canMove(nextX,nextY,curRound+1)){
			//������һ���ڵ��Ƿ���֮ǰ��������ѹ��������֮ǰû���ѹ���Ҫ�Ƚ�����Ӧ�ڵ㡣������ȼӴ�ʱ����PVS 
			TrieNode *pSon = pNode->pSon[i];
			//	����ڵ㲻���ڣ������ڵ㣬����ʱ�����ʱΪ0 
			
			if( !pSon )pSon = trie.insert(pNode,i);	
			
			// make
			int tmp = invalid[nextX][nextY];
			invalid[nextX][nextY] = curRound+1; 
			// make end
			
			int unused;
			
			
			double value;
			if(~principal){		//��������ʹ���㴰
				value = -AB( pSon ,unused, pt1, point(nextX,nextY), -alapha - 0.01, -alapha, dep-1);
				if(dep == game.maxDep){
					startFix(i,value);
				}
				if(value > score){
					if(alapha < value && value < beta && dep > 2){
						value = -AB( pSon ,unused, pt1, point(nextX,nextY), -beta, -value, dep-1);	//����Ϊ�������д�ú�α�����г��� 
						if(dep == game.maxDep){
							startFix(i,value);
						}
					}
					score = value;
					bestDir=i;
					pNode->bestSon = i;
				} 
			}else{				//������������,���������� 
				value = -AB( pSon ,unused, pt1, point(nextX,nextY), -beta, -alapha, dep-1);
				if(dep == game.maxDep){
					startFix(i,value);
				}
			}
			
			// undo make
			invalid[nextX][nextY] = tmp;
			// undo end

			if( value>score ){
				score = value;
				bestDir=i;
				pNode->bestSon = i;
			}
			
			
			
			
			//���������� 
			/*if(dep == game.maxDep){
				if(DEBUG){
					cout<<" --- dir = "<<i<<" score is ";
					printf("%.4f\n",value);
				}
			}*/
		}
		
	}
	
	//����ǵ�һ����ã�������ԼӸ����������� 
	if(dep == game.maxDep){
		//Fix the score and return a better direction.
	}
	return score;
} 

double oldVal;
int getDir(){
	int MAXdir=0;
	double MAXval=-INF;
	point me = getHead(0);
	point op = getHead(1);
	
	double expect = 0.0; 
//	expect = AB(trie.root,MAXdir, me, op, -INF, INF, dep);    //������ 
///*	�������� 
	if(game.maxDep == 14){
		expect = AB(trie.root, MAXdir, me, op, -INF, INF, game.maxDep);
	}else{
		expect = AB(trie.root, MAXdir, me, op, oldVal-game.aspirationErr, oldVal+game.aspirationErr, game.maxDep);
		if(expect < oldVal-game.aspirationErr){
			expect = AB(trie.root, MAXdir, me, op, -INF, expect, game.maxDep);
		}else if(expect > oldVal+game.aspirationErr){
			expect = AB(trie.root, MAXdir, me, op, expect, INF, game.maxDep);
		}		
	}
	oldVal = expect;
//*/ 

	/*if(DEBUG){
		if(!game.TLE){
			cout<<"����"<<game.maxDep/2 <<" �����. dir = "<<MAXdir; 
			printf(" score = %.4f\n",expect);	
			outputPath();		
		}
		else{
			cout<<"����"<<game.maxDep/2 <<" ���ж�. "<<endl;
		}
	}*/
	return MAXdir;
}


//��Ϊ��������,�˺�����������. 
void calcMaxDep(){
	game.maxDep = 14;
} 

int solve(){
	int ans=0;
	while(1){
		int tmp = getDir();
		if(game.TLE){
			break;
		}else{
			ans = tmp;
		}
		
		double curTime = runTime();
		if(curTime<0.1){
			game.maxDep += 2;
		}
		game.maxDep += 2;
	}
	return ans;
}

//��ʼ��,����main������ͷ����. 
void init(){
	game.hitTime = 0;
	game.timeLimit = 1.10; 
	game.aspirationErr = 0.2;
	calcMaxDep();
}


int dist2OP[maxn][maxn];
int calcOverall(){
	for(int i=0;i<=n+1;i++){
		for(int j=0;j<=m+1;j++){
			dist2OP[i][j] = INF;
		}
	}
	point op = getHead(OP);
	int quehead = 0;
	int quetail = 0;
	que[quetail++]=op;
	dist2OP[op.x][op.y]=0;
	while(quetail!=quehead){
		point cur = que[quehead++];
		int x = cur.x;
		int y = cur.y;
		for(int i=0;i<4;i++){
			int nextX = x+dx[i];
			int nextY = y+dy[i];
			if(dist2OP[nextX][nextY] !=INF)continue;
			if( !invalid[nextX][nextY] ){
				que[quetail++] = (point(nextX,nextY));
				dist2OP[nextX][nextY] = dist2OP[x][y] + 1;
			}
		}
	}
}


int main(){
	//if(DEBUG)freopen("input.txt","r",stdin);
	//freopen("output.txt","w",stdout);

	init();
	
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

	for (int i=0;i<obsCount;i++){
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

	if (!whetherGrow(total)){ // ���غ�����������
	
		deleteEnd(0);
		deleteEnd(1);
	}

 	//����ת��Ϊ�ϰ���
 	body2Obstacle();
	
	/*if(DEBUG){
		cout<<"n = "<<n<<" m = "<<m<<endl;
		cout<<"Current round is "<<game.round<<endl;
	 	cout<<"I'm now at "<<getHead(0).x<<","<<getHead(0).y<<endl;
	    cout<<"He's now at "<<getHead(1).x<<","<<getHead(1).y<<endl;
	}*/


	//����һ������
	Json::Value ret;
	

	
	int ans = solve();
	
	ret["response"]["direction"] = ans;

	Json::FastWriter writer;
	
	//cout<<"U D L R is "<<Ucount<<" "<<Dcount<<" "<<Lcount<<" "<<Rcount<<endl;
	cout<<writer.write(ret)<<endl;

	return 0;
}