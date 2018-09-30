

#ifndef _ASTAR_ALGORITHM_H
#define _ASTAR_ALGORITHM_H
#include "cocos2d.h"
#include "battle/Map/MapHandler.h"

//map marks
#define  AVAIL     0
#define  UNAVAIL  -1
#define  START   100
#define  END     111
#define  ROAD     10
#define GET_F(X)  (X->G + X->H)


typedef struct st_StarNode
{
	//for st_StarNode itself
	int type; //st_StarNode type
	int i; //i index
	int j; //j index
	//for A star algorithm
	double G; //past road cost
	double H; //heuristic, F = G + H
	struct st_StarNode* parent; //parent st_StarNode, used for trace road
	struct st_StarNode* next; //only used for open and close list
}st_StarNode;

//==========================open close list operation================
st_StarNode* m_pOpenList;
st_StarNode* m_pCloseList;
int   m_iStartX;
int   m_iStartY;
int   m_iEndX;
int   m_iEndY;

bool IsStart( int x,int y)
{
	return (x == m_iStartX && y == m_iStartY);
}

bool IsEnd( int x,int y)
{
	return (x == m_iEndX && y == m_iEndY);
}

void init_openlist()
{
	m_pOpenList = NULL;
}
void init_closelist()
{
	m_pCloseList = NULL;
}
void destroy_openlist()
{
	st_StarNode* q;
	st_StarNode* p = m_pOpenList;
	while (p != NULL)
	{
		q = p->next;
		free(p);
		p = q;
	}
}
void destroy_closelist()
{
	st_StarNode* q;
	st_StarNode* p = m_pCloseList;
	while (p != NULL)
	{
		q = p->next;
		free(p);
		p = q;
	}
}
void insert_into_openlist(st_StarNode* new_st_StarNode) //insert and sort by F
{
	st_StarNode* p;
	st_StarNode* q;
	if (m_pOpenList == NULL)
	{
		m_pOpenList = new_st_StarNode; //insert as the first
		return;
	}
	p = m_pOpenList;
	while (p != NULL)
	{
		q = p;
		p = p->next;
		if (p == NULL)
		{
			q->next = new_st_StarNode; //insert as the last
			return;
		}
		else if (GET_F(new_st_StarNode) < GET_F(p))
		{
			q->next = new_st_StarNode; //insert before p, sorted
			new_st_StarNode->next = p;
			return;
		}
	}
	
}
void insert_into_closelist(st_StarNode* new_st_StarNode) //just insert before head
{
	if (m_pCloseList == NULL)
	{
		m_pCloseList = new_st_StarNode; //insert as the first
		return;
	}
	else
	{
		new_st_StarNode->next = m_pCloseList; //insert before head
		m_pCloseList = new_st_StarNode;
		return;
	}
}
st_StarNode* find_st_StarNode_in_list_by_ij(st_StarNode* st_StarNode_list, int di, int dj)
{
	st_StarNode* p = st_StarNode_list;
	while (p)
	{
		if (p->i == di && p->j == dj)
			return p;
		p = p->next;
	}
	return NULL;
}
st_StarNode* pop_firstst_StarNode_from_openlist() //get the minimum st_StarNode sorted by F
{
	st_StarNode* p = m_pOpenList;
	if (p == NULL)
	{
		return NULL;
	}
	else
	{
		m_pOpenList = p->next;
		p->next = NULL;
		return p;
	}
}
void remove_st_StarNode_from_openlist(st_StarNode* nd) //just remove it, do not destroy it
{
	st_StarNode* q;
	st_StarNode* p = m_pOpenList;
	if (m_pOpenList == nd)
	{
		m_pOpenList = m_pOpenList->next;
		return;
	}
	while (p)
	{
		q = p;
		p = p->next;
		if (p == nd) //found
		{
			q->next = p->next;
			p->next = NULL;
			return;
		}
	}
}
void remove_st_StarNode_from_closelist(st_StarNode* nd) //just remove it, do not destroy it
{
	st_StarNode* q;
	st_StarNode* p = m_pCloseList;
	if (m_pCloseList == nd)
	{
		m_pCloseList = m_pCloseList->next;
		return;
	}
	while (p)
	{
		q = p;
		p = p->next;
		if (p == nd) //found
		{
			q->next = p->next;
			p->next = NULL;
			return;
		}
	}
}
//===================================================================
//=======================calculate H, G =============================
//calculate Heuristic value
//(reimplemented when porting a star to another application)
double calc_H(int cur_i, int cur_j, int end_i, int end_j)
{
	return (abs(end_j - cur_j) + abs(end_i - cur_i)) * 10.0; //the heuristic
}
//calculate G value
//(reimplemented when porting a star to another application)
double calc_G(st_StarNode* cur_st_StarNode)
{
	st_StarNode* p = cur_st_StarNode->parent;
	if (abs(p->i - cur_st_StarNode->i) + abs(p->j - cur_st_StarNode->j) > 1)
		return 14.0 + p->G; //the diagonal cost is 14
	else
		return 10.0 + p->G; //the adjacent cost is 10
}
void init_start_st_StarNode(st_StarNode* st, int si, int sj, int ei, int ej)
{
	memset(st, 0, sizeof(st_StarNode));
	st->type = START;
	st->i = si;
	st->j = sj;
	st->H = calc_H(si, sj, ei, ej);
	st->G = 0;
}
void init_end_st_StarNode(st_StarNode* ed, int ei, int ej)
{
	memset(ed, 0, sizeof(st_StarNode));
	ed->type = END;
	ed->i = ei;
	ed->j = ej;
	ed->H = 0;
	ed->G = 9999; //temp value
}
void init_pass_st_StarNode(st_StarNode* pd, int pi, int pj)
{
	memset(pd, 0, sizeof(st_StarNode));
	pd->type = AVAIL;
	pd->i = pi;
	pd->j = pj;
}
//check the candidate st_StarNode (i,j) when extending parent_st_StarNode
int check_neighbor(int width, int height, int di, int dj, st_StarNode* parent_st_StarNode, st_StarNode* end_st_StarNode)
{
	st_StarNode* p;
	st_StarNode* temp;
	double new_G;

	//µØÍ¼check...
	if (di < 0 || dj < 0 || dj > height-1 || di > width-1)
		return UNAVAIL;

	//1. get map handler
	//if (sgMapHandler()->CheckCollision(Vec2(di, dj)))
	//{
	//	return UNAVAIL ;
 	//}

	//2. check if existed in close list
	p = find_st_StarNode_in_list_by_ij(m_pCloseList, di, dj); 
	if (p != NULL)
	{
		//found in the closed list, check if the new G is better,
		temp = p->parent;
		p->parent = parent_st_StarNode;
		new_G = calc_G(p);
		if (new_G >= p->G)
		{
			p->parent = temp; //if new_G is worse, recover the parent
		}
		else
		{
			//the new_G is better, remove it from close list, insert it into open list
			p->G = new_G;
			remove_st_StarNode_from_closelist(p); //remove it
			insert_into_openlist(p); //insert it, sorted
		}
		return AVAIL;
	}
	//3. check if existed in open list
	p = find_st_StarNode_in_list_by_ij(m_pOpenList, di, dj); //in open list
	if (p != NULL)
	{
		//found in the open list, check if the new G is better
		temp = p->parent;
		p->parent = parent_st_StarNode;
		new_G = calc_G(p);
		if (new_G >= p->G)
		{
			p->parent = temp; //if new_G is worse, recover the parent
		}
		else
		{
			//the new_G is better, resort the list
			p->G = new_G;
			remove_st_StarNode_from_openlist(p); //remove it
			insert_into_openlist(p); //insert it, sorted
		}
		return AVAIL;
	}
	
	//4. none of above, insert a new st_StarNode into open list
	//if (map[di][dj] == END)
	if(IsEnd(di,dj))
	{
		//4~. check if it is end st_StarNode
		end_st_StarNode->parent = parent_st_StarNode;
		end_st_StarNode->G = calc_G(end_st_StarNode);
		insert_into_openlist(end_st_StarNode); //insert into openlist
		return AVAIL;
	}
	else
	{
		//4~~. create a new st_StarNode
		//int size = sizeof(st_StarNode);
		p = (st_StarNode * )malloc( sizeof(st_StarNode));
		init_pass_st_StarNode(p, di, dj);
		p->parent = parent_st_StarNode;
		p->H = calc_H(di, dj, end_st_StarNode->i, end_st_StarNode->j);
		p->G = calc_G(p);
		insert_into_openlist(p); //insert into openlist
		return AVAIL;
	}
}
//extend the current st_StarNode on the map
//(reimplemented when porting a star to another application)
void extend_st_StarNode(st_StarNode* cd, int width, int height, st_StarNode* end_st_StarNode)
{
	int up_status, down_status, left_status, right_status;
	int ci, cj; //cur st_StarNode i, j
	int ti, tj; //temp i, j
	ci = cd->i;
	cj = cd->j;
	//1. up
	ti = ci - 1;
	tj = cj;
	up_status = check_neighbor( width, height, ti, tj, cd, end_st_StarNode);
	//2. down
	ti = ci + 1;
	tj = cj;
	down_status = check_neighbor( width, height, ti, tj, cd, end_st_StarNode);
	//3. left
	ti = ci;
	tj = cj - 1;
	left_status = check_neighbor( width, height, ti, tj, cd, end_st_StarNode);
	//4. right
	ti = ci;
	tj = cj + 1;
	right_status = check_neighbor( width, height, ti, tj, cd, end_st_StarNode);
	//5. leftup
	ti = ci - 1;
	tj = cj - 1;
	if (up_status == AVAIL && left_status == AVAIL)
		check_neighbor( width, height, ti, tj, cd, end_st_StarNode);
	//6. rightup
	ti = ci - 1;
	tj = cj + 1;
	if (up_status == AVAIL && right_status == AVAIL)
		check_neighbor( width, height, ti, tj, cd, end_st_StarNode);
	//7. leftdown
	ti = ci + 1;
	tj = cj - 1;
	if (down_status == AVAIL && left_status == AVAIL)
		check_neighbor( width, height, ti, tj, cd, end_st_StarNode);
	//8. rightdown
	ti = ci + 1;
	tj = cj + 1;
	if (down_status == AVAIL && right_status == AVAIL)
		check_neighbor( width, height, ti, tj, cd, end_st_StarNode);
	
}

