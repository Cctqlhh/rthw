#pragma once
#include "Things.hpp"
using namespace std;


struct Compare 
{ 
    bool operator()(const pair<int, int>& a, const pair<int, int>& b) const 
    { 
        if (a.first == b.first) 
        { 
            return a.second > b.second; // 如果键相同，则按值排序 
        } 
        return a.first < b.first; 
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
    multimap<pair<int,int>, Things, Compare> things_map;    // key为(距离，物品价值)，value为物品对象
    // 每个泊位的map可能会产生重复问题，导致泊位根据map更改最优目标物品时产生目标重复问题


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
        int curframe_min_distance = INT_MAX;    // 当前帧最近物品的距离
        Things curframe_nearest_thing;          // 当前帧最近物品
        auto curframe_it = cur_things.begin();  // 当前帧最近物品迭代器
        if(cur_things.size() != 0)
        {
            //遍历当前帧中的物品对象，选出当前帧最近的物品
            for (auto it = cur_things.begin(); it != cur_things.end(); ++it)
            {
                distance = abs(this->x - it->x) + abs(this->y - it->y); // 修改为曼哈顿距离函数！！！！！！！！！！！
                // it->dst_to_breth = distance;
                //选出当前帧的最近物品
                if (distance < curframe_min_distance)  //如果当前帧物品距离比历史最近物品更近，则更新
                {
                    curframe_min_distance = distance;
                    curframe_nearest_thing = *it;
                    curframe_it = it;
                }
            }

            //当前帧最近物品存入map中
            things_map.insert(make_pair(make_pair(curframe_min_distance,curframe_nearest_thing.value),curframe_nearest_thing));

            update_nearest_thing_from_history();
        }
    }     

    // void changeGoal(int curframe_id, int status)//当前帧id，机器人状态
    // {
    //     // 对map容器的每帧最近物品进行判断，如果超时，则删除
    //     for (auto it = things_map.begin(); it != things_map.end(); ++it)
    //     {
    //         if((1000-(curframe_id - it->first)) <= 300)
    //         {
    //             things_map.erase(it);
    //         }
    //     }
    //     // 2.如果历史最近物品超时，则根据map容器强行更新历史最近物品
    //     if((1000-(curframe_id - nearest_thing.frame_id)) <= 300)
    //     {
    //         int cur_min_distance = INT_MAX;
    //         Things cur_nearest_thing;
    //         for (auto it = things_map.begin(); it != things_map.end(); ++it)
    //         {
    //             if (it->second.dst_to_breth < cur_min_distance)
    //             {
    //                 cur_min_distance = it->second.dst_to_breth;
    //                 cur_nearest_thing = it->second;
    //             }
    //         }
    //         this->min_distance = cur_min_distance;
    //         this->nearest_thing = cur_nearest_thing;
    //     }

    //     //2.机器人取走历史最近物品，重新更新历史最近物品
    //     //如何判断机器人取走了历史最近物品？要根据机器人的状态吗，还是机器人的坐标
    //     if(status == 1)
    //     {//机器人取走了历史最近物品，更新历史最近物品
    //         things_map.erase(this->nearest_thing.frame_id);  //先将历史最近物品从map中删除

    //         //每次都要遍历things_map，这里可以单独写一个遍历函数，减少重复代码
    //         int cur_min_distance = INT_MAX;
    //         Things cur_nearest_thing;
    //         for (auto it = things_map.begin(); it != things_map.end(); ++it)
    //         {
    //             if (it->second.dst_to_breth < cur_min_distance)
    //             {
    //                 cur_min_distance = it->second.dst_to_breth;
    //                 cur_nearest_thing = it->second;
    //             }
    //         }
    //         this->min_distance = cur_min_distance;
    //         this->nearest_thing = cur_nearest_thing;

    //     }
    // }


    // 机器人获取物品信息前，首先判断nearest_thing是否超时，
    // 如果不超时，函数不操作；
    // 如果超时，则从map容器中顺序找到不超时的第一个最好物品，更新nearest_thing。
    void judge_timeout(const int& curframe_id)
    {
        if((curframe_id - nearest_thing.frame_id) >= 700)   // 物品存在了700帧以上，即还剩不到300帧，则超时
        {
            while(true){
                auto first_it = this->things_map.begin();
                if((curframe_id - first_it->second.frame_id) >= 700)
                {
                    this->things_map.erase(first_it);
                }
                else
                    break;
                
            }
            update_nearest_thing_from_history();
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
        if(first_it->second != this->nearest_thing)
        {
            this->nearest_thing = first_it->second;
            this->things_map.erase(first_it);  
        }

    }

};