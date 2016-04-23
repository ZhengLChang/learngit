#include <iostream>
#include <queue>
#include <vector>
#include <list>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>

using namespace std;

typedef struct Table_
{
	int Known;
	int Dist;
#if 0
	int Path;
#endif
}Table;

typedef struct AdjacentTable_
{
	int index;
	struct AdjacentTable_ *next;
}AdjacentTable;

int insertAdj(AdjacentTable *head, int index);

void Unweighted(Table *T, vector<vector<int> > &adj, int NumVertex, int begin)
{
	queue<int> que;
	int front_value;
	int tmp;
	que.push(begin);
	while(!que.empty())
	{
		front_value = que.front();
		que.pop();
		T[front_value].Known = 1;
		for(tmp = 0; tmp < adj[front_value].size(); tmp++)
		{
			int p = adj[front_value][tmp];
			if(T[p].Known == 0)
			{
				que.push(p);
				if(T[p].Dist == -1)
				{
					T[p].Dist = T[front_value].Dist + 1;
				}
			}

		}
	}
}
#if 0
void Unweighted(Table *T, AdjacentTable *adj, int NumVertex, int begin)
{
	AdjacentTable tmp, *tmp1 = NULL;
	int vertex;
	tmp.index = 0;
	tmp.next = NULL;

	insertAdj(&tmp, begin);	
	for(; tmp.next != NULL;)
	{
		vertex = tmp.next->index;
		tmp1 = tmp.next;
		tmp.next = tmp1->next;
		free(tmp1);
		tmp1 = NULL;
		for(tmp1 = adj[vertex].next; tmp1->next != NULL; tmp1 = tmp1->next)
		{
			if(T[tmp1->index].Dist == -1 || T[tmp1->index].Dist > T[vertex].Dist + 1)
			{
				T[tmp1->index].Dist = T[vertex].Dist + 1;
				insertAdj(&tmp, tmp1->index);	
			}
		}
	}
       	return ;
}
#endif
#if 1
int insertAdj(vector<int> &adj, int index)
{
	adj.push_back(index);
	/*
	tmp1 = malloc(sizeof(AdjacentTable));
	if(tmp1 == NULL)
	{
		printf("malloc error: %s\n", strerror(errno));
		return -1;
	}
	tmp1->index = index;
	tmp1->next = head->next;
	head->next = tmp1;
	*/
	return 0;
}
#endif
int main(void)
{
	vector<vector<int> > adj(7);
	int i = 0, j = 0, begin = 0;
	Table T[7];
	int adjTable[7][4] = 
	{
		{1, 2, 3, 0},
		{3, 4, 0, 0},
		{5, 0, 0, 0},
		{2, 5, 6, 0},
		{3, 6, 0, 0},
		{0, 0, 0, 0},
		{5, 0, 0, 0}
	};
	for(i = 0; i < 7; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if(adjTable[i][j] != 0)
			{
				insertAdj(adj[i], adjTable[i][j]);
			}
		}
	}
	for(i = 0; i < 7; i++)
	{
		T[i].Known = 0;
		T[i].Dist = -1;
#if 0
		T[i].Path = 0;
#endif
	}
	T[begin].Dist = 0;
	Unweighted(T,adj, 7, begin);
	for(i = 0; i < 7; i++)
	{
		printf("%d\n", T[i].Dist);
	}
	return 0;
}























