#ifndef GRID_H
#define GRID_H

#include <utility>

typedef std::pair<int, int> Point;

class Grid {
  int width;
  int height;
  int age;

  int** space;
  Point new_point(void);
  Point new_point(int x, int y);

  public:
  Grid(int width, int height);
  ~Grid();
  void tick(void);
};

#endif