//=======================search algorithm======================================

//¿ªÊ¼ A* patch
st_StarNode* a_star_search( int width, int height, int start_i, int start_j, int end_i, int end_j)
{
	st_StarNode* cur_st_StarNode;
	st_StarNode* start_st_StarNode;
	st_StarNode* end_st_StarNode;
	//create start and end st_StarNode
	m_iStartX = start_i ;
	m_iStartY = start_j;
	m_iEndX   = end_i;
	m_iEndY  = end_j ;

	int size = sizeof(st_StarNode);
	start_st_StarNode = (st_StarNode *)malloc(size);
	init_start_st_StarNode(start_st_StarNode, start_i, start_j, end_i, end_j);
	end_st_StarNode = (st_StarNode *) malloc(size);
	init_end_st_StarNode(end_st_StarNode, end_i, end_j);
	
	//init open and close list
	init_openlist();
	init_closelist();
	//put start_st_StarNode into open list
	insert_into_openlist(start_st_StarNode);
	
	//start searching
	while (1)
	{
		cur_st_StarNode = pop_firstst_StarNode_from_openlist(); //it has the minimum F value
		if (cur_st_StarNode == NULL || cur_st_StarNode->type == END)
		{
			break; //found the road or no road found
		}
		
		extend_st_StarNode(cur_st_StarNode,  width, height, end_st_StarNode); //the key step!!
		insert_into_closelist(cur_st_StarNode);
	}
	//you can track the road by the st_StarNode->parent
	return cur_st_StarNode;
}

#endif /* file end */
