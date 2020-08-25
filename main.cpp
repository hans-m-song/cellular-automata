#include "grid.hpp"
#include <iostream>
#include <unistd.h>

void run(Grid* grid, int ticks) {
  for (int i = 0; i < ticks; i++) {
    grid->tick();
    grid->print();
    usleep(100000);
  }
}

int main(int argc, char** argv) {
  if (argc != 5) {
    std::cout << "incorrect number of arguments\n"
              << "example: ./cellular_automata width height density ticks\n";
    return 1;
  }

  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  double initial_density = atof(argv[3]);
  int ticks = atoi(argv[4]);

  Grid* grid = new Grid(width, height, initial_density);
  grid->print();
  run(grid, ticks);

  int initial_cells = (int)(initial_density * width * height);
  std::cout << "parameters: width=" << width << ", heigh=" << height
            << ", initial_density=" << initial_density << ", ticks=" << ticks
            << ", initial_cells=" << initial_cells
            << ", final_cells=" << grid->sum() << "\n";

  delete grid;
  return 0;
}