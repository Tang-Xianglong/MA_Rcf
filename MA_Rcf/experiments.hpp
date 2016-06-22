//
//  experiments.hpp
//  MA_Rcf
//
//  项目试验的源代码。包括数据格式的转换，优化过程的处理。
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#ifndef experiments_hpp
#define experiments_hpp

#include "memetic.hpp"
#include <fstream>
#include <cstdio>

//生成BA模型网络
void Generate_net();

//以此独立实验
void optexe(vector<NetClass> &, int ,FILE*);

//将数字转化为字符串
void ConvertIntToChar(int ,char *);
int LocalOptFlag(vector<NetClass> &);

//将数据转换为pajek格式
void To_pjk_form(NetClass& net);


#endif /* experiments_hpp */
