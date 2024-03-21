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
const int boat_num = 5;
const int berth_num = 10;
const int N = 210;

Robot robot[robot_num + 10];
// Berth berth[berth_num + 10];
vector<Berth> berth(10);
vector<Berth> berth_order(10);  //对berth进行排序后的容器
// Boat boat[10];
vector<Boat> boat(5);

int money, boat_capacity, id;
char ch[N][N];
Grid gds(n, vector<int>(n, 1)); //全1地图

// deque<vector<Things>> things;
// vector<Things> cur_things;

map<int,vector<Things>> total_things;
// vector<Things> cur_things; // 当前帧的物品信息

bool compareByTransportTime(const Berth& a, const Berth& b);
void modifyGoalOfRobot(Robot& rbt, const int& curframe_id);
State NearestBerthOfRobotNow(const Robot& rbt);

void Init()
{
    for(int i = 1; i <= n; i ++) {// ��ͼ 200*200
        scanf("%s", ch[i] + 1);
        for(int j = 1; j <= n; j ++){
            if(ch[i][j] == '.' or ch[i][j] == 'B') {
                gds[i-1][j-1] = 0;
            }
        }
    }
    for(int i = 0; i < berth_num; i ++) // ��λ 10
    {
        int id;
        scanf("%d", &id); // ��λ��� 0-9
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed); // ��λ���Ͻ�����4*4 ����ʱ��֡�� װ���ٶ�ÿ֡װ����

        berth[id].berth_id = id;
        berth[id].num_in_berth = 0;    // 泊位的物品数量清零（初始化）
    }

    berth_order = berth; // 复制一份berth_order，用于排序
    sort(berth_order.begin(), berth_order.end(), compareByTransportTime);

    // // 设置5艘船的固定目标泊位为前五个泊位
    // for(int i = 0; i < 5; i ++) //判题器的泊位序号是泊位的id号
    // {
    //     boat[i].goal_berth = berth_order[i].berth_id;    // 每艘船的固定目标泊位
    //     // 船的目标泊位在第一帧input的时候被覆盖为-1 因为此时还没有下达移动命令，机器人目标泊位还处于-1状态
    //     // boat[i].goal = berth_order[i].Berth_id;          // 每艘船的目标泊位
    // }

    // 5艘船，每艘船负责2个泊位
    for(int i = 0; i < boat_num; i ++) //判题器的泊位序号是泊位的id号
    {
        boat[i].goal_berth_1 = berth_order[i].berth_id;    // 每艘船的固定目标泊位
        boat[i].goal_berth_2 = berth_order[i+5].berth_id;    // 每艘船的固定目标泊位

    }


    // // 10个机器人对应5个泊位
    // for(int i = 0;i<5;i++)
    // {
    //     robot[i].berthgoal = {berth_order[i%5].x, berth_order[i%5].y};  // 机器人的目标泊位是泊位的坐标state
    //     robot[i].berthgoal_id = berth_order[i%5].berth_id;  // 机器人的目标泊位的id

    // }
    
    // for(int i = 5;i<10;i++)
    // {

    //     robot[i].berthgoal = {berth_order[i%5].x+3, berth_order[i%5].y+3};
    //     robot[i].berthgoal_id = berth_order[i%5].berth_id;  // 机器人的目标泊位的id
    // }

    // 10个机器人对应10个泊位
    for(int i = 0;i<robot_num;i++)
    {
        robot[i].berthgoal = {berth_order[i].x, berth_order[i].y};  // 机器人的目标泊位是泊位的坐标state
        robot[i].berthgoal_id = berth_order[i].berth_id;  // 机器人的目标泊位的id

    }


    scanf("%d", &boat_capacity); // �����������װ����Ʒ��
    for(int i = 0;i<5;i++)
    {
        boat[i].num = 0; // 船的载货量初始化为0
    }

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

    total_things.insert(make_pair(id, cur_things)); // 加入到总的物品信息容器中

    // // 每帧的开始，分别在选出的5个泊位中建立一个存取物品地址的map
    // for(int i = 0; i < 5; i ++)
    // {
    //     berth[boat[i].goal_berth].choose_nearest_thing(total_things[id]);
    //     // cerr << berth[boat[i].goal_berth].things_map.size() << endl;
    // }

    // 每帧的开始，分别在10个泊位中建立一个存取物品地址的map
    for(int i = 0; i < berth_num; i ++)
    {
        berth[i].choose_nearest_thing(total_things[id]);
        // cerr << berth[boat[i].goal_berth].things_map.size() << endl;
    }

    for(int i = 0; i < robot_num; i ++) // 
    {
        // int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].pos.first, &robot[i].pos.second, &robot[i].status); // 
    }
    for(int i = 0; i < 5; i ++) // 
    {
        scanf("%d%d\n", &boat[i].status, &boat[i].goal); //
    }

    char okk[100];
    scanf("%s", okk); //
    return id;
}


