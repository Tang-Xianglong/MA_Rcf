//
//  Dijkstra.cpp
//  MA_Rcf
//
//  Dijkstra算法的定义
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#include"Dijkstra.hpp"
#include <vector>
void Dijkstra(
              const int num_of_vertex,	//节点数目
              const int start_vertex,		//源节点
              int** G,				//有向图邻接矩阵
              int *distance,				//各个节点到达源节点的距离
              int *pre_vertex				//各个节点的前一个节点
)
{
    std::vector<bool> is_inS;		//是否已经在S集合
    is_inS.reserve(0);
    is_inS.assign(num_of_vertex, false);	//初始化，所有的节点都不在S中
    
    /*初始化distance和pre_vertex数组*/
    for (int i=0; i < num_of_vertex; ++i)
    {
        distance[i] = G[start_vertex][i];
        if(G[start_vertex][i] < INT_MAX)
            pre_vertex[i] = start_vertex;
        else
            pre_vertex[i] = -1;
    }
    pre_vertex[start_vertex] = -1;
    
    /*开始实用贪心思想循环处理不在S集合中的每一个节点*/
    is_inS[start_vertex] = true;		//源节点放入S集合
    
    int u = start_vertex;
    
    for (int i = 1; i < num_of_vertex; ++i)	//这里循环从1开始是因为开始节点已经存放在S中了，还有numOfVertex-1个节点要处理
    {
        /*选择distance最小的一个节点*/
        int next_vertex = u;
        int temp_distance = INT_MAX;
        for (int j = 0; j < num_of_vertex; ++j)
        {
            if ((is_inS[j] == false) && (distance[j] < temp_distance))
            {
                next_vertex = j;
                temp_distance = distance[j];
            }
        }
        is_inS[next_vertex] = true;	//放入S集合
        u = next_vertex;	//下次寻找的开始节点
        
        /*更新distance*/
        for (int j = 0; j < num_of_vertex; ++j)
        {
            if (is_inS[j] == false && G[u][j] < INT_MAX)
            {
                int temp = distance[u] + G[u][j];
                if (temp < distance[j])
                {
                    distance[j] = temp;
                    pre_vertex[j] = u;
                }
            }
        }
    }
}
/*Dijkstra调用示例*/
/*
 using namespace std;
 const int N = 7;
 int G[][N] = {
	{false, true, false, false, false, false, false},
	{true, false, true, true, true, true, false},
	{false, true, false, false, true, true, true},
	{false, true, false, false, false, false, false},
	{false, true, true, false, false,false, false},
	{false, true, true, false, false, false, false},
	{false, false, true, false, false, false, false}
 };
 
 int main(int argc, const char* argv[])
 {
 //	srand((unsigned)time(NULL));
	int distance[N];
	int pre_vertex[N];
	//将bool型图矩阵转换为int型
	for (int i = 0; i < N; ++i)
        for(int j = 0; j < N; ++j)
        {
            if (i != j)
            {
                if (G[i][j] == false)
                {
                    G[i][j] = INT_MAX;
                }
                else
                    G[i][j] = 1;
            }
            else
                G[i][j] = 0;
    }
 
	//计算图G中所有最短路径
	for (int i = 0; i < N; ++i)
	{
        Dijkstra(N, i, G, distance, pre_vertex);
        for (int j = 0; j < N; ++j)
        {
            int index = j;
            stack<int> trace;
            while (pre_vertex[index] != -1)	//直到源节点
            {
                trace.push(pre_vertex[index]);
                index = pre_vertex[index];
            }

            cout << "路径： ";
            while (!trace.empty())
            {
                cout << trace.top() << " -- ";
                trace.pop();
            }
            cout << j;
            cout << " 距离是： " << distance[j] << endl;
        }
	}
 
	return 0;
 }
 */