//
//  memetic.cpp : MA_Rcf算法的核心实现部分。框架是全局搜索＋局部搜索。
//  MA_Rcf
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#include "memetic.hpp"
extern const int scant = SCANT;
extern const double beita = BEITA;
extern const int pop_size = POP_SIZE;
extern const int loop_count = GENERATION;
extern const int nd_num = NODE_NUM;

void varietypop( NetClass & netdt)
{
    //初始化，并随机的以某一节点开始进行衍生
    vector <int> cntidx;
    vector <int> uctidx;
    int ndct = node_num;
    int nodeidx = rand()%ndct;
    //搜集与节点nodeidx相连与不相连的节点
    for(int i=0;i<ndct;i++)
    {
        if(i!=nodeidx)
        {
            if(netdt.net[i].nodeblock[nodeidx])
                cntidx.push_back(i);
            else
                uctidx.push_back(i);
        }
    }
    //搜集与节点nodeidx不相连的边
    vector<EdgeType> edgeidx;
    EdgeType midvx;
    for(unsigned int i=0;i<uctidx.size();i++)
    {
        int idx = uctidx[i];
        midvx.nodex = idx;
        for(int j=0;j<netdt.net[idx].degcnt;j++)
        {
            midvx.nodey = netdt.net[idx].nodeidx[j];
            edgeidx.push_back(midvx);
        }
    }
    //进行交换，得到不同的网络
    for(unsigned int i=0;i<cntidx.size();i++)
    {
        for(unsigned int j=0;j<edgeidx.size();j++)
        {
            if(cntidx[i]!=edgeidx[j].nodey&&
               netdt.net[cntidx[i]].nodeblock[edgeidx[j].nodey]!=1&&
               netdt.net[nodeidx].nodeblock[edgeidx[j].nodex]!=1&&
               netdt.net[edgeidx[j].nodex].nodeblock[edgeidx[j].nodey]==1&&
               netdt.net[nodeidx].nodeblock[cntidx[i]]==1)
            {
                netdt.chge(nodeidx,cntidx[i],edgeidx[j].nodex,edgeidx[j].nodey);
                //若交换后没有影响网络的联通性，不考虑网络鲁棒性的变化，接受
                if(!netdt.ConnecTest())
                    netdt.chge(nodeidx,edgeidx[j].nodex,cntidx[i],edgeidx[j].nodey);
                else
                    break;
            }
        }
    }
}

void crossover( vector<NetClass> & netdata)
{
    //以索引的方式随机打乱种群顺序
    vector <int> idx(pop_size);
    for(int i=0;i<pop_size;i++)					//给索引初始化
        idx[i] = i;
    for(int i=0;i<pop_size;i++)					//随机的交换，打乱顺序
    {
        int xp = rand()%pop_size;
        int x = idx[i];
        idx[i] = idx[xp];
        idx[xp] = x;
    }
    //对种群中多有的个体进行交叉操作
    for(int i=0;i<pop_size;i += 2)
    {
        //两两一组交叉
        crosscase(netdata[idx[i]],netdata[idx[i+1]],netdata[pop_size+i],netdata[pop_size+i+1]);
        if (netdata[pop_size+i].robustness==0||netdata[pop_size+i+1].robustness==0)
        {
            i-=2;
        }
    }
}

