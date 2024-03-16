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
using namespace std::chrono;

mutex mtx;
condition_variable cv;
queue<int> computationQueue; // 需要计算路径的机器人队列
bool finished = false; // 标记是否所有帧处理完成

void interactWithJudger(int totalFrames) {
    for (int frame = 1; frame <= totalFrames; ++frame) {
        // cerr << "interactWithJudger" << frame << endl;
        int id = Input(); // 读取场面信息 id第几帧
        auto frameStartTime = high_resolution_clock::now();
        if(id == 1){
            for(int i = 0; i < robot_num; i ++){
                // robot[i].goal = robot[i].pos;
                robot[i].goal = robot[i].berthgoal;
            }
        }
        
    // 机器人操作
        for(int i = 0; i < robot_num; ++i){ // 第i个机器人的操作
        // for(int i = 0; i < 5; i ++){ // 第i个机器人的操作
        // 路径规划 
            robot[i].getMap(gds); // 传入地图
            robot[i].planPath(); // 获取路径

            if (robot[i].plan_ready == 0) {
                unique_lock<mutex> lock(mtx);
                computationQueue.push(i); // 需要计划路径的机器人加入队列
                lock.unlock();
                cv.notify_all(); 
                robot[i].plan_ready = -1;
            }
            // cerr << robot[i].plan_ready << endl;
            // cerr << robot[i].goal.first << " " << robot[i].goal.second
            // << " from " << robot[i].pos.first << " " << robot[i].pos.second << endl;
            
            robot[i].move();  // 移动
            if(robot[i].cmd != -1)
                printf("move %d %d\n", i, robot[i].cmd);
            // printf("get %d\n", i); // 指令：get 机器人id0-9 取货
            // printf("pull %d\n", i); // 指令：pull 机器人id0-9 放货

            robot[i].updateMap(gds); // 更新地图
        }
        //船指令
            // ship id0-4 泊位id0-9 船移动到泊位
            // go id0-4 船运输货物到虚拟点
        
        // // 等待直到接近15ms

        auto frameEndTime = frameStartTime + milliseconds(14);
        while (high_resolution_clock::now() < frameEndTime) {
            // 忙等待
        }
        // std::this_thread::sleep_until(frameEndTime);

        // while (duration_cast<milliseconds>(high_resolution_clock::now() - frameStartTime).count() < 14) {
        //     this_thread::sleep_for(milliseconds(1)); // 防止过早发送
        // }

        // 发送“OK”
        puts("OK"); // 所有指令结束后OK
        fflush(stdout);
        
    }
    // 标记完成，通知路径规划线程
    lock_guard<mutex> lock(mtx);
    finished = true;
    cv.notify_all();
    
}

void pathPlanning() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx); // 加锁
        cv.wait(lock, [] { return !computationQueue.empty() || finished; });
        if (finished) {
            lock.unlock(); // 解锁
            break; // 所有帧处理完成,退出线程
        }

        while (!computationQueue.empty()) {
            int robotId = computationQueue.front();
            computationQueue.pop();

            lock.unlock();

            robot[robotId].rePlan();

            lock.lock();
        }
    }
}

int main() {
    const int totalFrames = 15000;
    Init(); //初始化
    cerr << "Init" << endl;
    thread judgerThread(interactWithJudger, totalFrames);
    thread planningThread(pathPlanning);

    judgerThread.join();
    planningThread.join();

    return 0;
}

// int main()
// {
//     Init(); //初始化
    
//     for(int zhen = 1; zhen <= 15000; zhen ++) // 按帧处理
//     {
//         int id = Input(); // 读取场面信息 id第几帧
//         if(zhen == 1){
//             for(int i = 0; i < robot_num; i ++){
//                 robot[i].goal = robot[i].pos;
//             }
//         }


//         for(int i = 0; i < robot_num; i ++){ // 第i个机器人的操作
//         // for(int i = 0; i < 5; i ++){ // 第i个机器人的操作
//         // 路径规划 
//             robot[i].getMap(gds); // 传入地图
//             robot[i].planPath(robot[i].berthgoal); // 修改目标，获取路径
//             // robot[i].planPath({29, 147}); // 修改目标，获取路径
//             robot[i].move(); // 移动
            
//             if(robot[i].cmd != -1)
//                 printf("move %d %d\n", i, robot[i].cmd);
//             printf("get %d\n", i); //指令：get 机器人id0-9  取货
//             printf("pull %d\n", i); //指令：pull 机器人id0-9  放货
//         //船指令
//             // ship id0-4 泊位id0-9 船移动到泊位
//             // go id0-4 船运输货物到虚拟点

//             robot[i].updateMap(gds); // 更新地图
//         }


//         puts("OK"); // 所有指令结束后OK
//         fflush(stdout);
//     }

//     return 0;
// }
