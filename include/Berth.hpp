#pragma once
using namespace std;

struct Berth
{
    int x;
    int y;
    int transport_time;
    int loading_speed;
    Berth(){}
    Berth(int x, int y, int transport_time, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
    }
};