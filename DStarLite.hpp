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

#include "DStarLite.hpp"

DStarLite::DStarLite(const pair<int, int> &dim, const State &start, const State &goal) :
    _map {Grid(dim.first, vector<int>(dim.second, 0))}, s_start {start}, s_goal {goal},
    s_current {start}, _g {Grid(dim.first, vector<int>(dim.second, _INF_))},
    _rhs {Grid(dim.first, vector<int>(dim.second, _INF_))}
{
  rhs(s_goal) = 0;
  U.push(s_goal, {heuristic(s_start, s_goal), 0});

  computeShortestPath();
}

DStarLite::DStarLite(const Grid &map, const pair<int, int> &dim, const State &start, const State &goal) :
    _map {map}, s_start {start}, s_goal {goal},
    s_current {start}, _g {Grid(dim.first, vector<int>(dim.second, _INF_))},
    _rhs {Grid(dim.first, vector<int>(dim.second, _INF_))}
{
  rhs(s_goal) = 0;
  U.push(s_goal, {heuristic(s_start, s_goal), 0});

  computeShortestPath();
}

void DStarLite::computeShortestPath()
{
  while (U.size())
  {
    auto [kOld, u] = U.top();
    if ((kOld >= calculateKey(s_start)) and (rhs(s_start) == g(s_start)))
      break; //已找到最佳路线

    U.pop();
    auto kNew = calculateKey(u);

    if (kOld < kNew)
    {
      U.push(u, kNew); //k变大 优先级降低重新放回队列
      continue;
    }

    if (g(u) > rhs(u))
    {
      g(u) = rhs(u); // 成本下降，更新G
    }
    else
    {
      g(u) = _INF_;
      updateVertex(u); //成本上升，更新节点信息
    }
    for (const auto &neighbor: neighborStates(u))
    {
      updateVertex(neighbor); // 更新邻居节点信息
    }
  }
}

void DStarLite::updateVertex(const State &s)
{
  if (s != s_goal) // 中间节点
  {
    rhs(s) = computeRHS(s); //更新当前节点rhs
  }

  if (g(s) != rhs(s)) // rhs有改变
  {
    U.update(s, calculateKey(s)); // 加入优先队列
  }
  else
  {
    U.remove(s); // rhs无改变，移出优先队列，意味着该节点未受影响
  }
}

int DStarLite::computeRHS(const State &s) const
{
  int RHS = _INF_;
  for (const auto &neighbor: neighborStates(s))
  {
    RHS = std::min(RHS, g(neighbor) + cost(s, neighbor));
  }
  return RHS;
}

Key DStarLite::calculateKey(const State &s) const
{
  auto min = std::min(g(s), rhs(s));
  return {min + heuristic(s_start, s) + km, min};
}

int DStarLite::heuristic(const State &s1, const State &s2) const
{
  auto diff = s1 - s2;
  return std::abs(diff.first) + std::abs(diff.second);
  // return std::sqrt(std::pow(diff.first, 2) + std::pow(diff.second, 2));
}

int DStarLite::cost(const State &s1, const State &s2) const
{
  return (map(s1) or map(s2)) ? _INF_ : 1;
}

vector<State> DStarLite::neighborStates(const State &s) const
{
  vector<State> neighbors;
  for (const auto &a: actions)
  {
    auto nextState = s + a;

    if (nextState.first < 0 or nextState.second < 0
        or static_cast<size_t>(nextState.first) >= _map.size()
        or static_cast<size_t>(nextState.second) >= _map[0].size())
      continue; //超出边界则下一个点

    if (map(nextState))
      continue; //障碍物则下一个点

    neighbors.push_back(nextState);
  }
  return neighbors;
}

deque<State> DStarLite::getPath() const
{
  deque<State> path = {s_current};
  State s            = s_current;

  while (s != s_goal) // 一直循环直到终点
  {
    State s_next = peekNext(s);
    if (s_next == s)
      break; // 下一位置==当前遍历位置，终止循环
    s = s_next; // 更新当前遍历位置
    path.push_back(s); // 加入路径
  }

  return path;
}

State DStarLite::peekNext(const State &s) const
{
  if (s == s_goal)
    return s; // 当前位置为终点则返回当前位置

  State s_min = s; // 初始化最小值为当前位置
  for (const auto &neighbor: neighborStates(s))
  {
    if (g(neighbor) < g(s_min))
    {
      s_min = neighbor; // 更新最小G值的邻居位置，作为下一个位置
    }
  }
  return s_min;
}

State DStarLite::moveNext()
{
  s_current = peekNext(s_current); // 更新当前机器人位置
  return s_current;
}

void DStarLite::toggleCell(const State &s)
{
  // boundary checks
  if (s.first < 0 or s.second < 0 or static_cast<size_t>(s.first) >= _map.size()
      or static_cast<size_t>(s.second) >= _map[0].size())
    return;

  // current state and goal states cannot be occupied
  if (s == s_goal or s == s_current)
    return;

  km += heuristic(s_start, s_current);
  s_start = s_current;

  map(s) = 1 - map(s);
  updateVertex(s);
  for (const auto &neighbor: neighborStates(s))
  {
    updateVertex(neighbor);
  }

  computeShortestPath();
}

void DStarLite::blockCell(const State &u)
{
  if (map(u))
  {
    return;
  }
  else
  {
    toggleCell(u);
  }
}

void DStarLite::clearCell(const State &u)
{
  if (map(u))
  {
    toggleCell(u);
  }
  else
  {
    return;
  }
}

void DStarLite::updateMap(const Grid &newMap)
{
  if (newMap.size() != _map.size() or newMap[0].size() != _map[0].size())
    return;

  for (size_t i = 0; i < _map.size(); i++)
  {
    for (size_t j = 0; j < _map[0].size(); j++)
    {
      if (_map[i][j] != newMap[i][j])
      {
        toggleCell({i, j});
      }
    }
  }
}

int DStarLite::g(const State &s) const
{
  return _g[s.first][s.second];
}

int &DStarLite::g(const State &s)
{
  return _g[s.first][s.second];
}

int DStarLite::rhs(const State &s) const
{
  return _rhs[s.first][s.second];
}

int &DStarLite::rhs(const State &s)
{
  return _rhs[s.first][s.second];
}

int DStarLite::map(const State &s) const
{
  return _map[s.first][s.second];
}

int &DStarLite::map(const State &s)
{
  return _map[s.first][s.second];
}

#endif /* __DSTARLITE_DSTARLITE_HPP__ */