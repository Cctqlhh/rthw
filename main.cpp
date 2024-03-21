#include <bits/stdc++.h>
// 标准库
#include <utility>
#include <algorithm>
#include <vector>

// 算法类头文件
#include "DStarLite.hpp"
// #include "include/DStarLite.hpp"
// 类与全局变量定义头文件
#include "Robot.hpp"
#include "Berth.hpp"
#include "Boat.hpp"
#include "GlobalData.hpp"
#include "Things.hpp"
using namespace std;
using namespace std::chrono;

mutex mtx;
condition_variable cv;
queue<int> computationQueue; // 需要计算路径的机器人队列
bool finished = false; // 标记是否所有帧处理完成

void interactWithJudger(int totalFrames) {
    for (int frame = 1; frame <= totalFrames; ++frame) {
        int id = Input(); // 读取场面信息 id第几帧
        // cerr << "interactWithJudger" << frame << endl;
        // cerr << "input" << frame << endl;
        auto frameStartTime = high_resolution_clock::now();
        if(id == 1){
            for(int i = 0; i < robot_num; i ++){
                robot[i].goal = robot[i].berthgoal;
            }
            for (int i = 0; i < 5; i++){
                boat[i].goal = boat[i].goal_berth;     // 每艘船的目标泊位
                printf("ship %d %d\n", i, boat[i].goal);
            }
        }
        
    // 机器人操作
        for(int i = 0; i < robot_num; ++i){ // 第i个机器人的操作
            // cerr << "robot " << i << endl;
            // cerr << robot[i].status << endl;
        // 路径规划 

            if(id != 1 and robot[i].plan_ready == 0)
                continue;

            robot[i].getMap(gds); // 传入地图
            modifyGoalOfRobot(robot[i], id); // 修改目标（到达则修改）
            // 如果更改会导致在move planPath 中，使 plan_ready变为0
            // robot[i].planPath(); // 路径规划方式：目标改变重新规划/根据规则调整路径

            robot[i].move();  // 移动,planPath已放到move中
            robot[i].updateMap(gds); // 更新地图

            if (robot[i].plan_ready == 0) { // 未准备好，即重新规划路径，加入队列
                unique_lock<mutex> lock(mtx);
                computationQueue.push(i); // 需要计划路径的机器人加入队列
                lock.unlock();
                cv.notify_all(); 
                robot[i].plan_ready = -1; //准备中
                continue;
            }

            if (robot[i].cmd != -1)
                printf("move %d %d\n", i, robot[i].cmd);
            
            if(robot[i].getflag == 1 and robot[i].goods == 0){
                // cerr << i << "get false";
                // cerr << robot[i].pos.first << "," << robot[i].pos.second << "to" << robot[i].goal.first << "," << robot[i].goal.second << endl;
                robot[i].getflag = 0;
            }

            if (robot[i].pos == robot[i].goal and robot[i].goods == 0) // 机器人未携带物品
            {
                printf("get %d\n", i); // 指令：get 机器人id0-9  取货
                if(!(robot[i].pos.first >= berth[robot[i].berthgoal_id].x 
                    and robot[i].pos.first <= berth[robot[i].berthgoal_id].x + 3 
                    and robot[i].pos.second >= berth[robot[i].berthgoal_id].y 
                    and robot[i].pos.second <= berth[robot[i].berthgoal_id].y + 3)){
                        robot[i].getflag = 1;
                        // cerr << robot[i].pos.first << "," << robot[i].pos.second << "to" << robot[i].goal.first << "," << robot[i].goal.second << endl;

                }
            }

            if (robot[i].goods == 1) // 机器人携带物品
            {
                // 机器人当前位置在其对应泊位范围内
                if (robot[i].pos.first >= berth[robot[i].berthgoal_id].x 
                    and robot[i].pos.first <= berth[robot[i].berthgoal_id].x + 3 
                    and robot[i].pos.second >= berth[robot[i].berthgoal_id].y 
                    and robot[i].pos.second <= berth[robot[i].berthgoal_id].y + 3)
                {
                    printf("pull %d\n", i);                      // 指令：pull 机器人id0-9  放货  每个机器人在自己对应泊位放货
                    berth[robot[i].berthgoal_id].num_in_berth += 1; // 泊位物品数量+1
                    // robot[i].goal = robot[i].pos;
                    robot[i].getflag = 0;
                }
            }
        }
        // cerr << "robot" << frame << endl;


        // // 轮船指令
        // for (size_t i = 0; i < 5; i++)
        // {
        //     if (boat[i].status == 0) // 船在移动中 不做任何其他事情
        //     {
        //         continue;
        //     }
        //     if (boat[i].status == 1) // 船在移动完成或者装货状态（装货状态是到达泊位，不装货状态是到达虚拟点）
        //     {
        //         // 轮船到达泊位，开始装货   每一帧一定装货
        //         if (boat[i].goal != -1) // 船到达且船的目标不是虚拟点==船到达泊位
        //         {
        //             // 泊位装货物（判题器自动）   用于维护5个船的货物数量 一定会发生，所以放在前面
        //             if (berth[boat[i].goal_berth].num_in_berth <= berth[boat[i].goal_berth].loading_speed)
        //             {
        //                 boat[i].num += berth[boat[i].goal_berth].num_in_berth;
        //                 berth[boat[i].goal_berth].num_in_berth = 0; // 泊位的物品数量到零 全部裝上船
        //             }
        //             else
        //             {
        //                 boat[i].num += berth[boat[i].goal_berth].loading_speed;
        //                 berth[boat[i].goal_berth].num_in_berth -= berth[boat[i].goal_berth].loading_speed;
        //             }
        //             // 船最后一次去虚拟点的时候如果没有装满物品也需要出发并且能够在最后到达虚拟点，避免浪费最后装的物品
        //             if (15000 - frame <= berth[boat[i].goal_berth].transport_time + 10)  //可修改
        //             {
        //                 printf("go %d\n", i);
        //                 continue; // 船最后一次去虚拟点，且没有装满物品，不再给该船下达指令
        //             }
        //             // 只有在船装满货物之后，才给船下达指令
        //             if (boat[i].num < boat_capacity)
        //             {
        //                 continue;   // 若该船没有装满，则不给该船下达指令
        //             }
        //             //船的物品数量被程序装超载，但实际上船不会超载，满了就不会再装了
        //             if (boat[i].num >= boat_capacity)
        //             {
        //                 berth[boat[i].goal_berth].num_in_berth = berth[boat[i].goal_berth].num_in_berth + (boat[i].num - boat_capacity);
        //                 // boat[i].num = boat_capacity; // 船装满之后，船的货物数量不超过船的容量                        
        //             }
        //             printf("go %d\n", i);
                    
        //         }
        //         if (boat[i].goal == -1) // 船已到达且目标是虚拟点==船到达虚拟点
        //         {
        //             boat[i].num = 0; // 清空船的货物数量 全部转换成价值
        //             boat[i].goal = boat[i].goal_berth; //此处之后可改目标泊位
        //             printf("ship %d %d\n", i, boat[i].goal);
        //         }
        //     }
        // }


        // 轮船指令
        for (size_t i = 0; i < 5; i++)
        {
            if (boat[i].status == 0) // 船在移动中 不做任何其他事情
            {
                // 在3*transport_time+500强行发走，先清空船的容量，保证最后一次发走的时刻，船停留在泊位装货
                if (15000 - id == 3*berth[boat[i].goal_berth].transport_time + 500)  //可修改
                {
                    printf("go %d\n", i);
                    cerr<<" boat zai lu shang   "<<boat[i].num<<endl;
                    continue;
                }
                continue;
            }
            if (boat[i].status == 1) // 船在移动完成或者装货状态（装货状态是到达泊位，不装货状态是到达虚拟点）
            {
                // 轮船到达泊位，开始装货   每一帧一定装货
                if (boat[i].goal != -1) // 船到达且船的目标不是虚拟点==船到达泊位
                {
                    // 泊位装货物（判题器自动）   用于维护5个船的货物数量 一定会发生，所以放在前面
                    if (berth[boat[i].goal_berth].num_in_berth <= berth[boat[i].goal_berth].loading_speed)
                    {
                        boat[i].num += berth[boat[i].goal_berth].num_in_berth;
                        berth[boat[i].goal_berth].num_in_berth = 0; // 泊位的物品数量到零 全部裝上船
                    }
                    else
                    {
                        boat[i].num += berth[boat[i].goal_berth].loading_speed;
                        berth[boat[i].goal_berth].num_in_berth -= berth[boat[i].goal_berth].loading_speed;
                    }

                    // 在3*transport_time+500强行发走，先清空船的容量，保证最后一次发走的时刻，船停留在泊位装货
                    if (15000 - id == 3*berth[boat[i].goal_berth].transport_time + 500)  //可修改
                    {
                        cerr<<" boat zai bo wei   "<<boat[i].num<<endl;
                        printf("go %d\n", i);
                        continue;
                    }
                    

                    // 船最后一次去虚拟点的时候如果没有装满物品也需要出发并且能够在最后到达虚拟点，避免浪费最后装的物品
                    if (15000 - id <= berth[boat[i].goal_berth].transport_time + 10)  //可修改
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
                        berth[boat[i].goal_berth].num_in_berth = berth[boat[i].goal_berth].num_in_berth + (boat[i].num - boat_capacity);
                        // boat[i].num = boat_capacity; // 船装满之后，船的货物数量不超过船的容量                        
                    }
                    printf("go %d\n", i);
                    
                }
                if (boat[i].goal == -1) // 船已到达且目标是虚拟点==船到达虚拟点
                {
                    boat[i].num = 0; // 清空船的货物数量 全部转换成价值
                    boat[i].goal = boat[i].goal_berth; //此处之后可改目标泊位
                    printf("ship %d %d\n", i, boat[i].goal);
                    // 在3*transport_time+500强行发走，先清空船的容量，保证最后一次发走的时刻，船停留在泊位装货
                    if (15000 - id == 3*berth[boat[i].goal_berth].transport_time + 500)  //可修改
                    {
                        cerr<<" boat zai xu ni dian   "<<boat[i].num<<endl;
                        printf("go %d\n", i);
                        continue;
                    }
                }
            }
        }



        auto frameEndTime = frameStartTime + milliseconds(14);
        while (high_resolution_clock::now() < frameEndTime) {
            // 忙等待
        }
        // 发送“OK”
        
        puts("OK"); // 所有指令结束后OK
        // cerr << "OK" << id << endl;
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
            //重新规划时需要获取最新地图
            robot[robotId].getMap_cst(gds);
            robot[robotId].rePlan();
            lock.lock();
        }
    }
}

int main() {
    const int totalFrames = 15000;
    Init(); //初始化
    // cerr << "Init" << endl;
    thread judgerThread(interactWithJudger, totalFrames);
    thread planningThread(pathPlanning);

    judgerThread.join();
    planningThread.join();

    return 0;
}