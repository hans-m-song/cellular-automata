#include "grid.hpp"
#include "util.hpp"
#include <stdlib.h>

Grid::Grid(int width, int height) : width(width), height(height) {
  age = 0;
  space = new int*[width];
  for (int i = 0; i < height; i++) {
    space[i] = new int[height];
  }
}

Grid::~Grid() {
  for (int i = 0; i < height; i++) {
    delete space[i];
  }
  delete space;
}

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