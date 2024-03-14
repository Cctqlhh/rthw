#pragma once
using namespace std;

struct Berth
{
    int Berth_id;
    int x;
    int y;
    int transport_time;
    int loading_speed;
    int num_in_berth;  // 泊位内当前的物品数量

    Berth(){}
    Berth(int x, int y, int transport_time, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
    }
};