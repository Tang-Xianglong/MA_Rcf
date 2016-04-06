//
//  net.hpp
//  MA_Rcf
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#ifndef net_hpp
#define net_hpp

#include "gbdefine.h"
#include "sequeue.hpp"
#include "Dijkstra.hpp"
#include <ctime>
#include <stack>
#include <algorithm>
using namespace std;
extern const int node_num;

//定义网络数据结构
typedef struct nodetype
{
    vector <int> nodeidx;						//链式存储网络
    bool nodeblock[NODE_NUM];					//矩阵存储网络,
    bool exist;									//节点是否在网路中属性
    int degcnt;									//节点度属性
    double nload;								//节点当前负载,即网络所有平均最短路径经过该点的条数
    double ncapacity;							//节点能力
}NodeType;
//定义边的结构
typedef struct edgetype
{
    int nodex;									//边的x节点
    int nodey;									//边的y节点
}EdgeType;
// 定义网络类
class NetClass
{
public:
    NetClass();
    ~NetClass();
    vector<NodeType> net;
    double robustness;
    double airfa;
    
    void BuildNet();							//建立核型为3，每次添加的边为2的网络
    void WriteNet(FILE * fp,int);				//将网络写入文件
    void WriteRobust(FILE *,int);				//将网络的鲁棒性写入文件
    void ReadNet(FILE *);
    bool ConnecTest();							//检测网络的联通性
    double CalcRobust();						//计算网络鲁棒性
    void chge(int ,int ,int ,int);				//交换两条边
    void update_load(const int nd_ct);			//更新网络节点的Load
    void initialize_capacity();					//初始化网络节点的Capacity
    void DeleteNet();							//仅删除网络
private:
    int ndct;									//网络的节点数
    int eapointcnt;								//每增加一个节点添加的边的数量
    vector <double> GroupPoint;					//统计每次网络被攻击后给联通子分量的大小
    vector <int>    GroupMark;					//标记网络受到攻击后的每个联通子分量
    SequeueClass Q;
    int NodeChoose(int);						//根据现有的网络中节点的度的大小选择需要连接的节点
    int FdMxDeg();								//找到现有的网络中，度最大的节点
    void DeleteNode(int, int *);				//将给出的节点从现有的网络中删除
    double FdMxGpt(int);						//找到受攻击后网络的最大联通子分量
    void Bfas(int ,int , int);					//遍历网络联通子分量
    void DeptGp(int * ,int );					//将网络分成联通子分量
    int breakedge(int ,int );
    void connectedge(int ,int ,int );			
    
protected:
};

#endif /* net_hpp */
