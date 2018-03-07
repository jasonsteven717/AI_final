#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <queue>
#include <stack>
#include <vector>
#pragma warning(disable:4996)
#define NODESIZE 21
#define MAX_ALIVE_CHESS_NUM 6
using namespace std;

struct node
{
	node()
	{
		this->nodeID = 0;
		this->which_circle = 0;
		this->connect = NULL;
	};
	node(int val)
	{
		this->nodeID = val;
		this->which_circle = 0;
		this->connect = NULL;
	};
	int nodeID;
	int role;
	int which_circle;//在哪一個circle裡面(判斷重複走步)
	node* connect;
};
struct player
{
	player()//for the copy_value_of_play_function
	{
	}
	/*
	上方玩家編號(id) = 1;
	下方玩家編號(id) = 2;
	*/
	player(string s)
	{
		if (!strcmp(s.c_str(), "top"))//上方玩家棋子設置(設置6個棋子在棋盤上哪一個位置
		{
			this->chess_position[0] = 4;//第0個棋子在棋盤編號4的點
			this->chess_position[1] = 0;
			this->chess_position[2] = 1;
			this->chess_position[3] = 2;
			this->chess_position[4] = 6;
			this->chess_position[5] = 3;

			for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
			{
				this->chess_space[i] = 5;//每一個棋子的氣一開始都是5
			}
			this->id = 1;//player編號
		}
		else//下方玩家棋子設置
		{
			this->chess_position[0] = 14;
			this->chess_position[1] = 18;
			this->chess_position[2] = 19;
			this->chess_position[3] = 20;
			this->chess_position[4] = 16;
			this->chess_position[5] = 17;

			for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
			{
				this->chess_space[i] = 5;//每一個棋子的氣一開始都是5
			}
			this->id = 2;//player編號
		}
		this->current_alive_chess_num = MAX_ALIVE_CHESS_NUM;//玩家一開始存活的棋子數量都是6
		this->last_move_circle_num = -1;
		this->move_in_same_circle_times = 0;
	}
	int id;
	int last_move_circle_num;//上一次移動的circle是哪一個
	int move_in_same_circle_times;//在同一個circle移動幾次
	int current_alive_chess_num;//該玩家目前棋面上的棋子數量
	int chess_position[MAX_ALIVE_CHESS_NUM];//該玩家每個棋子的對應位置
	int chess_space[MAX_ALIVE_CHESS_NUM];//該玩家每個棋子的氣(附近有幾個空格)
};
struct tree
{
	tree()
	{
		this->value = -1e9;
		this->instruction_where = -1;
		this->instruction_to = -1;
		this->top_current_state = new player("top");//(上方)，編號 = 1，建樹用
		this->down_current_state = new player("down");//(下方)，編號 = 2，建樹用
	}
	tree(int value, int instruction_where, int instruction_to)
	{
		this->value = value;
		this->instruction_where = instruction_where;
		this->instruction_to = instruction_to;
	}
	int value;
	int instruction_where;
	int instruction_to;
	vector<tree*> next;
	player* top_current_state;
	player* down_current_state;
};
struct state //possible_state
{
	state()
	{
		this->instruction_where = 0;
		this->instruction_to = 0;
	}
	state(int instruction_where, int instruction_to)
	{
		this->instruction_where = instruction_where;
		this->instruction_to = instruction_to;
	}
	int instruction_where;
	int instruction_to;
};

//--------------------------------------------------------------
//golbal variable
//--------------------------------------------------------------
node** Map = new node*[NODESIZE];//地圖
player* ai = new player("top");//電腦(上方)，棋子編號 = 1
player* p1 = new player("down");//玩家(下方)，棋子編號 = 2
player top;//電腦(上方)，棋子編號 = 1，建樹用
player down;//玩家(下方)，棋子編號 = 2，建樹用
state decision;//決策的下法
vector<state> possible_state;//紀錄可能的棋譜
bool vis[NODESIZE];
int d_min;//default value is 1e9
int Round;//第幾回合
int currentp1 = 15;//center初始點
int currentp2 = 17;//center初始點
int currentpI = 5;//ai後攻center初始點
int currentpII = 3;//center初始點
int direction = 0;//AI位置 0下1上
int tree_level = 1;//樹的層數
int how_many_time_in_center = 1;//進入center次數
								//--------------------------------------------------------------
								//function declaration
								//--------------------------------------------------------------
void init();//初始化
void path_set();//路徑設置
int calc_distance(node* start, node* end);//計算兩點間的距離
void distance_dfs(node* from, node* end, int d);//由calc_distance呼叫的dfs
void calc_space();//計算所有點的space
int space_dfs(player* which_turn, node* n, int temp_space);//由calc_space呼叫的dfs
void judge_rule(player* which_turn);//判斷規則
void move(player* which_turn, int which_node, int node_destination);//哪一方，那顆棋子(棋盤編號)，移到哪裡(棋盤編號)
void build_game_tree(int which_first, player* player_top_state, player* player_down_state, tree* parent_tree_node, int tree_level);//建樹
void build_judge(player* top_state, player* down_state);//判斷新的盤面(建樹用);//判斷新的盤面(建樹用)
player* copy_value_of_player(player* input);//copy pointer value 
int evaluation_function(int which_turn, player* top_state, player* down_state);
int min_Max_search(tree* root, int flag_of_minMax);
void random_stupid_ai(player* which_turn);//隨機笨AI
void little_smart_ai();//有點聰明的AI
void strong_ai();//有點強大的AI
void stepone();
void steptwo();
void stepthree();
void stepfour();
void stepfive();
void center();
void gameover();//gameover

int Round_for_plarer_first = 1;//for stupid_ai_2
void stepone02(player* which_turn);
void steptwo02(player* which_turn);
void stepthree02(player* which_turn);
void stepfour02(player* which_turn);
void stepfive02(player* which_turn);
//--------------------------------------------------------------
//test function declaration
//--------------------------------------------------------------
void printMap();