void crosscase( NetClass &net1,NetClass &net2,NetClass &net3,NetClass &net4)
{
    int nodecnt = node_num;									//获取网络节点数目
    int crosspoint = rand()%nodecnt;							//随机获取交叉点
    vector <EdgeType> net3edge;
    vector <EdgeType> net4edge;
    net3edge.clear(); net4edge.clear();
    EdgeType midedge;
    //将网络@net1 和 @net2 分别复制给 @net3 和 @net4
    for (int i=0;i<nodecnt;i++)
    {
        for(int j=i;j<nodecnt;j++)
        {
            net3.net[i].nodeblock[j] = net1.net[i].nodeblock[j];
            net3.net[j].nodeblock[i] = net1.net[j].nodeblock[i];
            net4.net[i].nodeblock[j] = net2.net[i].nodeblock[j];
            net4.net[j].nodeblock[i] = net2.net[j].nodeblock[i];
        }
        net3.net[i].nodeidx =  net1.net[i].nodeidx;
        net3.net[i].exist = true;
        net3.net[i].degcnt = net1.net[i].degcnt;
        
        net4.net[i].nodeidx =  net2.net[i].nodeidx;
        net4.net[i].exist = true;
        net4.net[i].degcnt = net2.net[i].degcnt;
    }
    //逐点进行交叉
    int st=0;
    int ed=0;
    
//    cout<<"crosspoint: "<<crosspoint<<endl;		//debug
    
    if(crosspoint>node_num/2)
    {
        st = crosspoint;
        ed = node_num;
    }
    else
    {
        st = 0;
        ed = crosspoint;
    }
    for(int i=st;i<ed;i++)
    {
        for(int j=0;j<nodecnt;j++)
        {
            //搜集网络@net3与@net4彼此特有的边
            if(!(net3.net[i].nodeblock[j]&&net4.net[i].nodeblock[j]))
            {
                if(net3.net[i].nodeblock[j])
                {
                    midedge.nodex = i;
                    midedge.nodey = j;
                    net3edge.push_back(midedge);
                }
                if(net4.net[i].nodeblock[j])
                {
                    midedge.nodex = i;
                    midedge.nodey = j;
                    net4edge.push_back(midedge);
                }
            }
        }
        //以一个网络为基准开始进行交叉
        //注：net3edge.size()=net4edge.size()
        
        int edgecnt = (int)net3edge.size();
        
 //       cout<<"based on node "<<i<<" exchange edge num: "<<edgecnt<<endl;   //debug
        
        for(int i=0;i<edgecnt;i++)
        {
            for(int j=0;j<edgecnt;j++)
            {
                //只有现有的按顺序得到的边还存在才进行考虑
                if(net4.net[net4edge[j].nodex].nodeblock[net4edge[j].nodey] == 1)
                {
                    vector <int> net3nodeyidx;
                    vector <int> net4nodeyidx;
                    int net3nodex = net3edge[i].nodex;
                    int net3nodey = net3edge[i].nodey;
                    int net4nodex = net4edge[j].nodex;
                    int net4nodey = net4edge[j].nodey;
                    //搜集net3中与节点net4nodey具有相同标签的节点相连的节点
                    for(int k=0;k<(int)net3.net[net4nodey].nodeidx.size();k++)
                    {
                        int kidx = net3.net[net4nodey].nodeidx[k];
                        if(kidx!=net3nodey&&net3.net[net3nodey].nodeblock[kidx]!=1)
                            net3nodeyidx.push_back(kidx);
                    }
                    //搜集net4中与节点net3nodey具有相同标签的节点相连的节点
                    for(int k=0;k<(int)net4.net[net3nodey].nodeidx.size();k++)
                    {
                        int kidx = net4.net[net3nodey].nodeidx[k];
                        if(kidx!=net4nodey&&net4.net[net4nodey].nodeblock[kidx]!=1)
                            net4nodeyidx.push_back(kidx);
                    }
                    //如果所搜集的节点数都不为零，进行交叉并调整
                    if(net3nodeyidx.size()&&net4nodeyidx.size())
                    {
                        int net3rand = rand()%(int)net3nodeyidx.size();
                        int net4rand = rand()%(int)net4nodeyidx.size();
                        net3.chge(net3nodex,net3nodey,net4nodey,net3nodeyidx[net3rand]);
                        net4.chge(net4nodex,net4nodey,net3nodey,net4nodeyidx[net4rand]);
                        break;
                    }
                }
            }
        }
        net3edge.clear();
        net4edge.clear();
    }
    if(net3.ConnecTest())
    {
        net3.CalcRobust();
        cout<<"net3.robust: "<<net3.robustness<<endl;
    }
    else
        net3.robustness = 0;
    if(net4.ConnecTest())
    {
        net4.CalcRobust();
        cout<<"net4.robust: "<<net4.robustness<<endl;
    }
    else
        net4.robustness = 0;
}

void seclectnet( vector<NetClass> & netdata)
{
    //初始化
    _comType player[2*pop_size];
    int chidx[2*pop_size];
    for (int i=0;i<2*pop_size;i++)
    {
        player[i].nodeidx = i;
        player[i].robust = netdata[i].robustness;
        chidx[i] = 0;
    }
    for (int i=0;i<pop_size;i++)
    {
        int selt = rand()%(2*pop_size);
        _comType midp = player[i];
        player[i] = player[selt];
        player[selt] = midp;
    }
    //选取得分前pop_size的个体为下一父代
    for(int i=0;i<2*pop_size;i+=2)
    {
        if(player[i].robust>player[i+1].robust)
            chidx[player[i].nodeidx] = 1;
        else
            chidx[player[i+1].nodeidx] =1;
    }
    //对新得到的种群进行整理
    for(int i=pop_size,k=0;i<2*pop_size&&k<2*pop_size;)
    {
        while(i<2*pop_size&&chidx[i]==0)
            i++;
        while(k<2*pop_size&&chidx[k]==1)
            k++;
        if(i<2*pop_size&&k<2*pop_size)
        {
            if(i>k)
            {
                for(int j=0;j<node_num;j++)
                {
                    for(int m=0;m<node_num;m++)
                        netdata[k].net[j].nodeblock[m] = netdata[i].net[j].nodeblock[m];
                    for(int m=0;m<(int)netdata[i].net[j].nodeidx.size();m++)
                        netdata[k].net[j].nodeidx[m] = netdata[i].net[j].nodeidx[m];
                }
                netdata[k].robustness = netdata[i].robustness;
                chidx[i] = 0;
                chidx[k] = 1;
            }
            else
            {
                i++;
            }
        }
    }
}

