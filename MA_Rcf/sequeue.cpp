//
//  sequeue.cpp
//  MA_Rcf
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#include "sequeue.hpp"

SequeueClass::SequeueClass( int SqSize )
{
    InitQueue(SqSize);
}

SequeueClass::SequeueClass()
{
    _SqSize = 0;
    InitQueue(_SqSize);
}

SequeueClass::~SequeueClass()
{
    DeleteSq();
}

void SequeueClass::InitQueue( int SqSize)
{
    _SqSize = SqSize;
    data.resize(_SqSize);
    front=rear=0;
}

void SequeueClass::SetNull()
{
    front=rear=0;
}

bool SequeueClass::EmptyQueue()
{
    int x;
    x=(rear-front+_SqSize)%_SqSize;
    if(x!=0)
        return true;
    else
        return false;
}

bool SequeueClass::EnQueue( int datax)
{
    if (front==(rear+1)%_SqSize)
    {
        cout<<"队列上溢"<<endl;
        return false;
    }
    else
    {
        rear=(rear+1)%_SqSize;
        data[rear]=datax;
        return true;
    }
}

int SequeueClass::DeQueue( int & datax)
{
    if (rear==front)
    {
        cout<<"队列下溢"<<endl;
        return false;
    }
    else
    {
        front=(front+1)%_SqSize;
        datax=data[front];
        return true;
    }
}

void SequeueClass::DeleteSq()
{
    SetNull();
    data.clear();
    data.resize(_SqSize);
}