//--------------------------------------------------------------
//function declaration implement
//--------------------------------------------------------------
void printMap()
{
	//Map[i] for i = {1~25}
	//0:no chess on that pos
	//1:play1(top:O) has chess on that pos
	//2:play2(down:X) has chess on that pos
	printf("        %c一%c一%c		|	    0一1一2					\n", Map[0]->role == 1 ? 'O' : (Map[0]->role == 2 ? 'X' : '#'), Map[1]->role == 1 ? 'O' : (Map[1]->role == 2 ? 'X' : '#'), Map[2]->role == 1 ? 'O' : (Map[2]->role == 2 ? 'X' : '#'));
	printf("      /  \\ | /  \\	|	  /  \\ | /  \\					\n");
	printf("     /     %c     \\ 	|	 /     3     \\						\n", Map[3]->role == 1 ? 'O' : (Map[3]->role == 2 ? 'X' : '#'));
	printf("    /      |      \\	|	/      |      \\						\n");
	printf("    %c      %c      %c	|      4       5       6						\n", Map[4]->role == 1 ? 'O' : (Map[4]->role == 2 ? 'X' : '#'), Map[5]->role == 1 ? 'O' : (Map[5]->role == 2 ? 'X' : '#'), Map[6]->role == 1 ? 'O' : (Map[6]->role == 2 ? 'X' : '#'));
	printf("   / \\   / | \\   / \\	|     / \\    / | \\    / \\							\n");
	printf("  %c 一%c-%c一%c一%c-%c一 %c	|    7 - 8- 9- 10-11-12-13						\n", Map[7]->role == 1 ? 'O' : (Map[7]->role == 2 ? 'X' : '#'), Map[8]->role == 1 ? 'O' : (Map[8]->role == 2 ? 'X' : '#'), Map[9]->role == 1 ? 'O' : (Map[9]->role == 2 ? 'X' : '#'), Map[10]->role == 1 ? 'O' : (Map[10]->role == 2 ? 'X' : '#'), Map[11]->role == 1 ? 'O' : (Map[11]->role == 2 ? 'X' : '#'), Map[12]->role == 1 ? 'O' : (Map[12]->role == 2 ? 'X' : '#'), Map[13]->role == 1 ? 'O' : (Map[13]->role == 2 ? 'X' : '#'));
	printf("   \\ /   \\ | /   \\ /	|     \\ /    \\ | /    \\ /						\n");
	printf("    %c      %c      %c	|      14     15      16						\n", Map[14]->role == 1 ? 'O' : (Map[14]->role == 2 ? 'X' : '#'), Map[15]->role == 1 ? 'O' : (Map[15]->role == 2 ? 'X' : '#'), Map[16]->role == 1 ? 'O' : (Map[16]->role == 2 ? 'X' : '#'));
	printf("    \\      |      /	|	\\      |      /						\n");
	printf("     \\     %c     /	|	 \\     17    /						\n", Map[17]->role == 1 ? 'O' : (Map[17]->role == 2 ? 'X' : '#'));
	printf("      \\  / | \\  /	|	  \\  / | \\  /				\n");
	printf("        %c一%c一%c		|	   18一19一20					\n", Map[18]->role == 1 ? 'O' : (Map[18]->role == 2 ? 'X' : '#'), Map[19]->role == 1 ? 'O' : (Map[19]->role == 2 ? 'X' : '#'), Map[20]->role == 1 ? 'O' : (Map[20]->role == 2 ? 'X' : '#'));
	getchar();

}

