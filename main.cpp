#include <bits/stdc++.h>
using namespace std;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;

struct Robot
{
    int x, y, goods;
    int status;
    int mbx, mby;
    Robot() {}
    Robot(int startX, int startY) {
        x = startX;
        y = startY;
    }
}robot[robot_num + 10];

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
}berth[berth_num + 10];

struct Boat
{
    int num, pos, status;
}boat[10];

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

int main()
{
    Init(); //初始化
    for(int zhen = 1; zhen <= 15000; zhen ++) // 按帧处理
    {
        int id = Input(); // 读取场面信息 id第几帧
        
        for(int i = 0; i < robot_num; i ++){
        
        // 机器人指令
            if(robot[i].status == 1){
                printf("move %d %d\n", i, 1);
            // printf("move %d %d\n", i, rand() % 4); //指令：move 机器人id0-9  移动方向右左上下0123
                if(robot[i].goods == 0)
                    printf("get %d\n", i); //指令：get 机器人id0-9  取货
                else
                    printf("put %d\n", i); //指令：put 机器人id0-9  放货
            }
                
            // printf("move %d %d\n", 3, 1);
            // printf("move %d %d\n", 7, 1);

            // get/pull id；货物生成处未携带物品则取货成功/泊位处且携带物品则放置成功
        //船指令
            // ship id0-4 泊位id0-9 船移动到泊位
            // go id0-4 船运输货物到虚拟点
        }
        
        puts("OK"); // 所有指令结束后OK
        fflush(stdout);
    }

    return 0;
}
