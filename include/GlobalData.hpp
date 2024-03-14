#pragma once
#include <bits/stdc++.h>
#include "Robot.hpp"
#include "Berth.hpp"
#include "Boat.hpp"
using namespace std;

typedef std::pair<int, int> State; // 位置坐标，x，y
typedef std::pair<State, int> Things; // 货物State（x, y）， 值value
typedef std::vector<vector<int>> Grid; //二维vector，地图值？

const int n = 200;
const int robot_num = 10;
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

deque<vector<Things>> things;
vector<Things> cur_things;

bool compareByTransportTime(const Berth& a, const Berth& b);

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
        berth[id].Berth_id = id;
        berth[id].num_in_berth = 0;    // 泊位的物品数量清零（初始化）
    }

    // 刘：我感觉最好不要对原berth的输入顺序打乱，复制一个新的泊位容器，对新容器排序打乱之后，通过每个元素的id 对应原容器的序号
    berth_order = berth; // 复制一份berth_order，用于排序
    sort(berth_order.begin(), berth_order.end(), compareByTransportTime);
    // 根据transport_time对vector<Berth>berth排序   排序后的vector元素序号和Berth_id不一致
    // sort(berth.begin(), berth.end(), compareByTransportTime);

    // 设置5艘船的固定目标泊位为前五个泊位
    for(int i = 0; i < 5; i ++) //判题器的泊位序号是泊位的id号
    {
        boat[i].goal_berth = berth_order[i].Berth_id;    // 每艘船的固定目标泊位
        // 船的目标泊位在第一帧input的时候被覆盖为-1 因为此时还没有下达移动命令，机器人目标泊位还处于-1状态
        // boat[i].goal = berth_order[i].Berth_id;          // 每艘船的目标泊位
    }



    for(int i = 0;i<5;i++)
    {
        robot[i].berthgoal = {berth_order[i%5].x, berth[i%5].y};  // 机器人的目标泊位是泊位的坐标state
        robot[i].berthgoal_id = berth_order[i%5].Berth_id;  // 机器人的目标泊位的id
    }
    
    for(int i = 5;i<10;i++)
    {
        robot[i].berthgoal = {berth_order[i%5].x+3, berth[i%5].y+3};
        robot[i].berthgoal_id = berth_order[i%5].Berth_id;  // 机器人的目标泊位的id
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
    //从第1帧开始 从1开始递增
    scanf("%d%d", &id, &money); // 帧数，已经获取的money
    //当前帧的物品信息cur_things在每一帧填入前清空
    cur_things.clear();
    int num;
    scanf("%d", &num); // ������������0-10
    for(int i = 1; i <= num; i ++) // ����������Ϣ
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val); // λ�� ���<=1000
        cur_things.push_back({{x, y}, val});
    }
    if(things.size() == 20)     //改为1000帧 后面就全部计算距离，相同思路
    {
        things.pop_front();
    }

    things.push_back(cur_things);

    for(int i = 0; i < robot_num; i ++) // ��������Ϣ
    {
        // int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].pos.first, &robot[i].pos.second, &robot[i].status); // �Ƿ�Я����Ʒ01 ���� ״̬0�ָ�1����
    }
    for(int i = 0; i < 5; i ++) // 5���� i��Ӧ��id0-4
    {
        scanf("%d%d\n", &boat[i].status, &boat[i].goal); // ״̬0�ƶ�(����)1����װ��/�������  Ŀ�겴λ����������-1��
    }


    
    char okk[100];
    scanf("%s", okk); //����OK
    return id;
}


// berth比较函数
bool compareByTransportTime(const Berth& a, const Berth& b) {
    return a.transport_time < b.transport_time;
}