int calc_distance(node* start, node* end)//計算兩點間的距離
{
	d_min = 1e9;
	memset(vis, 0, sizeof(vis));
	if (start->connect == NULL)
	{
		return 1e9;
	}
	distance_dfs(start, end, 0); //dfs
								 /*
								 //test unit node
								 node* s = Map[0];
								 node* e = Map[2];
								 printf("distance from %d to %d: %d \n",s->nodeID,e->nodeID,calc_distance(s,e));
								 getchar();
								 //test all nodes
								 for(int i = 0 ; i < NODESIZE ; ++i)
								 {
								 for(int j = 0 ; j < NODESIZE ; ++j)
								 {
								 node* s = Map[i];
								 node* e = Map[j];
								 printf("distance from %d to %d: %d \n",s->nodeID,e->nodeID,calc_distance(s,e));
								 }
								 getchar();
								 }
								 */
	return d_min;
}
void distance_dfs(node* from, node* end, int d)//由calc_distance呼叫的dfs
{
	node* find = Map[from->nodeID];
	if (vis[find->nodeID])return;
	if (d > d_min)
	{
		return;//prune the search tree
	}
	if (find->nodeID == end->nodeID)
	{
		d_min = d < d_min ? d : d_min;
		return;
	}
	vis[from->nodeID] = true;//for backtracking
	while (find->connect != NULL)
	{
		find = find->connect;

		if (!vis[find->nodeID])distance_dfs(find, end, d + 1);
	}
	vis[from->nodeID] = false;//for backtracking

}
void calc_space()//計算所有點的space
{
	//計算p1所有棋子的space
	for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
	{
		if (p1->chess_position[i] == -1) continue;//被吃掉的就不用判斷
		memset(vis, 0, sizeof(vis));
		p1->chess_space[i] = space_dfs(p1, Map[p1->chess_position[i]], 0);
		//printf("p1 的第%d個棋子(編號%d) space = %d\n",i,p1->chess_position[i],p1->chess_space[i]);
	}
	//計算ai所有棋子的space
	for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
	{
		if (ai->chess_position[i] == -1) continue;//被吃掉的就不用判斷
		memset(vis, 0, sizeof(vis));
		ai->chess_space[i] = space_dfs(ai, Map[ai->chess_position[i]], 0);
		//printf("ai 的第%d個棋子(編號%d) space = %d\n",i,ai->chess_position[i],ai->chess_space[i]);
	}
}
int space_dfs(player* which_turn, node* n, int temp_space)//由calc_space呼叫的dfs
{
	node* ptr = Map[n->nodeID];
	node* current_pos = Map[n->nodeID];//for backtracking
	int space = temp_space;
	vis[current_pos->nodeID] = true;
	while (ptr->connect != NULL)
	{
		ptr = ptr->connect;
		if (Map[ptr->nodeID]->role == 0 && !vis[ptr->nodeID]) //遇到空格，space+1
		{
			vis[ptr->nodeID] = true;//避免該位置重複計算
			space++;
		}
		else if (Map[ptr->nodeID]->role == which_turn->id)
		{
			if (!vis[ptr->nodeID])space = space_dfs(which_turn, ptr, space);
		}
	}
	vis[current_pos->nodeID] = false;//for backtracking
	return space;
}
void init()
{
	srand(time(NULL));
	d_min = 1e9;
	Round = 1;
	tree_level = 0;
	path_set();
}
void path_set()
{
	/*
	上方棋子編號(role)   = 1
	下方棋子編號(role)   = 2
	沒有棋子的位置(role) = 0
	*/
	for (int i = 0; i < NODESIZE; ++i)
	{
		Map[i] = new node();
		Map[i]->nodeID = i;
	}
	//--------------------------------------------------------------
	//路徑設置:node 0 - 10
	//--------------------------------------------------------------
	//node 0
	node *ptr = Map[0];
	ptr->which_circle = 0;
	ptr->role = 1;
	ptr->connect = new node(1);
	ptr = ptr->connect;
	ptr->connect = new node(3);
	ptr = ptr->connect;
	ptr->connect = new node(4);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 1
	ptr = Map[1];
	ptr->role = 1;
	ptr->which_circle = 0;
	ptr->connect = new node(0);
	ptr = ptr->connect;
	ptr->connect = new node(2);
	ptr = ptr->connect;
	ptr->connect = new node(3);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 2
	ptr = Map[2];
	ptr->role = 1;
	ptr->which_circle = 0;
	ptr->connect = new node(1);
	ptr = ptr->connect;
	ptr->connect = new node(3);
	ptr = ptr->connect;
	ptr->connect = new node(6);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 3
	ptr = Map[3];
	ptr->role = 1;
	ptr->which_circle = 0;
	ptr->connect = new node(0);
	ptr = ptr->connect;
	ptr->connect = new node(1);
	ptr = ptr->connect;
	ptr->connect = new node(2);
	ptr = ptr->connect;
	ptr->connect = new node(5);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 4
	ptr = Map[4];
	ptr->role = 1;
	ptr->which_circle = 1;
	ptr->connect = new node(0);
	ptr = ptr->connect;
	ptr->connect = new node(7);
	ptr = ptr->connect;
	ptr->connect = new node(8);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 5
	ptr = Map[5];
	ptr->role = 0;
	ptr->which_circle = 2;
	ptr->connect = new node(3);
	ptr = ptr->connect;
	ptr->connect = new node(9);
	ptr = ptr->connect;
	ptr->connect = new node(10);
	ptr = ptr->connect;
	ptr->connect = new node(11);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 6
	ptr = Map[6];
	ptr->role = 1;
	ptr->which_circle = 3;
	ptr->connect = new node(2);
	ptr = ptr->connect;
	ptr->connect = new node(12);
	ptr = ptr->connect;
	ptr->connect = new node(13);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 7
	ptr = Map[7];
	ptr->role = 0;
	ptr->which_circle = 1;
	ptr->connect = new node(4);
	ptr = ptr->connect;
	ptr->connect = new node(8);
	ptr = ptr->connect;
	ptr->connect = new node(14);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 8
	ptr = Map[8];
	ptr->role = 0;
	ptr->which_circle = 1;
	ptr->connect = new node(4);
	ptr = ptr->connect;
	ptr->connect = new node(7);
	ptr = ptr->connect;
	ptr->connect = new node(9);
	ptr = ptr->connect;
	ptr->connect = new node(14);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 9
	ptr = Map[9];
	ptr->role = 0;
	ptr->which_circle = 2;
	ptr->connect = new node(5);
	ptr = ptr->connect;
	ptr->connect = new node(8);
	ptr = ptr->connect;
	ptr->connect = new node(10);
	ptr = ptr->connect;
	ptr->connect = new node(15);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 10
	ptr = Map[10];
	ptr->role = 0;
	ptr->which_circle = 2;
	ptr->connect = new node(5);
	ptr = ptr->connect;
	ptr->connect = new node(9);
	ptr = ptr->connect;
	ptr->connect = new node(11);
	ptr = ptr->connect;
	ptr->connect = new node(15);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//--------------------------------------------------------------
	//路徑設置:node 11 - 20
	//--------------------------------------------------------------
	//node 11
	ptr = Map[11];
	ptr->role = 0;
	ptr->which_circle = 2;
	ptr->connect = new node(5);
	ptr = ptr->connect;
	ptr->connect = new node(10);
	ptr = ptr->connect;
	ptr->connect = new node(12);
	ptr = ptr->connect;
	ptr->connect = new node(15);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 12
	ptr = Map[12];
	ptr->role = 0;
	ptr->which_circle = 3;
	ptr->connect = new node(6);
	ptr = ptr->connect;
	ptr->connect = new node(11);
	ptr = ptr->connect;
	ptr->connect = new node(13);
	ptr = ptr->connect;
	ptr->connect = new node(16);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 13
	ptr = Map[13];
	ptr->role = 0;
	ptr->which_circle = 3;
	ptr->connect = new node(6);
	ptr = ptr->connect;
	ptr->connect = new node(12);
	ptr = ptr->connect;
	ptr->connect = new node(16);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 14
	ptr = Map[14];
	ptr->role = 2;
	ptr->which_circle = 1;
	ptr->connect = new node(7);
	ptr = ptr->connect;
	ptr->connect = new node(8);
	ptr = ptr->connect;
	ptr->connect = new node(18);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 15
	ptr = Map[15];
	ptr->role = 0;
	ptr->which_circle = 2;
	ptr->connect = new node(9);
	ptr = ptr->connect;
	ptr->connect = new node(10);
	ptr = ptr->connect;
	ptr->connect = new node(11);
	ptr = ptr->connect;
	ptr->connect = new node(17);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 16
	ptr = Map[16];
	ptr->role = 2;
	ptr->which_circle = 3;
	ptr->connect = new node(12);
	ptr = ptr->connect;
	ptr->connect = new node(13);
	ptr = ptr->connect;
	ptr->connect = new node(20);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 17
	ptr = Map[17];
	ptr->role = 2;
	ptr->which_circle = 4;
	ptr->connect = new node(15);
	ptr = ptr->connect;
	ptr->connect = new node(18);
	ptr = ptr->connect;
	ptr->connect = new node(19);
	ptr = ptr->connect;
	ptr->connect = new node(20);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 18
	ptr = Map[18];
	ptr->role = 2;
	ptr->which_circle = 4;
	ptr->connect = new node(14);
	ptr = ptr->connect;
	ptr->connect = new node(17);
	ptr = ptr->connect;
	ptr->connect = new node(19);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 19
	ptr = Map[19];
	ptr->role = 2;
	ptr->which_circle = 4;
	ptr->connect = new node(17);
	ptr = ptr->connect;
	ptr->connect = new node(18);
	ptr = ptr->connect;
	ptr->connect = new node(20);
	ptr = ptr->connect;
	ptr->connect = NULL;
	//node 20
	ptr = Map[20];
	ptr->role = 2;
	ptr->which_circle = 4;
	ptr->connect = new node(16);
	ptr = ptr->connect;
	ptr->connect = new node(17);
	ptr = ptr->connect;
	ptr->connect = new node(19);
	ptr = ptr->connect;
	ptr->connect = NULL;
	/*
	//output map
	for(int i = 0 ; i < NODESIZE ; ++i)
	{
	bool fir = true;
	node *tmp = Map[i];
	printf("%d :",tmp->nodeID);
	while(tmp->connect != NULL)
	{
	if(!fir) printf("->");
	tmp = tmp->connect;
	printf("%d",tmp->nodeID);
	fir = false;
	}
	printf("\n");
	}
	*/

}
void judge_rule(player* which_turn)//判斷哪一方
{
	//重新計算所有位置的氣
	calc_space();
	//判斷某個棋子有無被吃掉
	for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
	{
		if (which_turn->chess_position[i] == -1)continue;//被吃掉的就不用再判斷
		if (which_turn->chess_space[i] == 0)//如果某個棋子的氣=0
		{
			Map[which_turn->chess_position[i]]->role = 0;//將棋盤該位置變成空的
														 //-------------------------------------------------------------------------------//////////////////////////////////////////////////
			if (which_turn->id == 1)
			{
				cout << "p1";
			}
			else
				cout << "ai";

			cout << "被吃掉一顆" << endl;
			///////////////////////////////////////////////////////////////////////////////////////////////////////////

			which_turn->chess_position[i] = -1;//把那個棋子吃掉(設成-1)
			which_turn->current_alive_chess_num--;//該方棋子-1
		}
		if (which_turn->current_alive_chess_num <= 2)
		{
			printf("GAME OVER!!\n");
			if (which_turn->id == 1)
			{
				printf("上方玩家勝利!!!\n");
				printf("(原因:下方玩家棋子數在兩顆以下)\n");
			}
			else
			{
				printf("上方玩家勝利!!!\n");
				printf("(原因:下方玩家棋子數在兩顆以下)\n");
			}
			gameover();//gameover
		}
	}
	//判斷在某小圈重複移動寫在move()
}
void gameover()//gameover
{
	system("PAUSE");
	exit(0);//關閉程式
}
void move(player* which_turn, int which_node, int node_destination)//哪一方，那顆棋子(棋盤編號)，移到哪裡(棋盤編號)
{
	if (Map[which_node]->role != which_turn->id)//判斷要移動的地方是否是這回的的玩家
	{
		//printf("非法的移動:從 %d 到 %d(該點不是可以移動的棋子)\n",which_node,node_destination);
		printf("Loading...\n");
		system("PAUSE");
		if (which_turn->id == 2)random_stupid_ai(p1);
		else random_stupid_ai(ai);
		return;
	}
	if (Map[node_destination]->role != 0)//判斷要移動的目的是否是空的
	{
		//printf("非法的移動:從 %d 到 %d(該點上有其他棋子)\n",which_node,node_destination);
		printf("Loading...\n");
		system("PAUSE");
		if (which_turn->id == 2)random_stupid_ai(p1);
		else random_stupid_ai(ai);
		return;
	}
	if (calc_distance(Map[which_node], Map[node_destination]) == 1)//判斷是否可移動到該位置
	{
		//紀錄在哪個circle移動
		if (which_turn->last_move_circle_num == Map[node_destination]->which_circle)
		{
			which_turn->move_in_same_circle_times++;
		}
		else
		{
			which_turn->move_in_same_circle_times = 1;
			which_turn->last_move_circle_num = Map[node_destination]->which_circle;
		}
		//判斷在某小圈重複移動
		/*if(which_turn->move_in_same_circle_times > 4)
		{
		printf("GAME OVER!!\n");
		if(which_turn->id == 1)
		{
		printf("下方玩家勝利!!!\n");
		printf("(原因:上方玩家連續在同一個circle走超過4次)\n");
		}
		else
		{
		printf("上方玩家勝利!!!\n");
		printf("(原因:下方玩家連續在同一個circle走超過4次)\n");
		}
		gameover();//gameover
		}*/

		//變更地圖棋子資訊
		Map[which_node]->role = 0;
		Map[node_destination]->role = which_turn->id;
		//變更玩家棋子資訊
		for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
		{
			if (which_turn->chess_position[i] == which_node)
			{
				which_turn->chess_position[i] = node_destination;
				break;
			}
		}
		if (which_turn->id == 1)
		{
			judge_rule(p1);//自己下完，判斷對手
		}
		else
		{
			judge_rule(ai);
		}
	}
	else
	{
		//printf("非法的移動:從 %d 到 %d(移動距離不為1)\n",which_node,node_destination);
		printf("Loading...\n");
		system("PAUSE");
		if (which_turn->id == 2)random_stupid_ai(p1);
		else random_stupid_ai(ai);
		return;
	}

}
int evaluation_function(int which_turn, player* top_state, player* down_state)//which_turn 1代表上方，2代表下方
{
	int val = 0;
	//
	int Map_val[NODESIZE] = { 0 };
	Map_val[0] = 500;
	Map_val[1] = 500;
	Map_val[2] = 500;
	Map_val[3] = 5000;
	Map_val[4] = 5000;
	Map_val[5] = 10000;
	Map_val[6] = 5000;
	Map_val[7] = 5000;
	Map_val[8] = 10000;
	Map_val[9] = 1000;
	Map_val[10] = 1000;
	Map_val[11] = 1000;
	Map_val[12] = 10000;
	Map_val[13] = 1000;
	Map_val[14] = 5000;
	Map_val[15] = 5000;
	Map_val[16] = 5000;
	Map_val[17] = 500;
	Map_val[18] = 500;
	Map_val[19] = 500;
	Map_val[20] = 500;

	build_judge(top_state, down_state);
	//判斷輸贏
	if (top_state->current_alive_chess_num <= 2)//下方贏了
	{
		return 100000;
	}
	else if (down_state->current_alive_chess_num <= 2)//上方贏了
	{
		return -100000;
	}
	//判斷對手的棋子數量
	val += (MAX_ALIVE_CHESS_NUM - top_state->current_alive_chess_num) % MAX_ALIVE_CHESS_NUM * 5000;
	//判斷氣
	//判斷自己的位置
	for (int i = 0; i <MAX_ALIVE_CHESS_NUM; ++i)
	{
		val += Map_val[down_state->chess_position[i]];
	}
	//判斷連續走的次數
	if (down_state->move_in_same_circle_times == 1)val -= 0;
	if (down_state->move_in_same_circle_times == 2)val -= 1000;
	if (down_state->move_in_same_circle_times == 3)val -= 5000;
	if (down_state->move_in_same_circle_times == 4)val -= 10000;

	if (which_turn == 2)return val;
	else return (-1)*val;
}
void build_judge(player* top_state, player* down_state)//判斷新的盤面(建樹用)
{
	//重新計算所有位置的氣
	for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)//計算上方所有棋子的space
	{
		if (top_state->chess_position[i] == -1) continue;//被吃掉的就不用判斷
		memset(vis, 0, sizeof(vis));
		top_state->chess_space[i] = space_dfs(top_state, Map[top_state->chess_position[i]], 0);
	}

	for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)//計算下方所有棋子的space
	{
		if (down_state->chess_position[i] == -1) continue;//被吃掉的就不用判斷
		memset(vis, 0, sizeof(vis));
		down_state->chess_space[i] = space_dfs(down_state, Map[down_state->chess_position[i]], 0);
	}

	//判斷某個棋子數目
	for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
	{
		if (top_state->chess_position[i] == -1)continue;//被吃掉的就不用再判斷
		if (top_state->chess_space[i] == 0)//如果某個棋子的氣=0
		{
			Map[top_state->chess_position[i]]->role = 0;//將棋盤該位置變成空的
			top_state->chess_position[i] = -1;//把那個棋子吃掉(設成-1)
			top_state->current_alive_chess_num--;//該方棋子-1
		}
	}
	for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
	{
		if (down_state->chess_position[i] == -1)continue;//被吃掉的就不用再判斷
		if (down_state->chess_space[i] == 0)//如果某個棋子的氣=0
		{
			Map[down_state->chess_position[i]]->role = 0;//將棋盤該位置變成空的
			down_state->chess_position[i] = -1;//把那個棋子吃掉(設成-1)
			down_state->current_alive_chess_num--;//該方棋子-1
		}
	}
}
void build_game_tree(int which_first, player* player_top_state, player* player_down_state, tree* parent_tree_node, int tree_level)
{
	//which_first代表誰要先，(下方，編號2)先下的話則先建p1那層，再建ai那層...
	//player_state 第一次 = {top、down} 代表這層處理哪一方
	if (tree_level == 3)
	{
	}
	else if (which_first == 2)//這一層是處理下方
	{
		for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
		{
			if (player_down_state->chess_position[i] == -1)continue;//被吃掉就不用判斷
			node* ptr = Map[player_down_state->chess_position[i]];
			int temp_instruction_where = player_down_state->chess_position[i];
			while (ptr->connect != NULL)
			{
				ptr = ptr->connect;

				if (Map[ptr->nodeID]->role == 0)//可以走
				{
					tree* tnode = new tree;
					player* temp_state = player_down_state;
					//該state的指令
					tnode->instruction_where = temp_instruction_where;
					tnode->instruction_to = ptr->nodeID;
					//該state的移動
					if (temp_state->last_move_circle_num == Map[tnode->instruction_to]->which_circle)
					{
						temp_state->move_in_same_circle_times++;
					}
					temp_state->chess_position[i] = tnode->instruction_to;
					tnode->top_current_state = copy_value_of_player(player_top_state);//更新current state
					tnode->down_current_state = copy_value_of_player(temp_state);//更新current state
					if (tree_level == 2)tnode->value = evaluation_function(2, tnode->top_current_state, tnode->down_current_state);
					parent_tree_node->next.push_back(tnode);//建立下一層的node
				}
			}
		}
		//建立下一層
		for (int i = 0; i < parent_tree_node->next.size(); ++i)
		{
			if (parent_tree_node->next.at(i)->value == 100000 || parent_tree_node->next.at(i)->value == -100000)//該點已經結束遊戲(沒有下一層)
			{
				getchar();
			}
			else
			{
				player* top_state = copy_value_of_player(parent_tree_node->next.at(i)->top_current_state);
				player* down_state = copy_value_of_player(parent_tree_node->next.at(i)->down_current_state);
				build_game_tree(1, top_state, down_state, parent_tree_node->next.at(i), tree_level + 1);
			}
		}
	}
	else if (which_first == 1)//第一層是上方先建
	{
		for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
		{
			if (player_top_state->chess_position[i] == -1)continue;//被吃掉就不用判斷
			node* ptr = Map[player_top_state->chess_position[i]];
			int temp_instruction_where = player_top_state->chess_position[i];
			while (ptr->connect != NULL)
			{
				ptr = ptr->connect;
				if (Map[ptr->nodeID]->role == 0)//可以走
				{
					tree* tnode = new tree;
					player* temp_state = player_top_state;
					//該state的指令
					tnode->instruction_where = temp_instruction_where;
					tnode->instruction_to = ptr->nodeID;
					//該state的移動
					if (temp_state->last_move_circle_num == Map[tnode->instruction_to]->which_circle)
					{
						temp_state->move_in_same_circle_times++;
					}
					temp_state->chess_position[i] = tnode->instruction_to;
					tnode->top_current_state = copy_value_of_player(temp_state);//更新current state
					tnode->down_current_state = copy_value_of_player(player_down_state);//更新current state
																						//if(tree_level == 1)tnode->value = evaluation_function(1,tnode->top_current_state,tnode->down_current_state);
					parent_tree_node->next.push_back(tnode);//建立下一層的node
				}
			}
		}

		//建立下一層
		for (int i = 0; i < parent_tree_node->next.size(); ++i)
		{
			if (parent_tree_node->next.at(i)->value == 100000 || parent_tree_node->next.at(i)->value == -100000)//該點已經結束遊戲(沒有下一層)
			{
				getchar();
			}
			else
			{
				player* top_state = copy_value_of_player(parent_tree_node->next.at(i)->top_current_state);
				player* down_state = copy_value_of_player(parent_tree_node->next.at(i)->down_current_state);
				build_game_tree(2, top_state, down_state, parent_tree_node->next.at(i), tree_level + 1);
			}
		}
	}
}
//
int temp;
int min_v = 1e9;
int max_v = -1e9;
int min_Max_search(tree* root, int flag_of_minMax) //flag_of_minMax: 0 for min 1 for Max
{
	//第一次呼叫:min_Max_search(root,1)
	if (root->next.size() == 0)
	{
		temp = root->value;
		return temp;
	}
	if (flag_of_minMax == 0)//該層是MIN LEVEL
	{
		min_v = 1e9;
		for (int i = 0; i < root->next.size(); ++i)
		{

			root->next.at(i)->value = min_Max_search(root->next.at(i), 1);
			temp = root->next.at(i)->value;
			if (temp < min_v)
			{
				min_v = temp;
			}
		}
		return min_v;
	}
	else//flag_of_minMax == 1 該層是MAX LEVEL
	{
		max_v = -1e9;
		for (int i = 0; i < root->next.size(); ++i)
		{
			root->next.at(i)->value = min_Max_search(root->next.at(i), 0);
			temp = root->next.at(i)->value;
			if (temp > max_v)
			{
				max_v = temp;
			}
		}
		return max_v;
	}

}


