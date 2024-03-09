#pragma once
#include <bits/stdc++.h>
#include "Robot.hpp"
#include "Berth.hpp"
#include "Boat.hpp"
using namespace std;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;

Robot robot[robot_num + 10];
Berth berth[berth_num + 10];
Boat boat[10];

int money, boat_capacity, id;
char ch[N][N];
int gds[N][N];


void Init()
{
    for(int i = 1; i <= n; i ++) // 地图 200*200
        scanf("%s", ch[i] + 1);
    for(int i = 0; i < berth_num; i ++) // 泊位 10
    {
        int id;
        scanf("%d", &id); // 泊位编号 0-9
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed); // 泊位左上角坐标4*4 运输时间帧数 装载速度每帧装几个
    }
    scanf("%d", &boat_capacity); // 船的容量最多装的物品数
    char okk[100];
    scanf("%s", okk); // 读取OK
    printf("OK\n"); // 回复ok
    fflush(stdout);
}

int Input()
{
    scanf("%d%d", &id, &money); // 帧序号 钱
    int num;
    scanf("%d", &num); // 新增货物数量0-10
    for(int i = 1; i <= num; i ++) // 新增货物信息
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val); // 位置 金额<=1000
    }
    for(int i = 0; i < robot_num; i ++) // 机器人信息
    {
        // int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &robot[i].status); // 是否携带物品01 坐标 状态0恢复1正常
    }
    for(int i = 0; i < 5; i ++) // 5个船 i对应其id0-4
        scanf("%d%d\n", &boat[i].status, &boat[i].pos); // 状态0移动(运输)1正常装货/运输完成  目标泊位（如果虚拟点-1）
    char okk[100];
    scanf("%s", okk); //接受OK
    return id;
}




