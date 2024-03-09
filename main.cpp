#include <bits/stdc++.h>
// 标准库
#include <utility>
#include <algorithm>
#include <vector>
// 算法类头文件
#include "include/DStarLite.hpp"
// 类与全局变量定义头文件
#include "Robot.hpp"
#include "Berth.hpp"
#include "Boat.hpp"
#include "GlobalData.hpp"
using namespace std;


int main()
{
    Init(); //初始化
    for(int zhen = 1; zhen <= 15000; zhen ++) // 按帧处理
    {
        int id = Input(); // 读取场面信息 id第几帧
        
        for(int i = 0; i < robot_num; i ++){
        
        // 机器人指令
            if(robot[i].status == 1){
                printf("move %d %d\n", i, 1);
            // printf("move %d %d\n", i, rand() % 4); //指令：move 机器人id0-9  移动方向右左上下0123
                if(robot[i].goods == 0)
                    printf("get %d\n", i); //指令：get 机器人id0-9  取货
                else
                    printf("put %d\n", i); //指令：put 机器人id0-9  放货
            }
                
            // printf("move %d %d\n", 3, 1);
            // printf("move %d %d\n", 7, 1);

            // get/pull id；货物生成处未携带物品则取货成功/泊位处且携带物品则放置成功
        //船指令
            // ship id0-4 泊位id0-9 船移动到泊位
            // go id0-4 船运输货物到虚拟点
        }
        
        puts("OK"); // 所有指令结束后OK
        fflush(stdout);
    }

    return 0;
}
