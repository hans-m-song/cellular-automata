#ifndef GRID_H
#define GRID_H

#include <utility>
#include <vector>

typedef std::pair<int, int> Point;
typedef std::vector<int> Vector;
typedef std::vector<Vector> Vector2D;

class Grid {
  int width;
  int height;
  int age;

  Vector2D space;
  Point new_point(void);
  Point new_point(int x, int y);

  public:
  Grid(int width, int height);
  ~Grid();
  void tick(void);
  void print(void);
};

#endif