void localSearch( NetClass & net ,int &vacnt,FILE*fp1,FILE*fp2,vector<NetClass> &netdata)
{
    if(net.robustness==0)
    {
        return;
    }
    //随机选择局部搜索的节点
//    int luck = rand()%node_num;
    //节点的局部搜索
    for (int i=0;i<node_num;i++)
    {
        if (vacnt >= (int)GENERATION)
            break;
        searchCase(i,net,vacnt,fp1,fp2,netdata);
    }
}

void searchCase( int nodeidx,NetClass & net,int &valuecnt,FILE*fp_net,FILE*fp_robust,vector<NetClass> &netdata)
{
    //初始化，搜集与节点@nodeidx相连的节点和不与其相连的节点
    vector <int> cntidx;
    vector <int> uctidx;
    for(int i=0;i<node_num;i++)
    {
        if(i!=nodeidx)
        {
            if(net.net[i].nodeblock[nodeidx])
                cntidx.push_back(i);
            else
                uctidx.push_back(i);
        }
    }
//    cout<<"local search node: "<<nodeidx<<endl;	//debug
//    cout<<"cntidx.size: "<<cntidx.size()<<endl;	//debug
//    cout<<"ucntidx.size: "<<uctidx.size()<<endl;	//debug
    //搜集不与节点@nodeidx相连的边
    vector<EdgeType> edgeidx;
    EdgeType midvx;
    for(int i=0;i<(int)uctidx.size();i++)
    {
        int idx = uctidx[i];
        midvx.nodex = idx;
        
        for(int j=0;j<net.net[idx].degcnt;j++)
        {
            midvx.nodey = net.net[idx].nodeidx[j];
            edgeidx.push_back(midvx);
        }
    }
    //	cout<<"unconnected edge size: "<<edgeidx.size()<<endl;	//debug
    //	cout<<"val_j.size: "<<(edgeidx.size()/node_num)<<endl;	//debug
    //开始进行节点@nodeidx的局部搜索
    for(int i=0;i<(int)cntidx.size();i++)
    {
        for(int j=0;j<(int)(edgeidx.size()/node_num);j++)
        {
            if (valuecnt >= (int)GENERATION)
                break;
            int idxj = rand()%(int)edgeidx.size();
            //检测得到的边是否可以交换
            if(cntidx[i]!=edgeidx[idxj].nodey&&
               net.net[cntidx[i]].nodeblock[edgeidx[idxj].nodey]!=1&&
               net.net[nodeidx].nodeblock[edgeidx[idxj].nodex]!=1&&
               net.net[edgeidx[idxj].nodex].nodeblock[edgeidx[idxj].nodey]==1&&
               net.net[nodeidx].nodeblock[cntidx[i]]==1)
            {
                //初步断定是否交换得到的可以将换的边
                net.update_load(nd_num);	//更新节点的Load
                int x1deg = net.net[nodeidx].nload;
                int y1deg = net.net[cntidx[i]].nload;
                int x2deg = net.net[edgeidx[idxj].nodex].nload;
                int y2deg = net.net[edgeidx[idxj].nodey].nload;
                if(abs(x1deg-x2deg)+abs(y1deg-y2deg) <= beita*(abs(x1deg-y1deg)+abs(x2deg-y2deg)))
                {
                    int maxRbust=0;
                    
                    double Nrobustness = net.robustness;
                    net.chge(nodeidx,cntidx[i],edgeidx[idxj].nodex,edgeidx[idxj].nodey);
                    if(net.ConnecTest())
                    {	
                        net.CalcRobust();
                        //					cout<<"initial robust: "<<Nrobustness<<endl;
                        //					cout<<"changed robust: "<<net.robustness<<endl;
                        if(Nrobustness > net.robustness)
                        {
                            net.chge(nodeidx,edgeidx[idxj].nodex,cntidx[i],edgeidx[idxj].nodey);
                            net.robustness = Nrobustness;
                        }
                        if (valuecnt%20==0)
                        {
                            for (int pi=1;pi<2*pop_size;pi++)
                            {
                                if(netdata[maxRbust].robustness<netdata[pi].robustness)
                                    maxRbust=pi;
                            }
                            netdata[maxRbust].WriteRobust(fp_robust,valuecnt);
                        }
                        if(valuecnt%100==0)
                            netdata[maxRbust].WriteNet(fp_net,valuecnt);
                        if (valuecnt%SCANT==0)
                        {
                            for (int pi=1;pi<2*pop_size;pi++)
                            {
                                if(netdata[maxRbust].robustness<netdata[pi].robustness)
                                    maxRbust=pi;
                            }
                            cout<<valuecnt<<":"<<"maxrobust"<<netdata[maxRbust].robustness<<endl;
                            for(int i=0;i<2*pop_size;i++)
                                cout<<netdata[i].robustness<<" ";
                            cout<<endl;
                        }
                        valuecnt++;
                    }
                    else
                    {
                        net.chge(nodeidx,edgeidx[idxj].nodex,cntidx[i],edgeidx[idxj].nodey);
                    }
                }
            }
        }
    }
}