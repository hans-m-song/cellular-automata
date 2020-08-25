#include "grid.hpp"
#include "util.hpp"
#include <algorithm>
#include <iostream>
#include <stdlib.h>

Grid::Grid(int width, int height, double initial_density)
    : width(width), height(height) {

  space = allocate_space();

  int initial_cells = (int)(initial_density * width * height);
  Point point;
  for (int i = 0; i < initial_cells; i++) {
    point = empty_cell();
    space[point.first][point.second] = 1;
  }
}

Vector2D Grid::allocate_space(void) {
  Vector2D space = Vector2D(width);
  for (int x = 0; x < width; x++) {
    space[x] = Vector(height);
  }
  return space;
}

Point Grid::empty_cell(void) {
  int x_bound = width - 1;
  int y_bound = height - 1;
  int max = x_bound * y_bound;
  int x, y;
  for (int i = 0; i < max; i++) {
    x = random(0, width - 1);
    y = random(0, height - 1);
    if (!space[x][y]) {
      return Point(x, y);
    }
  }

  return Point(-1, -1);
}

int Grid::sum_neighbour(int x, int y) {
  int sum = 0;
  Point origin = Point(x, y);
  Point adjacent;
  for (int i = Direction::N; i <= Direction::NW; i++) {
    adjacent = apply_direction(origin, (Direction)i);
    if (space[adjacent.first][adjacent.second]) {
      sum += 1;
    }
  }
  return sum;
}

void Grid::print(void) {
  std::cout << "\e[1;1H\e[2J";
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      std::cout << (space[x][y] ? "o " : ". ");
    }
    std::cout << "\n";
  }
}

void Grid::tick(void) {
  next_space = allocate_space();
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      int neighbours = sum_neighbour(x, y);
      next_space[x][y]
          = neighbours == 3 ? 1 : neighbours == 2 ? space[x][y] : 0;
    }
  }
  space = next_space;
}

int Grid::sum(void) {
  int sum = 0;
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      sum += space[x][y];
    }
  }
  return sum;
}

Point Grid::apply_direction(Point origin, Direction direction) {
  int x = origin.first;
  int y = origin.second;
  switch (direction) {
    case Direction::N:
      x += 1;
      break;
    case Direction::NE:
      x += 1;
      y += 1;
      break;
    case Direction::E:
      y += 1;
      break;
    case Direction::SE:
      x -= 1;
      y += 1;
      break;
    case Direction::S:
      x -= 1;
      break;
    case Direction::SW:
      x -= 1;
      y -= 1;
      break;
    case Direction::W:
      y -= 1;
      break;
    case Direction::NW:
      x += 1;
      y -= 1;
      break;
  }

  return Point((x + width) % width, (y + height) % height);
}