// berth比较函数
bool compareByTransportTime(const Berth& a, const Berth& b) {
    return a.transport_time < b.transport_time;
}

void modifyGoalOfRobot(Robot& rbt, const int& curframe_id) {
    if(rbt.pos == rbt.goal or rbt.stop_flag > 0){ // 到达，或不正常停止
        // if(rbt.goal == rbt.berthgoal){ // 到达泊位，获取新的物品位置
        if (rbt.stop_flag == 1 or
            (rbt.pos.first >= berth[rbt.berthgoal_id].x 
            and rbt.pos.first <= berth[rbt.berthgoal_id].x + 3 
            and rbt.pos.second >= berth[rbt.berthgoal_id].y 
            and rbt.pos.second <= berth[rbt.berthgoal_id].y + 3)){
            // cerr<<"before gei   "<<"berth_map_size  "<< berth[rbt.berthgoal_id].things_map.size() << endl;
            berth[rbt.berthgoal_id].judge_occupy_timeout(curframe_id);

            rbt.goal = {berth[rbt.berthgoal_id].nearest_thing.x, berth[rbt.berthgoal_id].nearest_thing.y};
            
            berth[rbt.berthgoal_id].things_map.begin()->second->to_robot = 1; // 物品设定为已占用
            berth[rbt.berthgoal_id].things_map.erase(berth[rbt.berthgoal_id].things_map.begin()); // 更新泊位的最近物品
            berth[rbt.berthgoal_id].update_nearest_thing_from_history(); // 更新泊位的最近物品
            // cerr<<"after gei    "<<"berth_map_size  "<< berth[rbt.berthgoal_id].things_map.size() << endl;
            // cerr << "pos " << rbt.pos.first << "," << rbt.pos.second << endl;
            // cerr << "new goal " << rbt.goal.first << "," << rbt.goal.second << endl;
        }
        else if(rbt.stop_flag == 2){ // 第二种停止情况

            rbt.goal = NearestBerthOfRobotNow(rbt);
            // rbt.goal = rbt.berthgoal; // 暂时换一个泊位，可以计算除了原来泊位之外最近的指定泊位
        }
        else{ // 到达物品，返回泊位
            rbt.goal = rbt.berthgoal;
        }
        rbt.stop_flag = 0;
    }
    //未到达泊位，不进行操作
}

State NearestBerthOfRobotNow(const Robot& rbt){
    // int min_dis = _INF_;
    // State min_state;
    // for (size_t i = 0; i < 5; i++)
    // {
    //     Berth tempBerth = berth_order[i];
    //     if(rbt.berthgoal_id != tempBerth.berth_id){
    //         int temp_dis = manhattanDistance(rbt.pos.first, rbt.pos.second, tempBerth.x, tempBerth.y);
    //         if(temp_dis < min_dis){
    //             min_dis = temp_dis;
    //             min_state = {tempBerth.x, tempBerth.y};
    //         }
    //     }
    // }
    // rbt.berthgoal_id = min_id;
    // return min_state;
    Berth temp_b = berth_order[rand()%5];
    return {temp_b.x, temp_b.y}; 
}
