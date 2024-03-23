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

// Robot robot[robot_num + 10];
vector<shared_ptr<Robot>> robot(robot_num);
map<State, shared_ptr<Robot>> robot_noid;
// Berth berth[berth_num + 10];
vector<Berth> berth(10);
vector<Berth> berth_order(10);  //对berth进行排序后的容器
vector<Berth> berth_order_num_and_trans(10);  //对berth进行排序后的容器，按照num和transportime排序
// Boat boat[10];
vector<Boat> boat(5);

int boat_goalberth_flag = 0;   // 是否给船设置了目标泊位，0未设置，1已设置
int money, boat_capacity, id;
char ch[N][N];
Grid gds(n, vector<int>(n, 1)); //全1地图
Grid gds_norobot(n, vector<int>(n, 1)); // 1静态地图（机器人不作为障碍）
// deque<vector<Things>> things;
// vector<Things> cur_things;

bool compareByTransportTime(const Berth& a, const Berth& b);

bool modifyGoalOfRobot(shared_ptr<Robot>& rbt, const int& curframe_id);
State NearestBerthOfRobotNow(shared_ptr<Robot>& rbt);
void findAbleBerth(shared_ptr<Robot>& rbt);

// bool compareBy_num_and_trans(const Berth& a, const Berth& b);


void Init()
{
    for(int i = 1; i <= n; i ++) {// ��ͼ 200*200
        scanf("%s", ch[i] + 1);
        for(int j = 1; j <= n; j ++){
            if(ch[i][j] == '.' or ch[i][j] == 'B') {
                gds[i-1][j-1] = 0;
                gds_norobot[i-1][j-1] = 0;
            }
            if(ch[i][j] == 'A') {
                gds_norobot[i-1][j-1] = 0; // 机器人也设为可通行，用于静态地图
                robot_noid.insert(make_pair(make_pair(i-1, j-1), make_shared<Robot>(i-1, j-1)));
            }
        }
    }
    for(int i = 0; i < berth_num; i ++) // ��λ 10
    {
        int id;
        scanf("%d", &id); // berth id
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed); // berth 位置 运输时间 装载速度
        berth[id].berth_id = id;
        berth[id].num_in_berth = 0;    // 泊位的物品数量清零（初始化）
    }

    // 刘：我感觉最好不要对原berth的输入顺序打乱，复制一个新的泊位容器，对新容器排序打乱之后，通过每个元素的id 对应原容器的序号
    berth_order = berth; // 复制一份berth_order，用于排序
    berth_order_num_and_trans = berth; // 复制一份berth，用于排序，按照valuerate和transportime排序
    sort(berth_order.begin(), berth_order.end(), compareByTransportTime);
    // 根据transport_time对vector<Berth>berth排序   排序后的vector元素序号和Berth_id不一致
    // sort(berth.begin(), berth.end(), compareByTransportTime);

    // // 设置5艘船的固定目标泊位为前五个泊位
    // for(int i = 0; i < 5; i ++) //判题器的泊位序号是泊位的id号
    // {
    //     boat[i].goal_berth = berth_order[i].berth_id;    // 每艘船的固定目标泊位
    //     // 船的目标泊位在第一帧input的时候被覆盖为-1 因为此时还没有下达移动命令，机器人目标泊位还处于-1状态
    // }

    for (auto it = robot_noid.begin(); it != robot_noid.end(); ++it)
    {
        for(int i = 0; i < 3; ++i){ // 可调整，若初始化超过5s，则减少该值，值大可减少后续判断
            it->second->berthgoal_id = berth_order[i].berth_id; // 给定初始berthid
            it->second->berthgoal = {berth_order[i].x, berth_order[i].y};

            if(it->second->isBerthAble()) // 第一次默认可以，plan是为了获取路径，保存能否通路的信息
                it->second->Plan_to_Berth(gds_norobot);
        }
    }
    
    int num = 0; // 10个机器人分别对应一个泊位
    for (auto it = robot_noid.begin(); it != robot_noid.end(); ++it)
    {
        it->second->berthgoal_id = berth_order[num].berth_id; // 给定初始berthid
        it->second->berthgoal = {berth_order[num].x, berth_order[num].y};

        if(it->second->isBerthAble()){ // 如果berth可以前往，则plan
            it->second->Plan_to_Berth(gds_norobot);
            it->second->goal = it->second->berthgoal; // 规划好了，直接把目标设为泊位
            it->second->plan_ready = 1; // 规划完成标志
        }
        else{
            it->second->goal = it->second->pos; // berth不可行，则规划路径不通，令目标设为位置，当作上一次行动结束，后续规划
            // it->second->plan_ready = 0; //
        }
        ++num;
    }
    
    scanf("%d", &boat_capacity); // 载货量

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
    // vector<Things> cur_things; // 当前帧的物品信息
    vector<shared_ptr<Things>> cur_things; // 当前帧的物品信息

    //从第1帧开始 从1开始递增
    scanf("%d%d", &id, &money); // 帧数，已经获取的money
    //当前帧的物品信息cur_things在每一帧填入前清空

    int num;
    scanf("%d", &num); // ������������0-10
    for(int i = 1; i <= num; i ++) // ����������Ϣ
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val); // λ�� ���<=1000

        // Things thing(id, x, y, val); //创建一个物品对象
        auto thing = make_shared<Things>(id, x, y, val); //创建一个物品对象的shared_ptr
        
        // things_map.insert(make_pair(id, thing)); // 加入到things_map中
        // cur_things.push_back({{x, y}, val});

        cur_things.push_back(thing); // 加入到当前帧的物品信息
    }


    //遍历选出的10个泊位 并选择每个泊位在当前帧以及之前帧中最近的物品
    for(int i = 0; i < 10; i ++)
    {
        berth[i].choose_nearest_thing(cur_things); // 选出当前帧最近的物品
        berth[i].choose_nearest_thing();
        // cerr << berth[boat[i].goal_berth].things_map.size() << endl;
    }
    for(auto it = Berth::things_map_reok.begin(); it != Berth::things_map_reok.end(); ++it){
        it->second->to_robot = 0;
    }
    Berth::things_map_reok.clear(); // 清空


    // //当前帧的物品信息cur_things（已经去除了5个被锁定的物品）加入到things_map中
    // things_map.insert(make_pair(id, cur_things));

    for(int i = 0; i < robot_num; i ++) // 
    {
        State temp_pos;
        int temp_goods;
        int temp_status;
        scanf("%d%d%d%d", &temp_goods, &temp_pos.first, &temp_pos.second, &temp_status); //
        if(id == 1){
            robot[i] = robot_noid[temp_pos];
            robot[i]->robot_id = i;
        }

        // 状态写入
        robot[i]->pos = temp_pos;
        robot[i]->goods = temp_goods;
        robot[i]->status = temp_status;

        // scanf("%d%d%d%d", &robot[i].goods, &robot[i].pos.first, &robot[i].pos.second, &robot[i].status); // 
    }
    for(int i = 0; i < 5; i ++) // 
    {
        scanf("%d%d\n", &boat[i].status, &boat[i].goal); //
    }

    char okk[100];
    scanf("%s", okk); //
    return id;
}


