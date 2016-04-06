//
//  memetic.hpp
//  MA_Rcf
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#ifndef memetic_hpp
#define memetic_hpp

#include "net.hpp"
extern const int node_num;
extern const int pop_size;

struct _comType
{
    int nodeidx;
    double robust;
};

/*
	根据原有的一个网络得到种群
 */
void varietypop(NetClass & );
//
void comDegConect(NetClass &);
//
void localSearch(NetClass &,int &,FILE *,FILE *,vector<NetClass> &);

/*
	两个网络的交叉操作
 */
void crosscase(NetClass &,NetClass &,NetClass &,NetClass &);

/*
	对整个种群进行交叉操作
 */
void crossover(vector<NetClass> &);

/*
	选择操作
 */
void seclectnet(vector<NetClass> &);

void searchCase(int ,NetClass & ,int &,FILE*,FILE*,vector<NetClass> &);



#endif /* memetic_hpp */