player* copy_value_of_player(player* input)//copy pointer value 
{
	player* temp = new player();
	for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
	{
		temp->chess_position[i] = input->chess_position[i];
		temp->chess_space[i] = input->chess_space[i];
	}
	temp->current_alive_chess_num = input->current_alive_chess_num;
	temp->id = input->id;
	temp->last_move_circle_num = input->last_move_circle_num;
	temp->move_in_same_circle_times = input->move_in_same_circle_times;
	return temp;
}
//--------------------------------------------------------------------
//賴煒勛
//--------------------------------------------------------------------

void stepone02(player* which_turn) // 3-> 5
{

	decision.instruction_where = 3;
	decision.instruction_to = 5;
	move(which_turn, decision.instruction_where, decision.instruction_to);

	Round_for_plarer_first++;
}

void steptwo02(player* which_turn) // [5 -> 10] | [5-> 9] | [5-> 11]
{

	//>>>>>>>>>>>>>>>>>>>>>往中間塞<<<<<<<<<<<<<<<<<<<<<<<<<<
	if ((Map[10]->role == 0)) {
		decision.instruction_where = 5;
		decision.instruction_to = 10;
		move(which_turn, decision.instruction_where, decision.instruction_to);

	}
	else if ((Map[9]->role == 0)) {
		decision.instruction_where = 5;
		decision.instruction_to = 9;
		move(which_turn, decision.instruction_where, decision.instruction_to);

	}
	else if ((Map[11]->role == 0)) {
		decision.instruction_where = 5;
		decision.instruction_to = 11;
		move(which_turn, decision.instruction_where, decision.instruction_to);

	}
	/*----------------------------------------------------*/


	Round_for_plarer_first++;
}


