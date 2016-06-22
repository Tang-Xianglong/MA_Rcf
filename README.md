# MA_Rcf
Project Title: 
a memetic algorithm for enhancing complex networks against cascading failures.

This project is developed by C++.


Source files list:
main.cpp:
    程序主入口程序。
    定义种群，读取网络

Dijkstra(.hpp/.cpp):
    Dijkstra算法的声明。
    Dijkstra算法是求一个节点到其他所有节点的最短路径的有效算法。
    算法复杂度是O(n^2).

gbdefine(.h):
    算法各种常量和参数的定义。

net(.hpp/.cpp):
    网络及其相关类的声明。

sequeue(.hpp/.cpp):
    自定义的sequeue类，实现常用的队列操作！

experiments(.hpp/.cpp):
    项目试验的源代码。包括数据格式的转换，优化过程的处理。