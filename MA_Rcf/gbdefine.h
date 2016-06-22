//
//  gbdefine.h
//  MA_Rcf
//
//  算法各种常量和参数的定义。
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#ifndef gbdefine_h
#define gbdefine_h

//宏定义
#define NODE_NUM 100			//网络节点数
#define EACH_EDGE 2				//构建BA网络时，每次增加的边数

#define POP_SIZE 4				//种群的大小
#define GENERATION 50000		//算法循环代数


#define FILECNT 1				//需要独立重复的试验次数
#define SCANT 200				//每个隔多少次在屏幕上输出一次结果

#define BEITA 0.8               //局部搜索时，控制节点度相关性的因子
#define AIRFA 0.5 				//airfa的值

double RandFloat();


#endif /* gbdefine_h */
