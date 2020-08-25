#include "grid.hpp"
#include <iostream>

Grid* setup(int width, int height) { return new Grid(width, height); }

void cleanup(Grid* grid) { delete grid; }

void run(Grid* grid, int ticks) {
  for (int i = 0; i < ticks; i++) {
    grid->tick();
  }
}

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cout << "not enough arguments\n"
              << "example: ./cellular_automata width height ticks\n";
    return 1;
  }

  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  int ticks = atoi(argv[3]);

  Grid* grid = setup(width, height);
  run(grid, ticks);
  grid->print();
  cleanup(grid);
  return 0;
}