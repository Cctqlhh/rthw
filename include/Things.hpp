#pragma once
using namespace std;

struct Things
{
    int frame_id;
    int x;
    int y;
    int value;
    Things(){}
    Things(int frame_id,int x, int y, int value) {
        this -> frame_id = frame_id;
        this -> x = x;
        this -> y = y;
        this -> value = value;
    }
};