// berth比较函数    通过运输时间排序
bool compareByTransportTime(const Berth& a, const Berth& b) {
    return a.transport_time < b.transport_time;
}

// berth比较函数    通过价值和运输时间排序
bool compareBy_num_and_trans(const Berth& a, const Berth& b)
{
    // double ratioA= a.totalnum_tillnow/a.transport_time;
    // double ratioB= b.totalnum_tillnow/b.transport_time;
    // return ratioA > ratioB;
    return a.totalnum_tillnow > b.totalnum_tillnow;
}


bool modifyGoalOfRobot(shared_ptr<Robot>& rbt, const int& curframe_id) {
    if(rbt->pos == rbt->goal or rbt->stop_flag > 0 or !rbt->isBerthAble()){ // 到达，或不正常停止
        // if(rbt->goal == rbt->berthgoal){ // 到达泊位，获取新的物品位置
        if (rbt->stop_flag == 1 or
        (rbt->pos.first >= berth[rbt->berthgoal_id].x 
        and rbt->pos.first <= berth[rbt->berthgoal_id].x + 3 
        and rbt->pos.second >= berth[rbt->berthgoal_id].y 
        and rbt->pos.second <= berth[rbt->berthgoal_id].y + 3)){
            
            if(rbt->thing_flag == -1)
                rbt->thing_flag = 0; // 无目标物品
            else if(rbt->thing_flag == 3){
                // 其他berth
                rbt->thing_flag = 0;
            }
            
            if(!berth[rbt->berthgoal_id].judge_occupy_timeout(curframe_id)){
                // cerr << "judge false" << endl;
                return false;
            }
            // rbt->goal = {berth[rbt->berthgoal_id].nearest_thing.x, berth[rbt->berthgoal_id].nearest_thing.y};
            rbt->goal = {berth[rbt->berthgoal_id].nearest_thing->x, berth[rbt->berthgoal_id].nearest_thing->y};
            rbt->curthing_value = berth[rbt->berthgoal_id].nearest_thing->value; // 记录当前物品的价值
            rbt->thing_flag = 1; // 有目标物品，但未确定是否可达

            auto it = berth[rbt->berthgoal_id].things_map.begin();
            it->second->to_robot = 1; // 物品设定为已占用

            //
            // 容器保存占用的物品
            Berth::things_map_record.insert(make_pair(rbt->robot_id, make_pair(rbt->berthgoal_id, it->second)));
            //

            berth[rbt->berthgoal_id].things_map.erase(it); // 更新泊位的最近物品
            berth[rbt->berthgoal_id].update_nearest_thing_from_history(); // 更新泊位的最近物品
            // cerr << "pos " << rbt->pos.first << "," << rbt->pos.second << endl;
            // cerr << "new goal " << rbt->goal.first << "," << rbt->goal.second << endl;
        }
        else if(rbt->stop_flag == 2){ // 第二种停止情况
            rbt->goal = NearestBerthOfRobotNow(rbt);
            // rbt->goal = rbt->berthgoal; // 暂时换一个泊位，可以计算除了原来泊位之外最近的指定泊位
        }
        else{ // 到达物品，返回泊位
            if(rbt->thing_flag == 2){
                rbt->thing_flag = -1;
            }
            
            if(rbt->isBerthAble()){
                rbt->goal = rbt->berthgoal;
            }
            else if(rbt->berth_id_able == vector<int>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}){ // 泊位全不可用
                rbt->cantgo = 1;
                gds_norobot[rbt->pos.first][rbt->pos.second] = 1; // 机器人位置设为障碍
            }
            else{
                findAbleBerth(rbt);
                rbt->goal = rbt->berthgoal;
            }
        }
        rbt->stop_flag = 0;
    }
    //未到达泊位，不进行操作
    return true;
}