void stepthree02(player* which_turn) //3 -> 5
{

	//>>>>>>>>>>>>>>>>>>>>>往中間塞<<<<<<<<<<<<<<<<<<<<<<<<<<
	//以正中央為優先
	if ((Map[3]->role == 0)) {
		decision.instruction_where = 1;
		decision.instruction_to = 3;
		move(which_turn, decision.instruction_where, decision.instruction_to);

	}
	/*----------------------------------------------------*/

	Round_for_plarer_first++;
}


void random_stupid_ai_2(player* which_turn)
{
	if (Round_for_plarer_first == 1)
	{
		stepone02(which_turn);
	}
	else if (Round_for_plarer_first == 2)
	{
		steptwo02(which_turn);
	}
	else if (Round_for_plarer_first == 3)
	{
		stepthree02(which_turn);
	}

	else {

		possible_state.clear();//清空可能的下法
		for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
		{
			if (which_turn->chess_position[i] == -1)continue;//被吃掉的就不用判斷
			node* ptr = Map[which_turn->chess_position[i]];
			while (ptr->connect != NULL)
			{
				ptr = ptr->connect;
				if (Map[ptr->nodeID]->role == 0)
				{
					//tree* new_node = new tree(0,p1->chess_position[i],ptr->nodeID);
					state temp(which_turn->chess_position[i], ptr->nodeID);
					possible_state.push_back(temp);//加入可能的下法
				}
			}
		}
		//如果在同個圈圈中走3步且下一步也是同個圈圈，則重新亂數產生一個decision
		int rand_decision_num;
		bool right_decision = true;
		while (right_decision)
		{
			right_decision = false;
			rand_decision_num = rand() % possible_state.size();
			if (which_turn->move_in_same_circle_times == 3 && which_turn->last_move_circle_num == Map[possible_state.at(rand_decision_num).instruction_where]->which_circle)
			{
				right_decision = true;
			}
		}
		//走下一步
		decision.instruction_where = possible_state.at(rand_decision_num).instruction_where;
		decision.instruction_to = possible_state.at(rand_decision_num).instruction_to;
		move(which_turn, decision.instruction_where, decision.instruction_to);
	}
}
void random_stupid_ai(player* which_turn)
{
	possible_state.clear();//清空可能的下法
	for (int i = 0; i < MAX_ALIVE_CHESS_NUM; ++i)
	{
		if (which_turn->chess_position[i] == -1)continue;//被吃掉的就不用判斷
		node* ptr = Map[which_turn->chess_position[i]];
		while (ptr->connect != NULL)
		{
			ptr = ptr->connect;
			if (Map[ptr->nodeID]->role == 0)
			{
				//tree* new_node = new tree(0,p1->chess_position[i],ptr->nodeID);
				state temp(which_turn->chess_position[i], ptr->nodeID);
				possible_state.push_back(temp);//加入可能的下法
			}
		}
	}
	//如果在同個圈圈中走3步且下一步也是同個圈圈，則重新亂數產生一個decision
	int rand_decision_num;
	bool right_decision = true;
	while (right_decision)
	{
		right_decision = false;
		rand_decision_num = rand() % possible_state.size();
		if (which_turn->move_in_same_circle_times == 3 && which_turn->last_move_circle_num == Map[possible_state.at(rand_decision_num).instruction_where]->which_circle)
		{
			right_decision = true;
		}
	}
	//走下一步
	decision.instruction_where = possible_state.at(rand_decision_num).instruction_where;
	decision.instruction_to = possible_state.at(rand_decision_num).instruction_to;
	move(which_turn, decision.instruction_where, decision.instruction_to);
}
void strong_ai()
{
	//ai在下方
	tree* game_tree = new tree();
	tree_level = 0;
	game_tree->top_current_state = copy_value_of_player(ai);
	game_tree->down_current_state = copy_value_of_player(p1);
	build_game_tree(2, game_tree->top_current_state, game_tree->down_current_state, game_tree, 0);

	temp = -1e9;
	min_Max_search(game_tree, 0);

	//找minMax第一層中最大的做決策
	int M = -1e9;
	for (int i = 0; i < game_tree->next.size(); ++i)
	{
		if (game_tree->next.at(i)->value > M)
		{
			M = game_tree->next.at(i)->value;
			decision.instruction_where = game_tree->next.at(i)->instruction_where;
			decision.instruction_to = game_tree->next.at(i)->instruction_to;
		}
	}

	move(p1, decision.instruction_where, decision.instruction_to);
}



