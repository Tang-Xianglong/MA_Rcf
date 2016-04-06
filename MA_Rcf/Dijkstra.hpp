//
//  Dijkstra.hpp
//  MA_Rcf
//
//  Created by 唐向龙 on 16/4/1.
//  Copyright © 2016年 唐向龙. All rights reserved.
//

#ifndef Dijkstra_hpp
#define Dijkstra_hpp

/*算法能够输出单个源点start_vertex到其他所有点的最短路径pre_vertex和对应的长度distance*/
void Dijkstra(
              const int num_of_vertex,	//节点数目
              const int start_vertex,		//源节点
              int** G,					//有向图邻接矩阵
              int *distance,				//各个节点到达源节点的距离
              int *pre_vertex				//各个节点的前一个节点
);

#endif /* Dijkstra_hpp */
