//
//  experiments.cpp
//  MA_Rcf
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#include "experiments.hpp"

void Generate_net()
{
    NetClass netdata;
    /*generate the network*/
    netdata.BuildNet();
        if (netdata.ConnecTest())
        {
            netdata.robustness = netdata.CalcRobust();
            cout << "R: " << netdata.robustness << endl;
        }

    FILE *Initnet;
    Initnet = fopen("netdata_100.txt","w");
    netdata.WriteNet(Initnet,0);
    fclose(Initnet);
}

void To_pjk_form(NetClass& Net)
{
    /*change the netdata to pajek form*/
    FILE *datafp_net = fopen("USAir_after_optimize.txt","r");
    Net.ReadNet(datafp_net);
    
    ofstream out_du("net_USAir_du.txt");
    for (int i = 0; i < (int)NODE_NUM; ++i)
    {
        out_du << Net.net[i].degcnt<<endl;
    }
    out_du.close();
    
    ofstream out_net("net_USAir_optimized.txt");
    out_net << "*Vertices 332" << endl;
    for (int i=0; i < 332; ++i)
    {
        out_net << i+1 <<endl;
    }
    
    out_net << "*Edges" << endl;
    for (int i = 0; i < (int)NODE_NUM; ++i)
        for (int j = 0; j < (int) Net.net[i].nodeidx.size(); ++j)
        {
            if (Net.net[i].nodeidx[j] > i)
            {
                out_net << i+1 <<" "<< Net.net[i].nodeidx[j]+1 <<endl;
            }
        }
    
    out_net.close();
    fclose(datafp_net);
}


void optexe(vector<NetClass> &netdata, int timecount,FILE * datafp)
{
    /*确定文件的名字*/
    char P_Count[50];
    char R_Count[50];
    char NameForNet[100];
    char NameForRobut[100];
    NameForNet[0]='\0';
    NameForRobut[0]='\0';
    ConvertIntToChar((int)netdata[0].net.size(),P_Count);
    ConvertIntToChar(timecount,R_Count);
    strcat(P_Count," nodes,");     //strcat_s  是将两个字符串接在一起
    strcat(R_Count," results,");
    strcat(P_Count,R_Count);
    strcat(NameForNet,P_Count);
    strcat(NameForRobut,P_Count);
    strcat(NameForNet,"nets structure.txt");
    strcat(NameForRobut,"robustness record.txt");
    
    FILE *fp_robust;
    FILE *fp_net;
    //打开文件
    fp_robust=fopen(NameForRobut,"w");
    fp_net=fopen(NameForNet,"w");
    //初始化网络及其鲁棒性
    netdata[0].ReadNet(datafp);
    netdata[0].initialize_capacity();
    for(int i=0;i<pop_size;i++)
    {
        netdata[i].net = netdata[0].net;
        for(int j=0;j<node_num;j++)
        {
            varietypop(netdata[i]);
        }
        //		netdata[i].initialize_capacity();
        netdata[i].CalcRobust();
        cout << "R of Net " << i << " is: " << netdata[i].robustness <<endl;
    }
    //begin generation
    int loopcnt = (int)GENERATION;
    int valuecnt=0;
    for(valuecnt=0;valuecnt<loopcnt;)
    {
        cout << "******************" << endl;
        cout << "the " << valuecnt << " generation: " << endl;
        cout << "******************" << endl;
        //crossover operator
        crossover(netdata);
        //local search operator
        for(int i=0;i<2*pop_size;i++)
        {
            int flag = LocalOptFlag(netdata);
            cout<<"local search on net: "<<flag<<endl;
            localSearch(netdata[flag],valuecnt,fp_net,fp_robust,netdata);
        }
        //select operator
        seclectnet(netdata);
    }
    //删除优化后的网络以便程序重新开始
    for(int i=0;i<2*pop_size;i++)
    {
        netdata[i].DeleteNet();
    }
    //关闭打开的文件
    fclose(fp_net);
    fclose(fp_robust);
}

int LocalOptFlag(vector<NetClass> &netdata)
{
    double robust[2*POP_SIZE];
    double sumRobust = 0;
    double routBet[2*POP_SIZE];
    for (int i=0;i<2*pop_size;i++)
    {
        robust[i] = netdata[i].robustness;
        sumRobust += robust[i];
    }
    double sluck = rand()%1000/1000.0;
    routBet[0] = robust[0]/sumRobust;
    for (int i=1;i<2*pop_size;i++)
    {
        routBet[i] = routBet[i-1]+robust[i]/sumRobust;
    }
    int flag;
    for (int i=0;i<2*pop_size;i++)
    {
        if (sluck<routBet[i])
        {
            flag =  i;
            break;
        }
    }
    return flag;
}

void ConvertIntToChar(int t_count,char *name)
{
    int num=t_count;
    int icount=0;
    while (num!=0)
    {
        if (icount<100)
        {
            name[icount]=(char)(num%10+'0');
            num=num/10;
            icount++;
        }
    }
    name[icount]='\0';
    char nc;
    num=icount-1;
    for (int i=0;i<icount/2;i++,num--)
    {
        nc=name[i];
        name[i]=name[num];
        name[num]=nc;
    }
}