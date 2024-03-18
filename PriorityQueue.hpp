
#ifndef __DSTARLITE_PRIORITYQUEUE_HPP__
#define __DSTARLITE_PRIORITYQUEUE_HPP__

#include <vector>
#include <utility>
#include <algorithm>

using std::make_heap;
using std::pair;
using std::pop_heap;
using std::push_heap;
using std::vector;
using std::deque;

template<class Tp, class Te> class PriorityQueue
{
 public:

  pair<Tp, Te> top() const
  {
    return data.front();  // return {priority, elem} pair
  }

  Tp topKey() const
  {
    return data.front().first;  // return priority only;
  }

  int size() const
  {
    return data.size();
  }

  bool empty() const
  {
    return data.empty();
  }

  pair<Tp, Te> pop()
  {
    std::pop_heap(data.begin(), data.end(), std::greater<pair<Tp, Te>> {});
    pair<Tp, Te> temp = data.back();
    data.pop_back();
    return temp;
  }

  void push(const Te &elem, const Tp &priority)
  {
    data.push_back({priority, elem});
    std::push_heap(data.begin(), data.end(), std::greater<pair<Tp, Te>> {});
  }

  void update(const Te &elem, const Tp &priority)
  {
    auto &&it = find(elem);
    if (it == data.end())
    {
      push(elem, priority);
      return;
    }

    auto &[p, item] = *it;

    if (priority == p)
      return;
    p = priority; // 优先级不同则更新优先级
    std::make_heap(data.begin(), data.end(), std::greater<pair<Tp, Te>> {});
  }

  void remove(const Te &elem)
  {
    auto &&it = find(elem);
    if (it == data.end())
      return;

    data.erase(it);
    std::make_heap(data.begin(), data.end(), std::greater<pair<Tp, Te>> {});
  }

 private:
  vector<pair<Tp, Te>> data;

  typename vector<pair<Tp, Te>>::iterator find(const Te &elem)
  {
    for (auto it = data.begin(); it != data.end(); it++)
    {
      if (elem == it->second)
      {
        return it;
      }
    }
    return data.end();
  }
};

#endif /* __DSTARLITE_PRIORITYQUEUE_HPP__ */