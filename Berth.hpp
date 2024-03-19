#pragma once
#include "Things.hpp"
using namespace std;

int manhattanDistance(const int& x1, const int& y1, const int& x2, const int& y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// struct Compare 
// { 
//     bool operator()(const pair<int, int>& a, const pair<int, int>& b) const 
//     { 
//         if (a.first == b.first) 
//         { 
//             return a.second > b.second; // 如果键相同，则按值排序 
//         } 
//         return a.first < b.first; 
//     } 
// };

struct Compare {
    // 根据 价值/距离 比例排序 价值越高，距离越小，比值越高，物品越优
    bool operator()(const pair<int, int>& a, const pair<int, int>& b) const 
    {
        double ratioA = static_cast<double>(a.second) / a.first;
        double ratioB = static_cast<double>(b.second) / b.first;
        
        return ratioA > ratioB;
    }
};


struct Berth
{

    int berth_id;
    int x;
    int y;
    int transport_time;
    int loading_speed;

    int num_in_berth;  // 泊位内当前的物品数量
    Things nearest_thing;           // 历史最近物品
    int min_distance = INT_MAX;     // 历史最近物品的距离

    // multimap<pair<int,int>, Things, Compare> things_map;    // key为(距离，物品价值)，value为物品对象
    // 每个泊位的map可能会产生重复问题，导致泊位根据map更改最优目标物品时产生目标重复问题

    // 修改 things_map 的声明，使其存储指向 Things 对象的指针
    multimap<pair<int, int>, Things*, Compare> things_map;   // key为(距离，物品价值)，value为物品对象的指针

    Berth(){}
    Berth(int x, int y, int transport_time, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
    }

    //选择一帧最近的物品
    void choose_nearest_thing(vector<Things>& cur_things)
    {
        int distance;
        if(cur_things.size() != 0)
        {
            //遍历当前帧中的物品对象，把当前帧的物品全部放入该泊位的物品things_map中
            for (auto it = cur_things.begin(); it != cur_things.end(); ++it)
            {
                distance = manhattanDistance(this->x, this->y, it->x, it->y);
                // 每个泊位存储了每一帧所有的物品信息
                // things_map.insert(make_pair(make_pair(distance, it->value), *it));
                // 将对象的地址插入到 things_map 中
                things_map.insert(make_pair(make_pair(distance, it->value), &(*it)));
            }

            //当前帧最近物品存入map中
            // things_map.insert(make_pair(make_pair(curframe_min_distance,curframe_nearest_thing.value),curframe_nearest_thing));

            update_nearest_thing_from_history();
        }
    }     

    // 机器人获取物品信息前，首先判断nearest_thing是否超时，
    // 如果不超时，函数不操作；
    // 如果超时，则从map容器中顺序找到不超时的第一个最好物品，更新nearest_thing。
    void judge_occupy_timeout(const int& curframe_id)
    {
        while(true){
            auto first_it = this->things_map.begin();

            // 判断是否被占用或超时     维持时间越大，剩余时间越小，越容易消失，越容易拿不到
            if(first_it->second->to_robot == 1 
                or (curframe_id - first_it->second->frame_id) >= 990){ 
                // 如果最优物品已被机器人占用或超时，则删除该物品，切换次优物品
                things_map.erase(things_map.begin()); // 切换次优目标物品
                continue;
            }
            update_nearest_thing_from_history();
            break;
        }
        
    }

    // 机器人取走历史最近物品，需要根据每个泊位的每帧最近物品map，重新更新历史最近物品
    void update_nearest_thing_from_history()
    {
        // 更新泊位最近的物品，即机器人需要取走的物品。
        // 1 机器人取走物品后需要从历史中更新；
        // 2 当前帧物品进入multimap后需要维护最近物品。
        // 3 机器人获取物品信息前，在nearest_thing超时的情况下，取出map中不超时的第一个最好物品，更新nearest_thing。
        
        auto first_it = this->things_map.begin();
        if(*(first_it->second) != this->nearest_thing)
        {   
            this->nearest_thing = *(first_it->second);
            // cerr << things_map.size() << endl;
            // this->things_map.erase(first_it);
        }

    }

};