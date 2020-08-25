#ifndef GRID_H
#define GRID_H

#include <utility>
#include <vector>

typedef std::pair<int, int> Point;
typedef std::vector<int> Vector;
typedef std::vector<Vector> Vector2D;
enum Direction { N, NE, E, SE, S, SW, W, NW };

class Grid {
  int width;
  int height;
  Vector2D space;
  Vector2D next_space;
  Point empty_cell(void);
  int sum_neighbour(int x, int y);
  Vector2D allocate_space(void);
  Point apply_direction(Point origin, Direction direction);

  public:
  Grid(int width, int height, double initial_density);
  void print(void);
  void tick(void);
  int sum(void);
};

#endif