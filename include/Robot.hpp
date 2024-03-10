#pragma once
#include"DStarLite.hpp"

using namespace std;

typedef std::pair<State, int> Things; // 货物State（x, y）， 值value

struct Robot
{
    // int x, y, goods;
    int goods = 0; // 0无货，1有货
    int cmd = -1;
    // int cmd;
    State pos;
    int status = 1; // 1正常，0恢复中
    // int mbx, mby;
    State goal;
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
        gds[pos.first][pos.second] = 0;
        map = gds;
    }

    void planPath(const State &newgoal){
        State new_goal = goal;
        // 是否重新选取目标地点（条件：到达终点/有更好的目标地点选取）
        if(pos == goal){ // 到达终点,重新选取目标地点
            new_goal = newgoal;
        }
        // 其他条件

        
        if(new_goal == goal){ // 目标不变，则依据障碍物调整路径
            adjustPath(map);           
        }
        else{ // 目标改变，重新规划路径
            modifyTarget(new_goal);
        }
        
    }

    void modifyTarget(State new_goal)
    {
        // if (new_goal == goal)
        //     return;
        goal = new_goal;
        dsl = DStarLite(map, {200, 200}, pos, goal);
    }

    void adjustPath(const Grid &newMap)
    {
        dsl.updateMap(newMap);
        
    }

    void move()
    {
        
        // if (path.size() == 0)
        //     return;
        // State now = path.front();
        // if(now != pos)
        //     return;
        
        // path.pop_front();
        // State next = path.front();
        // State change = next - now;
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
        path = dsl.getPath();
    
    }

    void updateMap(Grid &gds)
    {
        gds[pos.first][pos.second] = 1;
    }

    void chooseThings(const deque<vector<Things>> &global_things){
        // 计算开始复制的位置。确保不会超出范围。
        auto start = global_things.size() > 5 ? global_things.end() - 5 : global_things.begin();

        // 创建一个新的deque，包含最后五个元素
        
        things = deque<vector<Things>>(start, global_things.end());
        deque<vector<int>> distance;
        for(auto i = things.begin(); i != things.end(); i++){
            vector<int> temp;
            for(auto j = i->begin(); j != i->end(); j++){
                temp.push_back(manhattanDistance(pos.first, pos.second, j->first.first, j->first.second));
            }
            distance.push_back(temp);
        }
    int minDistance = std::numeric_limits<int>::max();
    size_t minDequeIndex = 0;
    size_t minVectorIndex = 0;

    // 遍历deque
    for (size_t i = 0; i < distance.size(); ++i) {
        // 遍历vector
        for (size_t j = 0; j < distance[i].size(); ++j) {
            if (distance[i][j] < minDistance) {
                minDistance = distance[i][j];
                minDequeIndex = i;
                minVectorIndex = j;
            }
        }
    }

    

    }

    int manhattanDistance(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

};