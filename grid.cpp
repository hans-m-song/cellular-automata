#include "grid.hpp"

#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "metric.hpp"
#include "util.hpp"

Metric metric;

Grid::Grid(int width, int height, double initial_density)
    : width(width), height(height) {
  // begin measuring benchmarks
  metric.start(Measure::Init);

  // dynamically allocate space for generations
  generation = allocate_space();
  next_generation = allocate_space();

  // number of initial live cells
  int initial_cells = (int)(initial_density * width * height);

  // populate first generation
  Point point;
  for (int i = 0; i < initial_cells; i++) {
    point = empty_cell();
    generation[point.first * width + point.second] = 1;
  }

  metric.stop(Measure::Init);
}

Grid::~Grid() {
  delete[] generation;
  delete[] next_generation;
}

int* Grid::allocate_space(void) {
  int* space = new int[width * height];
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      space[x * width + y] = 0;
    }
  }
  return space;
}

Point Grid::empty_cell(void) {
  int max = width * height;
  int x, y;
  for (int i = 0; i < max; i++) {
    x = random(0, width - 1);
    y = random(0, height - 1);
    if (!generation[x * width + y]) {
      return Point(x, y);
    }
  }

  return Point(-1, -1);
}

int Grid::sum_neighbour(int x, int y) {
  int sum = 0;
  Point origin = Point(x, y);
  Point adjacent;
  // check each direction
  for (int i = Direction::N; i <= Direction::NW; i++) {
    adjacent = apply_direction(origin, (Direction)i);
    if (generation[adjacent.first * width + adjacent.second]) {
      sum += 1;
    }
  }
  return sum;
}

void Grid::print(void) {
#ifndef DEBUG
  std::cout << "\e[1;1H\e[2J";
#endif

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      std::cout << (generation[x * width + y] ? "o " : ". ");
    }
    std::cout << "\n";
  }
}

void Grid::run(int ticks) {
  metric.start(Measure::Run);

  for (int i = 0; i < ticks; i++) {
    tick();
#ifdef VISUAL
    print();
    usleep(100000);
#endif
  }

  metric.stop(Measure::Run);
}

void Grid::tick(void) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      int neighbours = sum_neighbour(x, y);
      if (neighbours == 3) {
        // if 3 live neighbours exactly, cell lives
        next_generation[x * width + y] = 1;
      } else if (neighbours == 2) {
        // if 2 live neighbours exactly, cell maintains status
        next_generation[x * width + y] = generation[x * width + y];
      } else {
        // otherwise else cell dies
        next_generation[x * width + y] = 0;
      }
    }
  }
  // swap currently active generation
  std::swap(generation, next_generation);
}

int Grid::sum(void) {
  int sum = 0;
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      sum += generation[x * width + y];
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

Metric run_grid(int width, int height, double initial_density, int ticks) {
  metric.start(Measure::Total);

  Grid* grid = new Grid(width, height, initial_density);
  grid->run(ticks);
  delete grid;

  metric.stop(Measure::Total);
  return metric;
}
