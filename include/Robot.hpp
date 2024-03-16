#pragma once
#include"DStarLite.hpp"

using namespace std;

typedef std::pair<State, int> Things; // 货物State（x, y）， 值value

struct Robot
{
    // int x, y, goods;
    int goods = 0; // 0无货，1有货
    int cmd = -1; // 行动指令
    // int cmd;
    State pos; //位置
    int status = 1; // 1正常，0恢复中
    // int mbx, mby;
    State goal; //目标
    int wait = 0; // 因障碍等待的次数，达到1则重新规划路线，否则等待，并+1
    int plan_ready = 0; // 0未准备好，1准备好，-1正在规划

    Robot() {}
    Robot(int startX, int startY):
        pos({startX, startY}), goal({startX, startY}){}

    DStarLite dsl;
    deque<State> path;
    Grid map;
    State berthgoal;    // 机器人的目标泊位
    deque<vector<Things>> things; // 货物队列


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

    void planPath(){
        // 需要等待规划完成
        if(plan_ready == 1){ // 规划完了
            if(goal == dsl.goal() and pos != dsl.goal()){ 
                // 目标地点未变，且未到达目标地点，不用重新规划，可移动，判断机器人障碍
                
                State next = dsl.peekNext(pos); // 下一步位置
                if(map[next.first][next.second] == 1 and wait == 0){
                    wait += 1; // 下一步有机器人障碍,且还未等待，等待次数加一，不调整路径
                }
                else if(map[next.first][next.second] == 1 and wait > 0){
                    adjustPath(); // 等待一次以上,且有机器人障碍物，调整路径
                    wait = 0; // 等待清零
                }
                else if(map[next.first][next.second] == 0 and wait > 0){
                    wait = 0; // 没有障碍物，且等待了，等待次数清零，不调整路径。
                }

            }
            else{ // 目标改变，重新规划路径 / 目标未变，但已到达目标地点，重新规划路径（不存在这种情况，要在其他地方维护goal，即改变目标时goal要改变）
                // 加入计算队列
                // rePlan();
                plan_ready = 0; // 重新规划路径，未准备好
                wait = 0; // 等待清零
            }
        }
        // 规划未完成，则不做操作

        
    }

    void rePlan() // goal为新目标，重新规划路径
    {
        dsl = DStarLite(map, {200, 200}, pos, goal);
        plan_ready = 1;
    }

    void adjustPath()
    {
        dsl.updateMap(map);
    }

    void move()
    {   
        if(plan_ready == 1){
            if(wait > 0) // 等待
                cmd = -1;
            else{ // 不等待
                State next = dsl.moveNext();
                State change = next - pos;
                if (change.first == 1 and change.second == 0) //下
                    cmd = 3;
                else if (change.first == -1 and change.second == 0) //上
                    cmd = 2;
                else if (change.first == 0 and change.second == 1) //右
                    cmd = 0;
                else if (change.first == 0 and change.second == -1) //左
                    cmd = 1;
                else 
                    cmd = -1; // 不移动
                pos = next;
                // path = dsl.getPath(); // 不需要具体路径
            }
        }
        else cmd = -1; // 规划未完成，指令为不移动
        
    }

    void updateMap(Grid &gds)
    {
        gds[pos.first][pos.second] = 1;
    }

    void chooseThings(const deque<vector<Things>> &global_things){  // global_things 是最多20帧的全部物品
        // 计算开始复制的位置。确保不会超出范围。   计算后5个帧的物品信息
        auto start = global_things.size() > 5 ? global_things.end() - 5 : global_things.begin();

        // 创建一个新的deque，包含最后五个元素  行为帧数 列为帧的第几个物品
        things = deque<vector<Things>>(start, global_things.end());
        // 包含后五帧物品与机器人当前位置的曼哈顿距离   things 和 distance 两两对应
        deque<vector<int>> distance;
        // 遍历包含后五帧物品信息的新deque
        for(auto i = things.begin(); i != things.end(); i++){
            vector<int> temp;
            //遍历vector中的元素
            for(auto j = i->begin(); j != i->end(); j++){
                temp.push_back(manhattanDistance(pos.first, pos.second, j->first.first, j->first.second));
            }
            distance.push_back(temp);
        }
        int minDistance = std::numeric_limits<int>::max();
        size_t minDequeIndex = 0;
        size_t minVectorIndex = 0;

        // 遍历deque    在后五帧物品中找出相对于机器人当前位置最近的物品
        for (size_t i = 0; i < distance.size(); ++i) {
            // 遍历vector
            for (size_t j = 0; j < distance[i].size(); ++j) {
                if (distance[i][j] < minDistance) {
                    minDistance = distance[i][j];
                    minDequeIndex = i;      // i为相对于当前帧的帧序号 
                    minVectorIndex = j;     // j为该帧的第几个物品
                }
            }
        }

        // things[minDequeIndex][minVectorIndex];  //返回Things类型



    }

    int manhattanDistance(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

};