void stepone() {
	if (direction == 0) {
		decision.instruction_where = 16;
		decision.instruction_to = 12;
		move(p1, decision.instruction_where, decision.instruction_to);
		Round++;
	}
	if (direction == 1) {
		if ((Map[8]->role == 0) && (Map[4]->role == 1)) {
			decision.instruction_where = 4;
			decision.instruction_to = 8;
			move(ai, decision.instruction_where, decision.instruction_to);
		}
		else if ((Map[12]->role == 0) && (Map[6]->role == 1)) {
			decision.instruction_where = 6;
			decision.instruction_to = 12;
			move(ai, decision.instruction_where, decision.instruction_to);
		}
		Round++;
	}
}

void steptwo() {
	if (direction == 0) {
		if (Map[13]->role == 1 && Map[16]->role == 0 && Map[20]->role == 2) {
			decision.instruction_where = 20;
			decision.instruction_to = 16;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[8]->role == 0 && Map[14]->role == 2) {
			decision.instruction_where = 14;
			decision.instruction_to = 8;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else {
			if (Map[15]->role == 0 && Map[17]->role == 2) {
				decision.instruction_where = 17;
				decision.instruction_to = 15;
				move(p1, decision.instruction_where, decision.instruction_to);
			}
		}
		Round++;
	}
	if (direction == 1) {
		if (Map[13]->role == 2 && Map[6]->role == 0 && Map[2]->role == 1) {
			decision.instruction_where = 2;
			decision.instruction_to = 6;
			move(ai, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[8]->role == 0 && Map[4]->role == 1) {
			decision.instruction_where = 4;
			decision.instruction_to = 8;
			move(ai, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[12]->role == 0 && Map[6]->role == 1) {
			decision.instruction_where = 6;
			decision.instruction_to = 12;
			move(ai, decision.instruction_where, decision.instruction_to);
		}
		else {
			if (Map[5]->role == 0 && Map[3]->role == 1) {
				decision.instruction_where = 3;
				decision.instruction_to = 5;
				move(ai, decision.instruction_where, decision.instruction_to);
			}
		}
		Round++;
	}
}

void stepthree() {
	if (direction == 0) {
		if (Map[8]->role == 0 && Map[14]->role == 2) {
			decision.instruction_where = 14;
			decision.instruction_to = 8;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[13]->role == 1 && Map[16]->role == 0 && Map[20]->role == 2) {
			decision.instruction_where = 20;
			decision.instruction_to = 16;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[7]->role == 1 && Map[14]->role == 0 && Map[18]->role == 2) {
			decision.instruction_where = 18;
			decision.instruction_to = 14;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[17]->role == 0 && Map[19]->role == 2) {
			decision.instruction_where = 19;
			decision.instruction_to = 17;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else {
			if (Map[15]->role == 0 && Map[17]->role == 2) {
				decision.instruction_where = 17;
				decision.instruction_to = 15;
				move(p1, decision.instruction_where, decision.instruction_to);
			}
		}
		Round++;
	}
	if (direction == 1) {
		if (Map[12]->role == 0 && Map[6]->role == 1) {
			decision.instruction_where = 6;
			decision.instruction_to = 12;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[8]->role == 0 && Map[4]->role == 1) {
			decision.instruction_where = 4;
			decision.instruction_to = 8;
			move(ai, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[13]->role == 2 && Map[6]->role == 0 && Map[2]->role == 1) {
			decision.instruction_where = 2;
			decision.instruction_to = 6;
			move(ai, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[7]->role == 2 && Map[4]->role == 0 && Map[0]->role == 1) {
			decision.instruction_where = 0;
			decision.instruction_to = 4;
			move(ai, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[3]->role == 0 && Map[1]->role == 1) {
			decision.instruction_where = 1;
			decision.instruction_to = 3;
			move(ai, decision.instruction_where, decision.instruction_to);
		}
		else {
			if (Map[5]->role == 0 && Map[3]->role == 1) {
				decision.instruction_where = 3;
				decision.instruction_to = 5;
				move(ai, decision.instruction_where, decision.instruction_to);
			}
		}
		Round++;
	}
}

void stepfour() {
	if (direction == 0) {
		if (Map[7]->role == 1 && Map[14]->role == 0 && Map[18]->role == 2) {
			decision.instruction_where = 18;
			decision.instruction_to = 14;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[13]->role == 1 && Map[16]->role == 0 && Map[20]->role == 2) {
			decision.instruction_where = 20;
			decision.instruction_to = 16;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (((Map[9]->role == 1) || (Map[10]->role == 1) || (Map[11]->role == 1)) && (Map[15]->role == 0 && Map[17]->role == 2)) {
			decision.instruction_where = 17;
			decision.instruction_to = 15;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[19]->role == 2 && Map[17]->role == 0 && Map[19]->role == 2) {
			decision.instruction_where = 19;
			decision.instruction_to = 17;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else {
			if ((Map[15]->role == 0) && (Map[17]->role == 2)) {
				decision.instruction_where = 17;
				decision.instruction_to = 15;
				move(p1, decision.instruction_where, decision.instruction_to);
			}
			else {
				//cout<<"17-role : "<<Map[17]->role<<endl;
				//cout<<"19-role : "<<Map[19]->role<<endl;
				if ((Map[17]->role == 0) && (Map[19]->role == 2))
				{
					//cout<<"AAAAA"<<endl;
					decision.instruction_where = 19;
					decision.instruction_to = 17;
					move(p1, decision.instruction_where, decision.instruction_to);
				}
				//center();
				else
				{
					//cout<<"/*/*/*/*/*/*/*/*"<<endl;
					Round = Round + 2;
					//cout<<"Round/*/*/*/*/*/*/*/*/ : "<<Round<<endl;
					center();
				}
			}
		}
		Round++;
	}

}

void stepfive() {
	if (direction == 0) {
		if (Map[7]->role == 1 && Map[14]->role == 0 && Map[18]->role == 2) {
			decision.instruction_where = 18;
			decision.instruction_to = 14;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[13]->role == 1 && Map[16]->role == 0 && Map[20]->role == 2) {
			decision.instruction_where = 20;
			decision.instruction_to = 16;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (((Map[9]->role == 1) || (Map[10]->role == 1) || (Map[11]->role == 1)) && (Map[15]->role == 0) && Map[17]->role == 2) {
			decision.instruction_where = 17;
			decision.instruction_to = 15;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if ((Map[14]->role == 2) && (Map[16]->role == 2) && Map[17]->role == 0 && Map[19]->role == 2) {
			decision.instruction_where = 19;
			decision.instruction_to = 17;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else {
			if ((Map[15]->role == 0) && (Map[17]->role == 2)) {
				decision.instruction_where = 17;
				decision.instruction_to = 15;
				move(p1, decision.instruction_where, decision.instruction_to);
			}
			else {
				if ((Map[17]->role == 0) && (Map[19]->role == 2)) {
					decision.instruction_where = 19;
					decision.instruction_to = 17;
					move(p1, decision.instruction_where, decision.instruction_to);
				}
				else
				{
					Round++;
					center();
				}
			}
		}
		Round++;
	}
}

void center() {
	//cout<<"INTO CENTER !"<<endl;
	if (direction == 0) {
		if (Map[8]->role == 0 && Map[14]->role == 2) {
			//cout<<"1 : "<<endl;
			decision.instruction_where = 14;
			decision.instruction_to = 8;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else if (Map[7]->role == 1 && Map[14]->role == 0 && Map[18]->role == 2) {
			//cout<<"2 : "<<endl;
			decision.instruction_where = 18;
			decision.instruction_to = 14;
			move(p1, decision.instruction_where, decision.instruction_to);
		}
		else {
			//cout<<"3 : "<<endl;
			while (1) {
				int i = 0, j = 0;
				int t = rand() % 4;
				if (t == 0) {
					i = 9;
				}
				else if (t == 1) {
					i = 10;
				}
				else if (t == 2) {
					i = 11;
				}
				else if (t == 3) {
					i = 15;
				}
				int p = rand() % 2;
				if (p == 0) {
					//cout<<"4 : "<<endl;
					if (Map[5]->role == 0) {
						if (calc_distance(Map[currentp1], Map[5]) == 1) {
							//cout<<"5 : "<<endl;
							decision.instruction_where = currentp1;
							decision.instruction_to = 5;
							move(p1, decision.instruction_where, decision.instruction_to);
							currentp1 = 5;
							Round++;
							break;
						}
						else {
							//cout<<"6 : "<<endl;
						}
					}
					if (Map[i]->role == 0) {
						//cout<<"7 : "<<endl;
						if (calc_distance(Map[currentp1], Map[i]) == 1) {
							//cout<<"8 : "<<endl;
							decision.instruction_where = currentp1;
							decision.instruction_to = i;
							move(p1, decision.instruction_where, decision.instruction_to);
							currentp1 = i;
							break;
						}
						else {
							continue;
						}
					}
				}
				else if (p == 1) {
					//cout<<"9 : "<<endl;
					if (Map[5]->role == 0) {
						//cout<<"10 : "<<endl;
						if (calc_distance(Map[currentp2], Map[5]) == 1) {
							//cout<<"11 : "<<endl;
							decision.instruction_where = currentp2;
							decision.instruction_to = 5;
							move(p1, decision.instruction_where, decision.instruction_to);
							currentp2 = 5;
							Round++;
							break;
						}
						else {
							//cout<<"12 : "<<endl;
						}
					}
					if (Map[i]->role == 0) {
						//cout<<"13 : "<<endl;
						if (calc_distance(Map[currentp2], Map[i]) == 1) {
							//cout<<"14 : "<<endl;
							decision.instruction_where = currentp2;
							decision.instruction_to = i;
							move(p1, decision.instruction_where, decision.instruction_to);
							currentp2 = i;
							break;
						}
						else {
							//cout<<"15 : "<<endl;
							//break;
							continue;
						}
					}
				}
			}
		}
	}
}

void little_smart_ai()//有點聰明的AI
{
	//條件:AI在下方(p1處)
	if (Round == 1)
	{
		stepone();
	}
	else if (Round == 2)
	{
		steptwo();
	}
	else if (Round == 3)
	{
		stepthree();
	}
	else if (Round == 4)
	{
		stepfour();
	}
	else if (Round == 5)
	{
		stepfive();
	}
	else if (Round == 6)
	{
		//cout<<"little smart AI Round : "<<Round<<endl;
		//cout<<"how_many_time_in_center : "<<how_many_time_in_center<<endl;
		center();
		how_many_time_in_center++;
	}
	//else if (Round >= 6)attack();
	else if (Round >6)
	{
		random_stupid_ai(p1);
	}
}


int main()
{
	init();

	//build_game_tree(2,game_tree->top_current_state,game_tree->down_current_state,game_tree);//ai(下方)先下
	//test_distance();
	//getchar();
	printf("   【請選擇模式】  \n輸入:1  ->ai(下方)先下  \n輸入:2  ->p1(下方)先下  \n輸入:3  ->ai(下方)先下【高級AI搜尋】 \n");
	int n = 0;
	int ai_orgin = 0, ai_destination = 0;
	int p1_orgin = 0, p1_destination = 0;
	int round = 1;
	while (true)
	{
		scanf("%d", &n);
		system("cls");
		switch (n)
		{
			/*
			case 1:
			while(true)
			{
			system("cls");
			//printf("ai下哪?\n");
			//scanf("%d %d",&ai_orgin,&ai_destination);
			//move(ai,ai_orgin,ai_destination);//哪一方，那顆棋子(棋盤編號)，移到哪裡(棋盤編號)
			random_stupid_ai(ai);
			printMap();
			printf("ai的移動:從 %d 到 %d\n",decision.instruction_where,decision.instruction_to);

			printf("p1下哪?\n");
			scanf("%d %d",&p1_orgin,&p1_destination);
			move(p1,p1_orgin,p1_destination);//哪一方，那顆棋子(棋盤編號)，移到哪裡(棋盤編號)
			printf("p1的移動:從 %d 到 %d\n",p1_orgin,p1_destination);
			}
			break;
			case 2:
			while(true)
			{
			printMap();
			printf("p1下哪?\n");
			scanf("%d %d",&p1_orgin,&p1_destination);
			system("cls");
			move(p1,p1_orgin,p1_destination);//哪一方，那顆棋子(棋盤編號)，移到哪裡(棋盤編號)
			printf("p1的移動:從 %d 到 %d\n",p1_orgin,p1_destination);

			//printf("ai下哪?\n");
			//scanf("%d %d",&ai_orgin,&ai_destination);
			//move(ai,ai_orgin,ai_destination);//哪一方，那顆棋子(棋盤編號)，移到哪裡(棋盤編號)

			random_stupid_ai(ai);
			printf("ai的移動:從 %d 到 %d\n",decision.instruction_where,decision.instruction_to);
			}
			break;
			*/

		case 1:
			while (true)
			{
				cout << "※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※" << endl;
				//system("cls");
				cout << "Round : " << round << endl;
				cout << "P1( O ) 剩子個數 :" << (ai->current_alive_chess_num) << endl;
				cout << "AI( X ) 剩子個數 :" << (p1->current_alive_chess_num) << endl;

				//direction = 0;
				little_smart_ai();
				printMap();
				printf("AI( X )的移動:從 %d 到 %d\n", decision.instruction_where, decision.instruction_to);
				printf("P1( O )下哪?\n");
				printf("輸入:起點 目的\n");

				//>>-------------判斷輸入 is 正常 合法的<<-------------------
				while (true) {

					scanf("%d %d", &p1_orgin, &p1_destination);

					if (Map[p1_orgin]->role == 1 && Map[p1_destination]->role == 0)
					{
						//printf("合法從 %d 到 %d\n",decision.instruction_where,decision.instruction_to);
						break;
					}
					else
					{
						printf("不合法的輸入\n");
						//printf("不合法從 %d 到 %d\n",decision.instruction_where,decision.instruction_to);
						continue;
					}
				}
				//------------------------------------------------------------

				move(ai, p1_orgin, p1_destination);//哪一方，那顆棋子(棋盤編號)，移到哪裡(棋盤編號)
				cout << "※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※" << endl;
				cout << endl << endl;
				printf("P1( O )的移動:從 %d 到 %d\n", p1_orgin, p1_destination);
				round++;

			}
			break;

		case 2:
			while (true)
			{
				cout << "※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※" << endl;
				//system("cls");
				cout << "Round : " << round << endl;
				cout << "AI( O ) 剩子個數 :" << (ai->current_alive_chess_num) << endl;
				cout << "P1( X ) 剩子個數 :" << (p1->current_alive_chess_num) << endl;


				printMap();

				//printf("AI( X )的移動:從 %d 到 %d\n",decision.instruction_where,decision.instruction_to);
				printf("P1( X )下哪?\n");
				printf("輸入:起點 目的\n");

				//>>-------------判斷輸入 is 正常 合法的<<-------------------
				while (true) {

					scanf("%d %d", &p1_orgin, &p1_destination);

					if (Map[p1_orgin]->role == 2 && Map[p1_destination]->role == 0)
					{
						//printf("合法從 %d 到 %d\n",decision.instruction_where,decision.instruction_to);
						break;
					}
					else
					{
						printf("不合法的輸入\n");
						//printf("不合法從 %d 到 %d\n",decision.instruction_where,decision.instruction_to);
						continue;
					}
				}
				//------------------------------------------------------------

				//scanf("%d %d",&p1_orgin,&p1_destination);
				//system("cls");
				move(p1, p1_orgin, p1_destination);//哪一方，那顆棋子(棋盤編號)，移到哪裡(棋盤編號)
				printf("P1( X )的移動:從 %d 到 %d\n", p1_orgin, p1_destination);



				random_stupid_ai_2(ai);

				cout << "※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※" << endl;
				cout << endl << endl;
				printf("AI( O )的移動:從 %d 到 %d\n", decision.instruction_where, decision.instruction_to);

				round++;

			}
			break;
		case 3:
			while (true)
			{
				system("cls");
				//printf("ai下哪?\n");
				//scanf("%d %d",&ai_orgin,&ai_destination);
				//move(ai,ai_orgin,ai_destination);//哪一方，那顆棋子(棋盤編號)，移到哪裡(棋盤編號)	

				//random_stupid_ai(p1);
				direction = 0;
				strong_ai();
				printMap();
				printf("AI的移動:從 %d 到 %d\n", decision.instruction_where, decision.instruction_to);

				printf("p1下哪?\n");
				scanf("%d %d", &p1_orgin, &p1_destination);
				move(ai, p1_orgin, p1_destination);//哪一方，那顆棋子(棋盤編號)，移到哪裡(棋盤編號)
				printf("p1的移動:從 %d 到 %d\n", p1_orgin, p1_destination);
			}
			break;
		default:
			printf("錯誤輸入!請重新輸入!!\n");
		}
	}

	system("PAUSE");
	return 0;
}