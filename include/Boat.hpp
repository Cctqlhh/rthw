#pragma once
using namespace std;

struct Boat
{
    int num;        // 轮船所装有的物品数量？？？
    int goal;        // 表示目标泊位，如果目标泊位是虚拟点，则为-1
    int goal_berth;  // 每艘船的固定目标泊位
    int status;     // 状态, 0 表示移动(运输)中    1 表示正常运行状态(即装货状态或运输完成状态)   2 表示泊位外等待状态

};