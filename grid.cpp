#include "grid.hpp"
#include "util.hpp"
#include <iostream>
#include <stdlib.h>

Grid::Grid(int width, int height) : width(width), height(height) {
  age = 0;
  space = Vector2D(width);
  for (int x = 0; x < width; x++) {
    space[x] = Vector(height);
  }
}

Grid::~Grid() {}

Point Grid::new_point(void) {
  int x = random(0, width);
  int y = random(0, height);
  return Point(x, y);
}

Point Grid::new_point(int x, int y) { return Point(x, y); }

void Grid::tick(void) {
  age += 1;
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
    }
  }
}

void Grid::print(void) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      std::cout << (space[x][y] ? "x" : ".");
    }
    std::cout << "\n";
  }
}