#pragma once
#include <bits/stdc++.h>
#include "Robot.hpp"
#include "Berth.hpp"
#include "Boat.hpp"
#include "Things.hpp"
using namespace std;

typedef std::pair<int, int> State; // 位置坐标，x，y
// typedef std::pair<State, int> Things; // 货物State（x, y）， 值value
typedef std::vector<vector<int>> Grid; //二维vector，地图值？

// //创建储存全图物品信息的map
// map<int, vector<Things>> things_map;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;

Robot robot[robot_num + 10];
// Berth berth[berth_num + 10];
vector<Berth> berth(10);
Boat boat[10];

int money, boat_capacity, id;
char ch[N][N];
Grid gds(n, vector<int>(n, 1)); //全1地图

// deque<vector<Things>> things;
// vector<Things> cur_things;

bool compareByTransportTime(const Berth& a, const Berth& b);

void Init()
{
    for(int i = 1; i <= n; i ++) {// ��ͼ 200*200
        scanf("%s", ch[i] + 1);
        for(int j = 1; j <= n; j ++){
            if(ch[i][j] == '.' or ch[i][j] == 'B') {
                gds[i-1][j-1] = 0;
            }
            // if(ch[i][j] == 'A') {
            //     gds[i-1][j-1] = 1;
            // }
            // else {
            //     gds[i-1][j-1] = 1;
            // }
        }
    }
    for(int i = 0; i < berth_num; i ++) // ��λ 10
    {
        int id;
        scanf("%d", &id); // ��λ��� 0-9
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed); // ��λ���Ͻ�����4*4 ����ʱ��֡�� װ���ٶ�ÿ֡װ����
        berth[id].berth_id = id;
    }

    vector<Berth> berth_order = berth;
    // 根据transport_time对vector<Berth>排序
    sort(berth_order.begin(), berth_order.end(), compareByTransportTime);

     // 设置5艘船的固定目标泊位为前五个泊位
    for(int i = 0; i < 5; i ++) //判题器的泊位序号是泊位的id号
    {
        boat[i].goal_berth = berth_order[i].berth_id;    // 每艘船的固定目标泊位
    }


    for(int i = 0;i<5;i++)
    {
        robot[i].berthgoal = {berth_order[i%5].x, berth_order[i%5].y};
    }
    
    for(int i = 5;i<10;i++)
    {
        robot[i].berthgoal = {berth_order[i%5].x+3, berth_order[i%5].y+3};
    }




    scanf("%d", &boat_capacity); // �����������װ����Ʒ��
    char okk[100];
    scanf("%s", okk); // ��ȡOK
    printf("OK\n"); // �ظ�ok
    fflush(stdout);
}

int Input()
{
    vector<Things> cur_things; // 当前帧的物品信息

    //从第1帧开始 从1开始递增
    scanf("%d%d", &id, &money); // 帧数，已经获取的money
    //当前帧的物品信息cur_things在每一帧填入前清空

    int num;
    scanf("%d", &num); // ������������0-10
    for(int i = 1; i <= num; i ++) // ����������Ϣ
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val); // λ�� ���<=1000

        Things thing(id, x, y, val); //创建一个物品对象
        // things_map.insert(make_pair(id, thing)); // 加入到things_map中
        // cur_things.push_back({{x, y}, val});

        cur_things.push_back(thing); // 加入到当前帧的物品信息
    }

    // 遍历选出的5个泊位 并选择每个泊位在当前帧以及之前帧中最近的物品
    // 五个泊位选出自己的最近物品之后，更新map容器
    // 每帧的开始，选出5个泊位最近的物品
    for(int i = 0; i < 5; i ++)
    {
        berth[boat[i].goal_berth].choose_nearest_thing(cur_things);
    }

    // //当前帧的物品信息cur_things（已经去除了5个被锁定的物品）加入到things_map中
    // things_map.insert(make_pair(id, cur_things));
    
    

    // if(things.size() == 20)
    // {
    //     things.pop_front();
    // }

    // things.push_back(cur_things);

    for(int i = 0; i < robot_num; i ++) // ��������Ϣ
    {
        // int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].pos.first, &robot[i].pos.second, &robot[i].status); // �Ƿ�Я����Ʒ01 ���� ״̬0�ָ�1����
    }
    for(int i = 0; i < 5; i ++) // 5���� i��Ӧ��id0-4
    {
        scanf("%d%d\n", &boat[i].status, &boat[i].pos); // ״̬0�ƶ�(����)1����װ��/�������  Ŀ�겴λ����������-1��
    }
    char okk[100];
    scanf("%s", okk); //����OK
    return id;
}


// berth比较函数
bool compareByTransportTime(const Berth& a, const Berth& b) {
    return a.transport_time < b.transport_time;
}




