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
    Things nearest_thing;   // 目前最近的物品
    int min_distance = INT_MAX;
    Berth(){}
    Berth(int x, int y, int transport_time, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
    }

    //选择一帧最近的物品
    auto choose_nearest_thing(vector<Things>& cur_things)
    {
        Things nearest_thing;
        int distance;
        int min_distance;
        //遍历当前帧中的物品对象
        for (auto it = cur_things.begin(); it != cur_things.end(); ++it)
        {

            distance = abs(this->x - it->x) + abs(this->y - it->y);
            if (distance < this->min_distance)  //如果当前帧物品距离比历史最近物品更近，则更新
            {
                this->min_distance = distance;
                this->nearest_thing  = *it;

                cur_things.erase(it);
            }
        }

        //物品剩余帧时间小于最短距离，则说明该物品已经过时，可以更新
        if((1000-(cur_things[0].frame_id - nearest_thing.frame_id)) < min_distance)
        {
            
        }
       
    }
    


};