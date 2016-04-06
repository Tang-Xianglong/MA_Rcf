//
//  net.cpp
//  MA_Rcf
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#include "net.hpp"
#include <fstream>
#include <cstdio>
extern const int node_num = NODE_NUM;
const int each_edge = EACH_EDGE;
const double air_fa = AIRFA;

NetClass::NetClass()
{
    ndct=node_num;
    eapointcnt = each_edge; //每次增加的边数
    robustness = 0;
    airfa = air_fa;
    net.resize(ndct);
    for (int i=0;i<ndct;i++)
    {
        net[i].exist=false;
        net[i].degcnt=0;
        net[i].nload=0;
        net[i].ncapacity=0;
        for(int j=0;j<ndct;j++)
        {
            net[i].nodeblock[j] = 0;
        }
    }
    GroupPoint.resize(ndct);
    GroupMark.resize(ndct);
    for (int i=0;i<ndct;i++)
    {
        GroupPoint[i]=0;
        GroupMark[i]=0;
    }
    Q.InitQueue(ndct);
}

NetClass::~NetClass()
{
    GroupMark.clear();
    GroupPoint.clear();
    net.clear();
}

void NetClass::BuildNet()
{
    //初始化三个节点的全联通网络
    airfa = air_fa;
    int pcount=3;
    for (int i=0 ; i < pcount; i++)
    {
        for (int j=i+1;j<pcount;j++)
        {
            if(i != j)
            {
                net[i].nodeidx.push_back(j);
                net[j].nodeidx.push_back(i);
                
                net[i].nodeblock[j] = 1;
                net[j].nodeblock[i] = 1;
            }
        }
        //设置网络中节点的属性
        net[i].exist=1;
        net[i].degcnt=pcount-1;
    }
    //向初始的全联通网络中添加节点
    vector <int> choose;
    choose.clear();
    for (int i=pcount;i<ndct;i++)
    {
        choose.resize(node_num);
        for (int j=0;j<eapointcnt;j++)
        {
            int midx = NodeChoose(i);
            //防止重复连接
            if (choose[midx]==1)
            {
                j--;
                continue;
            }
            else
                choose[midx]=1;
            //连接选择好的节点
            net[i].nodeidx.push_back(midx);
            net[midx].nodeidx.push_back(i);
            
            net[i].nodeblock[midx] = 1;
            net[midx].nodeblock[i] = 1;
            
            //更新网络的度
            net[i].degcnt++;
            net[midx].degcnt++;
        }
        //设置新加入的节点的属性
        net[i].exist=1;
        choose.clear();
    }
    
    //初始化并更新节点Load和Capacity.
    initialize_capacity();
}

int NetClass::NodeChoose( int inode)
{
    //初始化整个网络的度为零
    int sumdegree = 0;
    //计算整个网络的度
    for (int i=0;i<inode;i++)
    {
        sumdegree = sumdegree + net[i].degcnt;
    }
    //初始化并累加节点的度形成轮盘赌
    vector <double> ip(inode);
    for (int i=0;i<inode;i++)
    {
        ip[i]=0;
        if(i==0)
            ip[i] = net[i].degcnt;
        else
            ip[i] = ip[i-1] + net[i].degcnt;
    }
    //对轮盘赌进行归一话处理
    for (int i=0;i<inode;i++)
    {
        ip[i] = ip[i]/(double)sumdegree;
    }
    //产生随机数，并根据随机数在轮盘上对应的节点
    int flag = 0;
    double ChooseP =0;
    ChooseP = (double)(rand()%1000)/1000;
    for (int i=0;i<inode;i++)
    {
        if (ChooseP<=ip[i])
        {
            flag=i;
            break;
        }
    }
    return flag;
}

void NetClass::ReadNet( FILE * fp)
{
    int midint =0;
    int nodecnt =0;
    if (!feof(fp))
    {
        if (!feof(fp))
            fscanf(fp,"%d",&nodecnt);
        if (!feof(fp))
            fscanf(fp,"%d%lf",&midint,&robustness);
        for(int i=0;i<nodecnt;i++)
        {
            if(!feof(fp))
            {
                fscanf(fp,"%d",&midint);
                fscanf(fp,"%d",&(net[i].degcnt));
                for(int j = 0;j<net[i].degcnt;j++)
                {
                    fscanf(fp,"%d",&midint);
                    net[i].nodeidx.push_back(midint);
                    net[i].nodeblock[midint] = 1;
                }
                net[i].exist = true;
            }
        }
    }
}

void NetClass::WriteNet( FILE * fp,int icount)
{
    fprintf(fp,"%-8d \n",ndct);
    fprintf(fp,"%-8d %-10f\n",icount,robustness);
    for (int i=0;i<ndct;i++)
    {
        int ncount=(int)net[i].nodeidx.size();
        fprintf(fp,"%-5d %-5d ",i,ncount);
        for (int j=0;j<ncount;j++)
        {
            fprintf(fp,"%-5d ",net[i].nodeidx[j]);
        }
        fprintf(fp,"\n");
    }
}

