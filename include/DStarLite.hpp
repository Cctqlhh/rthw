/**
 * @file DstarLite.hpp
 * @author Mohammad Abdussalam (mohammadahmad01722@gmail.com)
 * @brief Implementation of D* Lite algorithm.
 * @see http://idm-lab.org/bib/abstracts/papers/aaai02b.pdf
 * @version 0.1
 * @date 2022-12-22
 */

#ifndef __DSTARLITE_DSTARLITE_HPP__
#define __DSTARLITE_DSTARLITE_HPP__

#include <vector>
#include <deque>
#include <utility>
#include <algorithm>
#include <cmath>

#include "PriorityQueue.hpp"
#include "util.hpp"

#define _INF_ 0x2fffffff  // slightly less than INT32_MAX to avoid overflows

typedef std::pair<int, int> State; // 位置坐标，x，y
typedef std::pair<int, int> Key; // KEY
typedef std::vector<vector<int>> Grid; //二维vector，地图值？

class DStarLite
{
 public:
  DStarLite(){}
  DStarLite(const pair<int, int> &dim, const State &start, const State &goal);
  DStarLite(const Grid &map, const pair<int, int> &dim, const State &start, const State &goal);
  // vector<State> getPath() const; //获取路径，返回坐标vector
  deque<State> getPath() const; //获取路径，返回坐标deque
  State peekNext(const State &s) const; //返回下一个位置
  State moveNext(); //移动到下一个位置
  void toggleCell(const State &u); //切换位置状态
  void blockCell(const State &u); //阻塞障碍物
  void clearCell(const State &u); //清除障碍物
  void updateMap(const Grid &newMap); //更新地图
  State goal() const { return s_goal; }; //返回目标位置

 private:
  Grid _map; //存储网格状态，0为可通行，1为障碍物
  State s_start {0, 0};    // s_start is the robot start location in the current plan 机器人起始位置
  State s_goal {0, 0};     // s_goal is the goal location 机器人目的位置
  State s_current {0, 0};  // s_current is the current robot location 机器人当前位置
  Grid _g;    // The g_value is an estimate of the distance to goal distance from each location 值G
  Grid _rhs;  // The rhs values are one-step lookahead values based on the g-values 值rhs
  PriorityQueue<Key, State> U; //优先队列，存放要处理的节点，其KEY和位置坐标
  int km {0}; //内部计数器，用于更新优先队列，修饰Key，改变Key的值
  vector<State> actions {{0, 1}, {0, -1}, {-1, 0}, {1, 0}}; //移动 右左上下0123
  // vector<State> actions {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

  void computeShortestPath(); // 计算最短路径
  void updateVertex(const State &u); // 更新节点u信息
  int computeRHS(const State &u) const; // 计算节点u的rhs值
  Key calculateKey(const State &s) const; // 计算节点s的Key值，用于优先队列
  int heuristic(const State &s1, const State &s2) const; // 启发式评估节点s1到s2的成本
  int cost(const State &s1, const State &s2) const; // 评估节点s1到s2的成本
  vector<State> neighborStates(const State &s) const; // 返回节点s的邻居节点

  int map(const State &s) const;
  int &map(const State &s);
  int g(const State &s) const;
  int &g(const State &s);
  int rhs(const State &s) const;
  int &rhs(const State &s);
};

#endif /* __DSTARLITE_DSTARLITE_HPP__ */