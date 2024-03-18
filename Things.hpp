#pragma once
using namespace std;

struct Things
{
    int frame_id;
    int x;
    int y;
    int value;
    int dst_to_breth;   //距离某个泊位的距离
    int to_robot = 0;
    Things(){}
    Things(int frame_id,int x, int y, int value) {
        this -> frame_id = frame_id;
        this -> x = x;
        this -> y = y;
        this -> value = value;
    }

    bool operator==(const Things& other) const
    {
        return (this->frame_id == other.frame_id && this->x == other.x && this->y == other.y && this->value == other.value);
    }

    bool operator!=(const Things& other) const
    {
        return!(*this == other);
    }
};