#include <bits/stdc++.h>
// 标准库
#include <utility>
#include <algorithm>
#include <vector>

// 算法类头文件
#include "../include/DStarLite.hpp"
// #include "include/DStarLite.hpp"
// 类与全局变量定义头文件
#include "../include/Robot.hpp"
#include "../include/Berth.hpp"
#include "../include/Boat.hpp"
#include "../include/GlobalData.hpp"
using namespace std;

int main()
{
    Init(); // 初始化

    for (int zhen = 1; zhen <= 10; zhen++) // 按帧处理
    {
        int id = Input(); // 读取场面信息 id第几帧
        // 第一帧 目标位置为机器人初始位置
        if (zhen == 1)
        {
            for (int i = 0; i < robot_num; i++)
            {
                robot[i].goal = robot[i].pos; // goal 不是有参构造赋值的，而是通过赋值实现
            }

            // 在第一帧让所有5个轮船到离虚拟点最近的前五个泊位
            for (int i = 0; i < 5; i++)
            {
                boat[i].goal = boat[i].goal_berth;     // 每艘船的目标泊位
                printf("ship %d %d\n", i, boat[i].goal);
            }
        }

        // 机器人指令
        for (int i = 0; i < zhen; i++)
        {                         // 第i个机器人的操作
                                  // for(int i = 0; i < 2; i ++){ // 第i个机器人的操作
                                  // 路径规划
            robot[i].getMap(gds); // 传入初始化的地图
            // 机器人0 5对应 0 泊位
            robot[i].planPath(robot[i].berthgoal); // 修改目标，获取路径
            // robot[i].planPath({29, 147}); // 修改目标，获取路径
            robot[i].move();         // 移动
            robot[i].updateMap(gds); // 更新地图
            // for (size_t j = 0; j < robot[i].path.size(); ++j) {
            //     cerr << robot[i].path[j].first << "," << robot[i].path[j].second << endl;
            //     // cerr << robot[i].path.back().first << "," << robot[i].path.back().second << endl;
            // }
            // cerr << robot[i].path.back().first << "," << robot[i].path.back().second << endl;

            if (robot[i].cmd != -1)
                printf("move %d %d\n", i, robot[i].cmd);
            if (robot[i].goods == 0) // 机器人未携带物品
            {
                printf("get %d\n", i); // 指令：get 机器人id0-9  取货
            }
            if (robot[i].goods == 1) // 机器人携带物品
            {
                // 机器人当前位置在其对应泊位范围内
                if (robot[i].pos.first >= berth[robot[i].berthgoal_id].x and robot[i].pos.first <= berth[robot[i].berthgoal_id].x + 3 and robot[i].pos.second >= berth[robot[i].berthgoal_id].y and robot[i].pos.second <= berth[robot[i].berthgoal_id].y + 3)
                {
                    printf("pull %d\n", i);                      // 指令：pull 机器人id0-9  放货  每个机器人在自己对应泊位放货
                    berth[robot[i].berthgoal_id].Berth_num += 1; // 泊位物品数量+1
                }
            }

            // 船指令
            //  ship id0-4 泊位id0-9 船移动到泊位
            //  go id0-4 船运输货物到虚拟点
        }

        // 轮船指令
        for (size_t i = 0; i < 5; i++)
        {
            if (boat[i].status == 0) // 船在移动中 不做任何其他事情
            {
                continue;
            }
            if (boat[i].status == 1) // 船在移动完成或者装货状态（装货状态是到达泊位，不装货状态是到达虚拟点）
            {
                // 轮船到达泊位，开始装货   每一帧一定装货
                if (boat[i].goal != -1) // 船的目标不是虚拟点，船到达泊位
                {
                    // 泊位装卸货物（判题器自动）   用于维护5个船的货物数量 一定会发生，所以放在前面
                    if (berth[boat[i].goal_berth].Berth_num <= berth[boat[i].goal_berth].loading_speed)
                    {
                        boat[i].num += berth[boat[i].goal_berth].Berth_num;
                        berth[boat[i].goal_berth].Berth_num = 0; // 泊位的物品数量到零 全部裝上船
                    }
                    else
                    {
                        boat[i].num += berth[boat[i].goal_berth].loading_speed;
                        berth[boat[i].goal_berth].Berth_num -= berth[boat[i].goal_berth].loading_speed;
                    }
                    // 船最后一次去虚拟点的时候如果没有装满物品也需要出发并且能够在最后到达虚拟点，避免浪费最后装的物品
                    if (15000 - zhen <= berth[boat[i].goal_berth].transport_time + 10)  //可修改
                    {
                        printf("go %d\n", i);
                        continue; // 船最后一次去虚拟点，且没有装满物品，不再给该船下达指令
                    }
                    // 只有在船装满货物之后，才给船下达指令
                    if (boat[i].num < boat_capacity)
                    {
                        continue;   // 若该船没有装满，则不给该船下达指令
                    }
                    //船的物品数量被程序装超载，但实际上船不会超载，满了就不会再装了
                    if (boat[i].num >= boat_capacity)
                    {
                        boat[i].num = boat_capacity; // 船装满之后，船的货物数量不超过船的容量
                        berth[boat[i].goal_berth].Berth_num = berth[boat[i].goal_berth].Berth_num + (boat[i].num - boat_capacity);
                    }
                    printf("go %d\n", i);
                    
                }
                if (boat[i].goal == -1) // 船的目标是虚拟点，船到达虚拟点
                {
                    boat[i].num = 0; // 清空船的货物数量 全部转换成价值
                    boat[i].goal = boat[i].goal_berth;
                    printf("ship %d %d\n", i, boat[i].goal);
                }
            }
        }

        puts("OK"); // 所有指令结束后OK
        fflush(stdout);
    }
    for (int zhen = 11; zhen <= 15000; zhen++) // 按帧处理
    {
        int id = Input(); // 读取场面信息 id第几帧
        if (zhen == 1)
        {
            for (int i = 0; i < robot_num; i++)
            {
                robot[i].goal = robot[i].pos;
            }
        }

        for (int i = 0; i < robot_num; i++)
        {                                          // 第i个机器人的操作
                                                   // for(int i = 0; i < 2; i ++){ // 第i个机器人的操作
                                                   // 路径规划
            robot[i].getMap(gds);                  // 传入地图
            robot[i].planPath(robot[i].berthgoal); // 修改目标，获取路径
            // robot[i].planPath({29, 147}); // 修改目标，获取路径
            robot[i].move();         // 移动
            robot[i].updateMap(gds); // 更新地图
            // for (size_t j = 0; j < robot[i].path.size(); ++j) {
            //     cerr << robot[i].path[j].first << "," << robot[i].path[j].second << endl;
            //     // cerr << robot[i].path.back().first << "," << robot[i].path.back().second << endl;
            // }
            // cerr << robot[i].path.back().first << "," << robot[i].path.back().second << endl;

            if (robot[i].cmd != -1)
                printf("move %d %d\n", i, robot[i].cmd);
            printf("get %d\n", i);  // 指令：get 机器人id0-9  取货
            printf("pull %d\n", i); // 指令：pull 机器人id0-9  放货
            // 船指令
            //  ship id0-4 泊位id0-9 船移动到泊位
            //  go id0-4 船运输货物到虚拟点
        }

        puts("OK"); // 所有指令结束后OK
        fflush(stdout);
    }

    return 0;
}
