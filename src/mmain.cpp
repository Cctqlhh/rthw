// #include <SFML/Window.hpp>
// #include <SFML/Graphics.hpp>

// 标准库
#include <utility>
#include <algorithm>
#include <vector>
// 算法类头文件
#include "DStarLite.hpp"
// 处理网格单元类
// #include "cell_grid.hpp"

#define r 200  // number of rows in the grid行数
#define c 200  // number of cols in the grid列数

#define l 20  // length of grid cell in pixels
#define th 2  // thickness of cell outline in pixels

const std::pair<int, int> START = {r / 2, c / 4}; // 起点
const std::pair<int, int> GOAL  = {r / 2, 3 * c / 4}; // 终点

constexpr int w = th + c * (l + th);  // window width
constexpr int h = th + r * (l + th);  // window height

std::pair<int, int> mouse_get_cell(const sf::RenderWindow &window)
{
  auto mouse_pos = sf::Mouse::getPosition(window);
  int i          = (mouse_pos.y - 0.5 * th) / (l + th);
  int j          = (mouse_pos.x - 0.5 * th) / (l + th);

  i = std::clamp(i, 0, r - 1);
  j = std::clamp(j, 0, c - 1);

  return {i, j};
}

int main(void)
{
  DStarLite dsl({r, c}, START, GOAL); //初始换算法类对象， 地图行列，起点，终点
  CellGrid cg({r, c}, l, th, START, GOAL);

  // create the window
  sf::RenderWindow window(sf::VideoMode(w, h), "D*-Lite", sf::Style::Titlebar | sf::Style::Close);
  window.setFramerateLimit(24); //帧率限制？SFML的不重要
  window.clear(sf::Color::Green);

  // run the program as long as the window is open
  while (window.isOpen())
  {
    // check all the window's events that were triggered since the last iteration of the loop
    sf::Event event;
    while (window.pollEvent(event)) //事件循环
    {
      // "close requested" event: we close the window
      if (event.type == sf::Event::Closed) //关闭窗口
      {
        window.close();
      }

      if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) //鼠标左键按下
      {
        auto loc = mouse_get_cell(window);

        cg.blockCell(loc); //阻塞网格单元
        dsl.blockCell(loc); //阻塞网格单元
      }

      if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) //鼠标右键按下
      {
        auto loc = mouse_get_cell(window);

        cg.clearCell(loc); //清除网格单元
        dsl.clearCell(loc); //清除网格单元
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) //键盘R键按下
      {
        for (int i = 0; i < r; i++)
        {
          for (int j = 0; j < c; j++)
          {
            cg.clearCell({i, j}); //清除所有网格单元
            dsl.clearCell({i, j}); //清除所有网格单元
          }
        }
      }
    }
    // 事件结束时
    cg.setPath(dsl.getPath()); // 设置路径
    cg.draw(window); // 绘制网格

    // end the current frame
    window.display(); //显示窗口
  }

  return 0;
}