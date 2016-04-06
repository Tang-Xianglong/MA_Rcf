//
//  main.cpp
//  MA_Rcf
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#include "memetic.hpp"
#include "experiments.hpp"

int main(void)
{
    //种群变量
    vector<NetClass> netdata(2*pop_size);

    //随机种
    unsigned int recordRand = (unsigned)time(0);
    srand(recordRand);
    
    //打开数据文件
    FILE *datafp = fopen("/Users/tangxianglong/Documents/Xcode/MA_Rcf/MA_Rcf/netdata_100.txt","r");
    if(datafp==NULL)
    {
        cout<<"Can not open data file!"<<endl;;
        return -1;
    }
    
    //开始优化
    for(int optimes = 0;optimes<FILECNT;optimes++)
    {
        optexe(netdata, optimes+1,datafp);
    }
    
    //关闭流，结束！
    fclose(datafp);
    return 0;
}