#include "grid.hpp"
#include <iostream>

void run(char** argv) {
  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  int ticks = atoi(argv[3]);

  Grid* grid = new Grid(width, height);

  for (int i = 0; i < ticks; i++) {
    grid->tick();
  }

  delete grid;
}

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cout << "not enough arguments\n"
              << "example: ./cellular_automata width height ticks\n";
  }

  run(argv);

  return 0;
}