void NetClass::WriteRobust( FILE *fp,int icount)
{
    fprintf(fp,"%-10d %-10f\n",icount,robustness);
}

void NetClass::DeleteNet()
{
    for (int i=0;i<ndct;i++)
    {
        net[i].nodeidx.clear();
        net[i].exist=false;
        net[i].degcnt=0;
        GroupPoint[i]=0;
        GroupMark[i]=0;
        for(int j=0;j<node_num;j++)
        {
            net[i].nodeblock[j] = 0;
        }
    }
    robustness=0;
    Q.SetNull();
}

double NetClass::CalcRobust()
{
    //初始化变量
    int flag =0;int mark=1;
    double sumR=ndct;    //攻击很多节点
    double maxpoint = 0;
    for (int i=0;i<ndct;i++)
    {
        GroupMark[i]=1;
        GroupPoint[i]=0;
    }
    //更新节点Load和Capacity.
    initialize_capacity();
    //开始攻击
    do
    {
        flag=FdMxDeg();						//找到最大负载的节点
        DeleteNode(flag, &mark);			//删除节点,级联攻击下更新节点Load
        for(int i=0;i<node_num;i++)			//统计联通子分量的大小
            GroupPoint[GroupMark[i]-1]++;
        maxpoint=FdMxGpt(mark);				//找到最大联通子分量并得到大小
        
        for(int i=0;i<mark;i++)				//将统计结果重置为零
            GroupPoint[i]=0;
        sumR += maxpoint;					//将最大联通子分量的大小加和
        
    } while (mark<ndct);
    
    //统计剩余由独立节点组成的网络的大小,并复原网络的邻接表
    int ispoint = 0;
    for (int i=0;i<ndct;i++)
    {
        if (net[i].exist)
            ispoint++;
        else
            net[i].exist=1;
        //复原网络的邻接表
        net[i].degcnt = (int)net[i].nodeidx.size();
        const int ncount = (int)net[i].nodeidx.size();
        for (int j = 0; j < ncount; ++j)
        {
            const int idn = net[i].nodeidx[j];
            net[i].nodeblock[idn] = 1;
            net[idn].nodeblock[i] = 1;
        }
    }
    //将剩余节点加入，并计算网络鲁棒性
    sumR += ispoint-1;
    robustness = sumR/(ndct*(ndct+1)*1.0);//攻击很多节点
    initialize_capacity();

    return robustness;
}

bool NetClass::ConnecTest()
{
    //初始化变量，随机的从网络中的一个节点开始遍历网络
    int i = 0,ncount = 0,idx = 0;
    int travelnode=0;
    int beginode=rand()%ndct;
    Q.SetNull();
    Q.EnQueue(beginode);
    net[beginode].exist=false;
    travelnode++;
    while (Q.EmptyQueue())
    {
        Q.DeQueue(i);
        ncount=(int)net[i].nodeidx.size();
        for (int j=0;j<ncount;j++)
        {
            idx=net[i].nodeidx[j];
            if (net[idx].exist)
            {
                net[idx].exist=false;
                Q.EnQueue(idx);
                travelnode++;
            }
        }
    }
    //将节点的属性恢复
    for (int j=0;j<ndct;j++)
    {
        net[j].exist=true;
    }
    Q.SetNull();
    //查看是否所有的节点被遍历
    //并以此判断网络是否联通
    if (travelnode!=ndct)
        return false;
    else
        return true;
}

int NetClass::FdMxDeg()
{
    //从第节点0开始找第一个存在于网络中的节点
    int flag=0;
    double MaxLoad = 0.0;
    while (flag<ndct&&net[flag].exist!=1)
    {
        flag++;
    }
    //从第一个存在于网络中的节点开始寻找
    MaxLoad=net[flag].nload;
    for (int i=flag+1;i<ndct;i++)
    {
        if (net[i].exist)
        {
            if (MaxLoad<net[i].nload)
            {
                MaxLoad=net[i].nload;
                flag=i;
            }
        }
    }
    //得到网络中度最大的节点@ flag
    return flag;
}

void NetClass::DeleteNode( int flag, int * mark)
{
    unsigned const int ncount=(int)net[flag].nodeidx.size();
    for (unsigned int i=0; i<ncount; i++)
    {
        int idx = net[flag].nodeidx[i];
        if (net[idx].exist)
        {
            net[idx].degcnt--;
        }
        //更新网络邻接表
        net[flag].nodeblock[idx] = 0;
        net[idx].nodeblock[flag] = 0;
    }
    //设置该节点的属性为不存在于该网络中
    net[flag].exist=false;
    net[flag].degcnt=0;
    
    //将该节点所属的联通子分量从新划分
    DeptGp(mark,flag);
    
    //更新网络节点Load;
    update_load(ndct);
    
    //将过载的节点都依次删除
    for (unsigned int i=0; i<ncount; ++i)
    {
        int i_dx = net[flag].nodeidx[i];
        if (net[i_dx].exist)
        {
            if (net[i_dx].ncapacity < net[i_dx].nload)
                DeleteNode(i_dx, mark);		//用到了递归
        }
    }
}

