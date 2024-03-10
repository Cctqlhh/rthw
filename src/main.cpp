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
    Init(); //初始化
    
    for(int zhen = 1; zhen <= 10; zhen ++) // 按帧处理
    {
        int id = Input(); // 读取场面信息 id第几帧
        if(zhen == 1){
            for(int i = 0; i < robot_num; i ++){
                robot[i].goal = robot[i].pos;
            }
        }


        for(int i = 0; i < zhen; i ++){ // 第i个机器人的操作
        // for(int i = 0; i < 2; i ++){ // 第i个机器人的操作
        // 路径规划 
            robot[i].getMap(gds); // 传入地图
            robot[i].planPath({berth[i].x+2, berth[i].y+2}); // 修改目标，获取路径
            // robot[i].planPath({29, 147}); // 修改目标，获取路径
            robot[i].move(); // 移动
            robot[i].updateMap(gds); // 更新地图
            // for (size_t j = 0; j < robot[i].path.size(); ++j) {
            //     cerr << robot[i].path[j].first << "," << robot[i].path[j].second << endl;
            //     // cerr << robot[i].path.back().first << "," << robot[i].path.back().second << endl;
            // }
            // cerr << robot[i].path.back().first << "," << robot[i].path.back().second << endl;

            if(robot[i].cmd != -1)
                printf("move %d %d\n", i, robot[i].cmd);
            printf("get %d\n", i); //指令：get 机器人id0-9  取货
            printf("pull %d\n", i); //指令：put 机器人id0-9  放货
        //船指令
            // ship id0-4 泊位id0-9 船移动到泊位
            // go id0-4 船运输货物到虚拟点
        }


        puts("OK"); // 所有指令结束后OK
        fflush(stdout);
    }
    for(int zhen = 11; zhen <= 15000; zhen ++) // 按帧处理
    {
        int id = Input(); // 读取场面信息 id第几帧
        if(zhen == 1){
            for(int i = 0; i < robot_num; i ++){
                robot[i].goal = robot[i].pos;
            }
        }


        for(int i = 0; i < robot_num; i ++){ // 第i个机器人的操作
        // for(int i = 0; i < 2; i ++){ // 第i个机器人的操作
        // 路径规划 
            robot[i].getMap(gds); // 传入地图
            robot[i].planPath({berth[i].x+2, berth[i].y+2}); // 修改目标，获取路径
            // robot[i].planPath({29, 147}); // 修改目标，获取路径
            robot[i].move(); // 移动
            robot[i].updateMap(gds); // 更新地图
            // for (size_t j = 0; j < robot[i].path.size(); ++j) {
            //     cerr << robot[i].path[j].first << "," << robot[i].path[j].second << endl;
            //     // cerr << robot[i].path.back().first << "," << robot[i].path.back().second << endl;
            // }
            // cerr << robot[i].path.back().first << "," << robot[i].path.back().second << endl;

            if(robot[i].cmd != -1)
                printf("move %d %d\n", i, robot[i].cmd);
            printf("get %d\n", i); //指令：get 机器人id0-9  取货
            printf("pull %d\n", i); //指令：put 机器人id0-9  放货
        //船指令
            // ship id0-4 泊位id0-9 船移动到泊位
            // go id0-4 船运输货物到虚拟点
        }


        puts("OK"); // 所有指令结束后OK
        fflush(stdout);
    }

    return 0;
}