State NearestBerthOfRobotNow(shared_ptr<Robot>& rbt) {
    // int min_dis = _INF_;
    // State min_state;
    // for (size_t i = 0; i < 5; i++)
    // {
    //     Berth tempBerth = berth_order[i];
    //     if(rbt->berthgoal_id != tempBerth.berth_id){
    //         int temp_dis = manhattanDistance(rbt->pos.first, rbt->pos.second, tempBerth.x, tempBerth.y);
    //         if(temp_dis < min_dis){
    //             min_dis = temp_dis;
    //             min_state = {tempBerth.x, tempBerth.y};
    //         }
    //     }
    // }
    // rbt->berthgoal_id = min_id;
    // return min_state;
    Berth temp_b;
    while(true){
        temp_b = berth_order[rand()%10];
        // rbt->berthgoal_id = temp_b.berth_id;
        if(rbt->isBerthAble()) break;
        if(rbt->berth_id_able == vector<int>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}){
            rbt->cantgo = 1;
            break;
        }
    }
    return {temp_b.x+2, temp_b.y+2};
}

void findAbleBerth(shared_ptr<Robot>& rbt) {
    // int min_dis = _INF_;
    // State min_state;
    // for (size_t i = 0; i < 5; i++)
    // {
    //     Berth tempBerth = berth_order[i];
    //     if(rbt->berthgoal_id != tempBerth.berth_id){
    //         int temp_dis = manhattanDistance(rbt->pos.first, rbt->pos.second, tempBerth.x, tempBerth.y);
    //         if(temp_dis < min_dis){
    //             min_dis = temp_dis;
    //             min_state = {tempBerth.x, tempBerth.y};
    //         }
    //     }
    // }
    // rbt->berthgoal_id = min_id;
    // return min_state;
    while(true){
        Berth temp_b = berth_order[rand()%10];
        rbt->berthgoal_id = temp_b.berth_id;
        rbt->berthgoal = {temp_b.x+1, temp_b.y+1};
        if(rbt->isBerthAble()) break;
    }
}



void tenberth_to_fiveberth()
{
    for(int i = 0;i<5;i++)
    {
        robot[i]->berthgoal = {berth_order_num_and_trans[i%5].x, berth_order_num_and_trans[i%5].y};  // 机器人的目标泊位是泊位的坐标state
        robot[i]->berthgoal_id = berth_order_num_and_trans[i%5].berth_id;  // 机器人的目标泊位的id

    }
    
    for(int i = 5;i<10;i++)
    {

        robot[i]->berthgoal = {berth_order_num_and_trans[i%5].x+3, berth_order_num_and_trans[i%5].y+3};
        robot[i]->berthgoal_id = berth_order_num_and_trans[i%5].berth_id;  // 机器人的目标泊位的id
    }
}