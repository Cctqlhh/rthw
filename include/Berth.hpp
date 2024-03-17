#pragma once
#include "Things.hpp"
using namespace std;


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
    map<int, Things> things_map;    // 每个泊位的map可能会产生重复问题，导致泊位根据map更改最优目标物品时产生目标重复问题


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
                it->dst_to_breth = distance;
                //选出当前帧的最近物品
                if (distance < curframe_min_distance)  //如果当前帧物品距离比历史最近物品更近，则更新
                {
                    curframe_min_distance = distance;
                    curframe_nearest_thing = *it;
                    curframe_it = it;
                }
            }

            //当前帧最近物品存入map中
            things_map[cur_things[0].frame_id] = curframe_nearest_thing;

            //更新历史最近物品
            if( curframe_min_distance < min_distance)
            {
                this->min_distance = curframe_min_distance;
                this->nearest_thing = curframe_nearest_thing;
                cur_things.erase(curframe_it);   //删除当前帧最近物品
            }
        }

        // 每一帧一定要运行此函数，不管当前泊位检测到的当前帧物品是否为空
        // 每一帧更新完毕，判断是否超时，超时则删除map中的物品
        // 对map容器的每帧最近物品进行判断，如果超时，则删除
        for (auto it = things_map.begin(); it != things_map.end(); ++it)
        {
            if((1000-(cur_things[0].frame_id - it->first)) <= 300)
            {
                things_map.erase(it);
            }
        }
        // 如果历史最近物品超时，则根据map容器强行更新历史最近物品
        if((1000-(cur_things[0].frame_id - nearest_thing.frame_id)) <= 300)
        {
            int cur_min_distance = INT_MAX;            // 当前最近物品的距离(用于遍历map容器)
            Things cur_nearest_thing;                  // 当前最近物品(用于遍历map容器)
            for (auto it = things_map.begin(); it != things_map.end(); ++it)
            {
                if (it->second.dst_to_breth < cur_min_distance)
                {
                    cur_min_distance = it->second.dst_to_breth;
                    cur_nearest_thing = it->second;
                }
            }
            this->min_distance = cur_min_distance;
            this->nearest_thing = cur_nearest_thing;
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

    // 机器人取走历史最近物品，需要根据每个泊位的每帧最近物品map，重新更新历史最近物品
    void update_nearest_thing_from_history()
    {
        int cur_min_distance = INT_MAX;
        Things cur_nearest_thing;

        things_map.erase(this->nearest_thing.frame_id);  //先将历史最近物品从map中删除

        //每次都要遍历things_map，这里可以单独写一个遍历函数，减少重复代码
        int cur_min_distance = INT_MAX;
        Things cur_nearest_thing;
        for (auto it = things_map.begin(); it != things_map.end(); ++it)
        {
            if (it->second.dst_to_breth < cur_min_distance)
            {
                cur_min_distance = it->second.dst_to_breth;
                cur_nearest_thing = it->second;
            }
        }
        this->min_distance = cur_min_distance;
        this->nearest_thing = cur_nearest_thing;
    }

};