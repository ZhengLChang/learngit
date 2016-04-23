#include <iostream>
#include <list>
#include <queue>
#include <string>

using namespace std;

template<class Vertex>
class Graph
{
private:
	list<int> *HeadNodes;
	int n;
public:
	Graph(const int vertices = 0):n(vertices)
	{
		HeadNodes = new list<int>[n];
	}
	void BFS(int v)
	{
		visited = new bool[n];
		for(int i = 0; i < n; i++)
		{
			visited[i] = false;
		}
		visited[v] = true;
		queue<int> q;
		q.push(v);
		while(!q.empty())
		{
			v = q.front();
			q.pop();
			for()
		}
	}
#if 0
	void InsertVertex(Vertex v);
	void InsertEdge(Vertex u, Vertex v);
	void DeleteVertex(Vertex v);
	void DeleteEdge(Vertex u, Vertex v);
	bool IsEmpty();
	list<Vertex> Adjacent(Vertex v);
#endif
};











