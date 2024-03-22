#pragma once
#include "DStarLite.hpp"
using namespace std;
using namespace std::chrono;
// typedef std::pair<State, int> Things; // 货物State（x, y）， 值value

struct Robot
{
    // int x, y, goods;
    int robot_id;
    int getflag = 0;
    int goods = 0; // 0无货，1有货
    int cmd = -1; // 行动指令
    State pos; //位置
    
    int status = 1; // 1正常，0恢复中
    // int mbx, mby;
    State goal; //目标
    State next; // 下一位置
    int wait = 0; // 因障碍等待的次数，达到1则重新规划路线，否则等待，并+1
    int plan_ready = 0; // 0未准备好，1准备好，-1正在规划
    int stop_flag = 0; // 0未因莫名原因停止，1为第一种情况，2为第二种情况

    Robot() {}
    Robot(int startX, int startY):
        pos({startX, startY}), goal({startX, startY}){}

    DStarLite dsl;
    deque<State> path;
    Grid map;

    int berthgoal_id;   // 机器人的目标泊位id
    State berthgoal;    // 机器人的目标泊位坐标
    vector<int> berth_id_able{1, 1, 1, 1, 1, 1, 1, 1, 1, 1};// 泊位是否可用
    int thing_flag = -1; 
    // -1目标为泊位；0没有目标物品；1有目标物品，但未确定是否能到达；2能到达目标物品；3不能到达目标物品

    int cantgo = 0;

    // void wat(){
    //     // 是否重新选取目标地点（条件：到达终点/有更好的目标地点选取）
    //     // 选取目标，判断目标是否改变，改变则完全重新规划路径
    //     // 目标未改变，路径不重新规划，但要判断是否有新的障碍物或奖励，调整路径，更新路径
    //     // 移动，调整路径
    // }
    
    void getMap(Grid &gds)
    {
        gds[pos.first][pos.second] = 0; //将机器人当前位置在地图上设置为0
        map = gds;
    }

    void getMap_cst(const Grid &gds)
    {
        map = gds;
        map[pos.first][pos.second] = 0; //将机器人当前位置在地图上设置为0
    }

    void updateMap(Grid &gds)
    {
        gds[pos.first][pos.second] = 1;
    }

    void planPath(){
        if(goal == dsl.goal() and pos != dsl.goal()){ 
            // 目标地点未变，且未到达目标地点，不用重新规划，可移动，判断机器人障碍
            // if(stop_flag == 2){
            //     next = dsl.peekNext_ez(pos);
            // }
            // else 

            next = dsl.peekNext(pos); // 下一步位置


            if(map[next.first][next.second] == 1 and wait == 0){
                wait += 1; // 下一步有机器人障碍,且还未等待，等待次数加一，不调整路径
            }
            else if(map[next.first][next.second] == 1 and wait > 0){
// //////        
//             auto start1 = high_resolution_clock::now();
// //////
                adjustPath(); // 等待一次以上,且有机器人障碍物，调整路径
// //////          
//             auto end = high_resolution_clock::now();
//             // 计算运行时间
//             auto duration = duration_cast<std::chrono::milliseconds>(end - start1);
//             // 输出运行时间
//             std::cerr << "Program ran for " << duration.count() << " ms." << std::endl;
// //////
                
                    //好像不是/// 碰撞原因： 等待一次后调整路径，等待清零，但没有判断新的路径上下一步有没有机器人障碍。
            }
            else if(map[next.first][next.second] == 0 and wait > 0){
                wait = 0; // 没有障碍物，且等待了，等待次数清零，不调整路径。
            }

        }
        else{ // 目标改变，重新规划路径 / 目标未变，但已到达目标地点，重新规划路径（不存在这种情况，要在其他地方维护goal，即改变目标时goal要改变）
            // 加入计算队列
            plan_ready = 0; // 重新规划路径，未准备好
            wait = 0; // 等待清零
        }
        
    }

    void Plan_to_Berth(const Grid& m){
        dsl = DStarLite(m, {200, 200}, pos, berthgoal);
        if(!dsl.isPathAvailable())
            berth_id_able[berthgoal_id] = 0;
    }

    bool isBerthAble(){
        return berth_id_able[berthgoal_id];
    }

    void rePlan() // goal为新目标，重新规划路径
    {
        if(map[goal.first][goal.second] == 1){
            map[goal.first][goal.second] = 0;
        }
        dsl = DStarLite(map, {200, 200}, pos, goal);
    }

    void adjustPath()
    {
        // dsl.updateMap(map);
        // if(next == goal or (
            // next.first >= berthgoal.first
            // and next.first <= berthgoal.first + 3 
            // and next.second >= berthgoal.second 
            // and next.second <= berthgoal.second + 3))
        if(next == goal)
            wait = wait;
        else {
            dsl.toggleCell(next); // 障碍物位置
            next = dsl.peekNext(pos);
            if(map[next.first][next.second] == 1){
                wait = wait;
            }
            else{
                wait = 0; // 等待清零
            }
        }
        
    }

    void move()
    {   
        // 若没规划完，则不移动
        // 若规划完 ready，进入planPath判断是否重新规划路线
            // 若不重新规划，仍然ready，则根据规则移动
            // 若重新规划，！ready，则不移动等待规划完
        if(plan_ready == 1){
            planPath();
            if(plan_ready == 1){
                if(wait > 0) {// 等待
                    cmd = -1;
                    // cerr << "wait" << endl;
                }
                else{ // 不等待
                // 如果重新规划时候，在前面判断机器人障碍时候未准备好，move前准备好了，
                // 则会进行move，但此时并未判断新的路径前方是否有机器人障碍，
                // 因为路径规划时输入的map可能是上一帧的。
                    // State next = dsl.moveNext();
                    // next = dsl.peekNext(pos);
                    // cerr << pos.first << "," << pos.second << " to " << next.first << "," << next.second << endl;
                    State change = next - pos;
                    if (change.first == 1 and change.second == 0){ //下
                        cmd = 3;
                        dsl.updateSCurrent(next);
                    } 
                    else if (change.first == -1 and change.second == 0){ //上
                        cmd = 2;
                        dsl.updateSCurrent(next);
                    }
                    else if (change.first == 0 and change.second == 1){ //右
                        cmd = 0;
                        dsl.updateSCurrent(next);
                    }
                    else if (change.first == 0 and change.second == -1){ //左
                        cmd = 1;
                        dsl.updateSCurrent(next);
                    }
                    else{
                        // cerr << "pos == goal wait why" << endl;
                        // cerr << pos.first << "," << pos.second << 
                        // " " << goal.first << "," << goal.second <<
                        // " " << next.first << "," << next.second << " " << endl;
                        // cerr << dsl.goal().first << "," << dsl.goal().second << endl;
                        stop_process();
                        cmd = -1; // 不移动
                    }
                    pos = next;
                    // path = dsl.getPath(); // 不需要具体路径
                }
            }
            else {
                cmd = -1;
                // cerr << "need replan" << endl;
            }
        }
        else cmd = -1; // 规划未完成，指令为不移动
    }

    void stop_process(){
        // 如果停在泊位内，或正在前往目标物品(未携带物品)，则改变物品目标，重新规划
        if (pos.first >= berthgoal.first
            and pos.first <= berthgoal.first + 3 
            and pos.second >= berthgoal.second 
            and pos.second <= berthgoal.second + 3
            or goods == 0){
            stop_flag = 1;
        }
        else{ // 如果停在外面，且携带物品
            stop_flag = 2;
        }
    }

};