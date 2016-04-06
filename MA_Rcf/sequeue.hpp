//
//  sequeue.hpp : 自定义的sequeue类，实现常用的队列操作！
//  MA_Rcf
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#ifndef sequeue_hpp
#define sequeue_hpp

#include <iostream>
#include <vector>
using std::cin;
using std::cout;
using std::endl;
using std::vector;

class SequeueClass
{
public:
    SequeueClass();
    SequeueClass(int );
    ~SequeueClass();
    void InitQueue(int );
    void SetNull();
    bool EmptyQueue();
    bool EnQueue(int);
    int DeQueue(int &);
    vector<int> data;
private:
    int front;
    int rear;
    int _SqSize;
    void DeleteSq();
public:
};

#endif /* sequeue_hpp */