double NetClass::FdMxGpt( int mark)
{
    //从0开始到现有的最大联通子分量结束
    int maxpoint=0;
    for (int i=0;i<mark;i++)
    {
        if (GroupPoint[maxpoint]<GroupPoint[i])
        {
            maxpoint=i;
        }
    }
    return GroupPoint[maxpoint];
}

void NetClass::Bfas( int mediaflag,int mark, int flag)
{
    int i =0,ncount = 0,idx = 0;
    Q.SetNull();
    //将mediaflag标记为mark表示已经访问
    GroupMark[mediaflag]=mark;
    Q.EnQueue(mediaflag);
    //开始遍历存在于网络中，并且与节点@flag具有相同分量标签的子分量
    while (Q.EmptyQueue())
    {
        Q.DeQueue(i);
        if (net[i].exist)
        {
            ncount=(int)net[i].nodeidx.size();
            for (int j=0;j<ncount;j++)
            {
                idx=net[i].nodeidx[j];
                if (net[idx].exist&&GroupMark[idx]==GroupMark[flag])
                {
                    GroupMark[idx]=mark;
                    Q.EnQueue(idx);
                }
            }
        }
    }
}

void NetClass::DeptGp( int * mark,int flag)
{
    int ncount = 0,mediaflag = 0;
    ncount=(int)net[flag].nodeidx.size();
    for (int k=0;k<ncount;k++)
    {
        mediaflag=net[flag].nodeidx[k];
        if(GroupMark[mediaflag]==GroupMark[flag])
        {
            (*mark)++;
            Bfas(mediaflag,(*mark),flag);
        }
    }
}

int NetClass::breakedge( int edgex,int edgey)
{
    int pos=-1;
    for(int i=0;i<(int)net[edgex].nodeidx.size();i++)
    {
        if(net[edgex].nodeidx[i] == edgey)
        {
            pos = i;
            break;
        }
    }
    net[edgex].nodeblock[edgey] = 0;
    return pos;
}

void NetClass::connectedge( int edgex,int edgey,int pos)
{
    net[edgex].nodeblock[edgey] = 1;
    net[edgex].nodeidx[pos] = edgey;
}

void NetClass::chge( int x1,int y1,int x2,int y2)
{
    connectedge(x1,x2,breakedge(x1,y1));
    connectedge(y1,y2,breakedge(y1,x1));
    connectedge(x2,x1,breakedge(x2,y2));
    connectedge(y2,y1,breakedge(y2,x2));
}

void NetClass::update_load(const int nd_ct)
{
    const int N = nd_ct;
    /*清空节点初始Load值*/
    for (int i = 0; i < N; ++i)
    {
        net[i].nload = 0;
    }
    
    //定义二维数组，并申请内存
    int **G;
    G = new int* [N];
    for (int i = 0; i < N; ++i)
    {
        G[i] = new int [N];
    }
    
    //将网络中的邻接矩阵信息拷贝到G中
    for (int i = 0; i < N; ++i)
    {
        for (int j = i+1; j < N; ++j)
        {
            if (net[i].nodeblock[j] == 0)
            {
                G[i][j] = INT_MAX;
                G[j][i] = INT_MAX;
            }
            else
            {
                G[i][j] = 1;
                G[j][i] = 1;
            }
        }
        G[i][i] = 0;
    }
    int* distance = new int [N];	//最短路径长度
    int* pre_vertex = new int [N];	//最短路径节点序列直到源节点
    
    /*计算图G中所有最短路径*/
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
            while (!trace.empty())
            {
                net[trace.top()].nload++;	//最短路径经过的节点的Load累加
                trace.pop();
            }
        }		
    }
    
    for (int i = 0; i < N; ++i)
    {
        net[i].nload += N;	//节点Load累加所有以此节点为源节点的路径数			
    }
    
    //释放二维数组内存地址
    delete [] distance;
    distance = NULL;
    delete [] pre_vertex;
    pre_vertex = NULL;
    for (int i = 0; i < N; ++i)
    {
        delete [] G[i];
        G[i] = NULL;
    }
    delete [] G;
    G = NULL;
}

void NetClass::initialize_capacity()
{
    const int N = ndct;
    update_load(N);	
    const double air = airfa;
    for (int i = 0; i < N; ++i)
    {
        net[i].ncapacity = (1+air) * (net[i].nload);	//更新网络中节点的capacity
    }
}